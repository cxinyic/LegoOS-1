/*
 * Copyright (c) 2016-2018 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <lego/mm.h>
#include <lego/pid.h>
#include <lego/slab.h>
#include <lego/files.h>
#include <lego/sched.h>
#include <lego/futex.h>
#include <lego/completion.h>
#include <lego/kernel.h>
#include <lego/syscalls.h>
#include <lego/fit_ibapi.h>
#include <lego/signalfd.h>
#include <lego/timekeeping.h>
#include <processor/dependency_track.h>

#include <processor/processor.h>
#include <processor/pcache.h>
#include <processor/distvm.h>

#include <asm/pgalloc.h>
#include <asm/fpu/internal.h>

/* Initialized by the architecture: */
int arch_task_struct_size __read_mostly = sizeof(struct task_struct);
int arch_task_struct_order __read_mostly = get_order(sizeof(struct task_struct));

DEFINE_SPINLOCK(tasklist_lock);
unsigned long total_forks;
int nr_threads;			/* The idle threads do not count.. */

static inline struct task_struct *alloc_task_struct_node(int node)
{
	unsigned long p;

	p = __get_free_pages(GFP_KERNEL, arch_task_struct_order);
	return (struct task_struct *)p;
}

static inline void free_task_struct(struct task_struct *tsk)
{
	free_pages((unsigned long)tsk, arch_task_struct_order);
}

static inline void put_signal_struct(struct signal_struct *sig)
{
	if (atomic_dec_and_test(&sig->sigcnt))
		kfree(sig);
}

static inline unsigned long *
alloc_thread_stack_node(struct task_struct *tsk, int node)
{
	struct page *page;

	page = alloc_pages_node(node, GFP_KERNEL | __GFP_ZERO, THREAD_SIZE_ORDER);
	return page ? page_address(page) : NULL;
}

static inline void free_thread_stack(struct task_struct *tsk)
{
	__free_pages(virt_to_page(tsk->stack), THREAD_SIZE_ORDER);
}

/*
 * Setup stack end magic number
 * for overflow detection
 */
void setup_task_stack_end_magic(struct task_struct *tsk)
{
	unsigned long *stackend;

	stackend = end_of_stack(tsk);
	*stackend = STACK_END_MAGIC;
}

void __put_task_struct(struct task_struct *tsk)
{
	WARN_ON(atomic_read(&tsk->usage));
	WARN_ON(tsk == current);
	WARN_ON(tsk->state != TASK_DEAD);

	put_signal_struct(tsk->signal);
	free_thread_stack(tsk);
	tsk->stack = NULL;
	free_task_struct(tsk);
}

static void setup_thread_stack(struct task_struct *p, struct task_struct *org)
{
	/* Duplicate whole stack! */
	*task_thread_info(p) = *task_thread_info(org);

	/* Make the `current' macro work */
	task_thread_info(p)->task = p;
}

/*
 * Duplicate a new task_struct based on parent task_struct.
 * Allocate a new kernel stack and setup stack_info to make current work.
 */
static struct task_struct *dup_task_struct(struct task_struct *old, int node)
{
	struct task_struct *new;
	unsigned long *stack;
	int err;

	new = alloc_task_struct_node(node);
	if (!new)
		return NULL;

	stack = alloc_thread_stack_node(new, node);
	if (!stack)
		goto free_task;

	err = arch_dup_task_struct(new, old);

	/*
	 * arch_dup_task_struct() clobbers the stack-related fields.
	 * Make sure they're properly initialized before using any
	 * stack-related functions again.
	 */
	new->stack = stack;
	if (err)
		goto free_stack;

	setup_thread_stack(new, old);
	clear_tsk_need_resched(new);
	setup_task_stack_end_magic(new);

	/*
	 * One for us,
	 * one for whoever does the "release_task()" (usually parent)
	 */
	atomic_set(&new->usage, 2);

	new->wake_q.next = NULL;

#ifdef CONFIG_CHECKPOINT
	atomic_set(&new->pm_data.process_barrier, 0);
#endif

	return new;

free_stack:
	free_thread_stack(new);
free_task:
	free_task_struct(new);
	return NULL;
}

static void free_task(struct task_struct *tsk)
{
	if (WARN_ON(tsk->state != TASK_DEAD))
		return;

	free_thread_stack(tsk);
	tsk->stack = NULL;
	free_task_struct(tsk);
}

static inline void mm_free_pgd(struct mm_struct *mm)
{
	pgd_free(mm, mm->pgd);
}

static void check_mm(struct mm_struct *mm)
{
	/* TODO: check nr_ptes */
}

/*
 * Called when the last reference to the mm is dropped:
 * either by a lazy thread or by mmput (mm_users=0, mm_count=0).
 * Free the page directory and the mm.
 */
void __mmdrop(struct mm_struct *mm)
{
	BUG_ON(mm == &init_mm);

	/* Processor: Free distributed VMA resource */
	processor_distvm_exit(mm);

	mm_free_pgd(mm);
	check_mm(mm);
	kfree(mm);
}

/*
 * One process holds 1 mm_count.
 * Each thread within a process holds 1 mm_users.
 *
 * Call when the final thread of a process exit, thus mm_users=0
 *
 * However, this @mm may still have other users such as other kernel
 * process. So the final free of @mm itself is made possible by mmdrop(),
 * which decrement the mm_users.
 *
 * Although Lego does not share user mm with kernel process,
 * also there is no active_mm in Lego. But we follow the old code flow.
 */
static inline void __mmput(struct mm_struct *mm)
{
	BUG_ON(atomic_read(&mm->mm_users));
	BUG_ON(mm == &init_mm);

	/* Remove leftover of this process in pcache */
	pcache_process_exit(current);

	/* dec mm->mm_count */
	mmdrop(mm);
}

/*
 * Decrement the use count and release all resources for an mm
 * if this is the last user.
 */
void mmput(struct mm_struct *mm)
{
	if (atomic_dec_and_test(&mm->mm_users))
		__mmput(mm);
}

/**
 * get_task_mm - acquire a reference to the task's mm
 *
 * Returns %NULL if the task has no mm.  Checks PF_KTHREAD (meaning
 * this kernel workthread has transiently adopted a user mm with use_mm,
 * to do its AIO) is not set and if so returns a reference to it, after
 * bumping up the use count.  User must release the mm via mmput()
 * after use.  Typically used by /proc and ptrace.
 */
struct mm_struct *get_task_mm(struct task_struct *task)
{
	struct mm_struct *mm;

	task_lock(task);
	mm = task->mm;
	if (mm) {
		if (task->flags & PF_KTHREAD)
			mm = NULL;
		else
			atomic_inc(&mm->mm_users);
	}
	task_unlock(task);
	return mm;
}

static void complete_vfork_done(struct task_struct *tsk)
{
	struct completion *vfork;

	task_lock(tsk);
	vfork = tsk->vfork_done;
	if (likely(vfork)) {
		tsk->vfork_done = NULL;
		complete(vfork);
	}
	task_unlock(tsk);
}

static int wait_for_vfork_done(struct task_struct *child,
				struct completion *vfork)
{
	int killed;

	killed = wait_for_completion_killable(vfork);

	if (unlikely(killed)) {
		/* child was killded by signal */
		task_lock(child);
		child->vfork_done = NULL;
		task_unlock(child);
	}

	put_task_struct(child);
	return killed;
}

/* Please note the differences between mmput and mm_release.
 * mmput is called whenever we stop holding onto a mm_struct,
 * error success whatever.
 *
 * mm_release is called after a mm_struct has been removed
 * from the current process.
 *
 * This difference is important for error handling, when we
 * only half set up a mm_struct for a new process and need to restore
 * the old one.  Because we mmput the new mm_struct before
 * restoring the old one. . .
 * Eric Biederman 10 January 1998
 */
void mm_release(struct task_struct *tsk, struct mm_struct *mm)
{
#ifdef CONFIG_FUTEX
	/* Get rid of any futexes when releasing the mm */
	if (unlikely(tsk->robust_list)) {
		exit_robust_list(tsk);
		tsk->robust_list = NULL;
	}
#endif

	/* Get rid of any cached register state */
	deactivate_mm(tsk, mm);

	/* Signal userspace we are exiting: */
	if (tsk->clear_child_tid) {
		if (atomic_read(&mm->mm_users) > 1) {
			/*
			 * We don't check the error code - if userspace has
			 * not set up a proper pointer then tough luck.
			 */
			put_user(0, tsk->clear_child_tid);
			sys_futex(tsk->clear_child_tid, FUTEX_WAKE,
					1, NULL, NULL, 0);
		}
		tsk->clear_child_tid = NULL;
	}

	/*
	 * All done, finally we can wake up parent and return this mm to him.
	 * Also kthread_stop() uses this completion for synchronization.
	 */
	if (tsk->vfork_done)
		complete_vfork_done(tsk);
}

static struct mm_struct *mm_init(struct mm_struct *mm, struct task_struct *p)
{
	atomic_set(&mm->mm_users, 1);
	atomic_set(&mm->mm_count, 1);
	mm->map_count = 0;
	mm->pinned_vm = 0;
	mm_init_cpumask(mm);
	spin_lock_init(&mm->page_table_lock);
	init_rwsem(&mm->mmap_sem);

	/*
	 * pgd_alloc() will duplicate the identity kernel mapping
	 * but leaves other entries empty:
	 */
	mm->pgd = pgd_alloc(mm);
	if (unlikely(!mm->pgd)) {
		kfree(mm);
		return NULL;
	}

	/* Processor: init distributed VMA resource */
	if (processor_distvm_init(mm, get_memory_home_node(p))) {
		pgd_free(mm, mm->pgd);
		kfree(mm);
		return NULL;
	}

	return mm;
}

/*
 * Allocate and initialize an mm_struct.
 */
struct mm_struct *mm_alloc(void)
{
	struct mm_struct *mm;

	mm = kmalloc(sizeof(*mm), GFP_KERNEL);
	if (!mm)
		return NULL;

	memset(mm, 0, sizeof(*mm));
	return mm_init(mm, current);
}

/*
 * Allocate a new mm structure and copy contents from the
 * mm structure of the passed in task structure.
 */
static struct mm_struct *dup_mm_struct(struct task_struct *tsk)
{
	struct mm_struct *mm, *oldmm;

	oldmm = current->mm;

	mm = kmalloc(sizeof(*mm), GFP_KERNEL);
	if (!mm)
		return NULL;

	memcpy(mm, oldmm, sizeof(*mm));

	if (!mm_init(mm, tsk))
		goto out;

	processor_fork_dup_distvm(tsk, mm, oldmm);
	return mm;

out:
	mmput(mm);
	return NULL;
}

/*
 * mm_struct does not handle user virtual memory
 * so no need to copy all mmap:
 */
static int copy_mm(unsigned long clone_flags, struct task_struct *tsk)
{
	struct mm_struct *mm, *oldmm;

	tsk->mm = tsk->active_mm = NULL;
	tsk->nvcsw = tsk->nivcsw = 0;

	oldmm = current->mm;

	/*
	 * In lego, even kernel thread has a mm
	 * We don't steal mm.
	 */
	BUG_ON(!oldmm);

	if (clone_flags & CLONE_VM) {
		atomic_inc(&oldmm->mm_users);
		mm = oldmm;
		goto good_mm;
	}

	mm = dup_mm_struct(tsk);

	if (!mm)
		return -ENOMEM;

good_mm:
	tsk->mm = mm;
	tsk->active_mm = mm;

	return 0;
}

static void close_files(struct files_struct *files)
{
	int fd;
	struct file *f;

	spin_lock(&files->file_lock);
	for_each_set_bit(fd, files->fd_bitmap, NR_OPEN_DEFAULT) {
		f = files->fd_array[fd];
		BUG_ON(!f);

		/*
		 * XXX:
		 * If the file is shared by more than one processes,
		 * the f_op->release might be called more than once.
		 * This more be valid for pipe files, but does this
		 * also applies to all other kind of files?
		 *
		 * We need to be careful here! Check later.
		 */
		if (f->f_op->release)
			f->f_op->release(f);
		put_file(f);
		__clear_bit(fd, files->fd_bitmap);
		files->fd_array[fd] = NULL;
	}
	spin_unlock(&files->file_lock);
}

static void put_files_struct(struct files_struct *files)
{
	if (atomic_dec_and_test(&files->count)) {
		close_files(files);
		kfree(files);
	}
}

void exit_files(struct task_struct *tsk)
{
	struct files_struct * files = tsk->files;

	if (files) {
		task_lock(tsk);
		tsk->files = NULL;
		task_unlock(tsk);
		put_files_struct(files);
	}
}

/*
 * Allocate a new files structure and copy contents from the
 * passed in files structure.
 */
/*struct file_reduced{
    fmode_t f_mode;
    atomic_t f_count;
    unsigned int f_flags;
    loff_t f_pos;
    char f_name[FILENAME_LEN_DEFAULT];
    int fd;
    const struct file_operations *f_op;
    int ready_state;
    int ready_size;
};*/
static struct files_struct *dup_fd(struct files_struct *oldf)
{
	struct files_struct *newf;
	int fd;

	newf = kzalloc(sizeof(*newf), GFP_KERNEL);
	if (!newf)
		return NULL;

	atomic_set(&newf->count, 1);
	spin_lock_init(&newf->file_lock);
	bitmap_copy(newf->close_on_exec, oldf->close_on_exec, NR_OPEN_DEFAULT);

	/* Copy fd bitmap and get each open file */
	spin_lock(&oldf->file_lock);
	bitmap_copy(newf->fd_bitmap, oldf->fd_bitmap, NR_OPEN_DEFAULT);
	printk("close on exec size is %d\n", sizeof(*(newf->close_on_exec)));
	printk("fd_bitmap size is %d\n", sizeof(*(newf->fd_bitmap)));
	for_each_set_bit(fd, newf->fd_bitmap, NR_OPEN_DEFAULT) {
		struct file *f = oldf->fd_array[fd];

		BUG_ON(!f);
		newf->fd_array[fd] = f;
		get_file(f);
		if (f->f_op != NULL){
			printk("f_op address is %lx\n", f->f_op);
			if (f->f_op->open != NULL)
			{printk("open address is %lx\n", f->f_op->open);}
		}
		if (f->private_data!=NULL){
			{printk("f->private_data is %lx\n", f->private_data);}
		}

		/*
		 * Let special files know what is going on
		 * - pipe
		 * - socket?
		 */
		if (pipe_file(f->f_name))
		{	
			f->f_op->open(f);
		}
	}
	spin_unlock(&oldf->file_lock);

	return newf;
}

static struct files_struct *restore_fd(struct file_system *fs)
{
	struct files_struct *newf;
	struct file_reduced *tmp_file;
	struct file_system_reduced *tmp_fs;
	tmp_file = kmalloc(sizeof(*tmp_file), GFP_KERNEL);
	tmp_fs = kmalloc(sizeof(*tmp_fs), GFP_KERNEL);
	int fd;
	newf = kzalloc(sizeof(*newf), GFP_KERNEL);
	if (!newf)
		return NULL;

	atomic_set(&newf->count, 1);
	spin_lock_init(&newf->file_lock);
	void * files_meta;
	files_meta = kmalloc(4096, GFP_KERNEL);
	read_files_value(files_meta);

	memcpy(tmp_fs, files_meta, sizeof(*tmp_fs));
	fs->users = tmp_fs->users;
	fs->umask = tmp_fs->umask;
	memcpy(fs->cwd, tmp_fs->cwd, FILENAME_LEN_DEFAULT);
	memcpy(fs->root, tmp_fs->root, FILENAME_LEN_DEFAULT);
	spin_lock_init(&fs->lock);
	int size = sizeof(*tmp_fs);


	
	memcpy(newf->close_on_exec, files_meta+size, 8);
	memcpy(newf->fd_bitmap, files_meta+8+size, 8);
	int i = 0;
	size+=16;

	
	for_each_set_bit(fd, newf->fd_bitmap, NR_OPEN_DEFAULT) {
		printk("restore_fd, new i is %d\n",i);
		memcpy(tmp_file, files_meta+size, sizeof(*tmp_file));
		size += sizeof(*tmp_file);
		struct file *f;
		f = kmalloc(sizeof(*f), GFP_KERNEL);
		f->f_mode = tmp_file->f_mode;
		f->f_count = tmp_file->f_count;
		f->f_flags = tmp_file->f_flags;
		f->f_pos = tmp_file->f_pos;
		memcpy(f->f_name, tmp_file->f_name, FILENAME_LEN_DEFAULT);
		f->fd = tmp_file->fd;
		f->f_op = tmp_file->f_op;
		f->ready_state = tmp_file->ready_state;
		f->ready_size = tmp_file->ready_size;
		f->size_private_data = tmp_file->size_private_data;
		spin_lock_init(&f->f_pos_lock);
#ifdef CONFIG_EPOLL
		INIT_LIST_HEAD(&f->f_epi_links);
#endif 
		INIT_LIST_HEAD(&f->f_poll_links);
		newf->fd_array[fd] = f;
		get_file(f);
		if (pipe_file(f->f_name))
		{	
			f->f_op->open(f);
		}
		i = i+1;
	}
	for_each_set_bit(fd, newf->fd_bitmap, NR_OPEN_DEFAULT) {
		struct file *f;
		f = newf->fd_array[fd];
		if (f->size_private_data>0){
			void * data;
			data = kmalloc(f->size_private_data, GFP_KERNEL);
			memcpy(data, files_meta+size, f->size_private_data);
			size+=f->size_private_data;
			f->private_data = data;
		}
	}

	/*for_each_set_bit(fd, oldf->fd_bitmap, NR_OPEN_DEFAULT) {
		struct file *f = oldf->fd_array[fd];
		printk("restore_fd, new mode i is %d\n",f->f_mode);
	}*/
	
	return newf;

	
}

static int copy_fs(struct task_struct *tsk){
	strcpy(tsk->fs.cwd, current->fs.cwd);
	strcpy(tsk->fs.root, current->fs.root);
	memcpy((void*)(&tsk->fs.lock), (void*)(&current->fs.lock), sizeof(tsk->fs.lock));
	tsk->fs.umask = current->fs.umask;
	tsk->fs.users = current->fs.users;
	return 0;
}

static int copy_files(unsigned long clone_flags, struct task_struct *tsk)
{
	struct files_struct *oldf, *newf;
	int ret = 0;

	oldf = current->files;
	BUG_ON(!oldf);

	if (clone_flags & CLONE_FILES) {
		atomic_inc(&oldf->count);
		/* already copied the value in dup_task_struct */
		goto out;
	}

	newf = dup_fd(oldf);
	if (!newf) {
		ret = -ENOMEM;
		goto out;
	}

	tsk->files = newf;
	ret = 0;
out:
	return ret;
}

/*
 * Copy credentials for the new process created by fork()
 *
 * We share if we can, but under some circumstances we have to generate a new
 * set.
 *
 * The new process gets the current process's subjective credentials as its
 * objective and subjective credentials
 */
static int copy_creds(struct task_struct *p, unsigned long clone_flags)
{
	struct cred *new;
	struct cred *old = current->cred;

	if (clone_flags & CLONE_THREAD) {
		p->real_cred = get_cred(p->cred);
		p->cred = get_cred(p->cred);
		return 0;
	}

	new = kmalloc(sizeof(*new), GFP_KERNEL);
	if (!new)
		return -ENOMEM;

	*new = *old;
	atomic_set(&new->usage, 0);

	p->real_cred = get_cred(p->cred);
	p->cred = get_cred(p->cred);

	return 0;
}

static void exit_creds(struct task_struct *tsk)
{
	put_cred(tsk->real_cred);
	tsk->real_cred = NULL;

	put_cred(tsk->cred);
	tsk->cred = NULL;
}

static int copy_sighand(unsigned long clone_flags, struct task_struct *tsk)
{
	struct sighand_struct *sig;

	if (clone_flags & CLONE_SIGHAND) {
		atomic_inc(&current->sighand->count);
		return 0;
	}

	sig = kzalloc(sizeof(*sig), GFP_KERNEL);
	if (!sig)
		return -ENOMEM;

	/*
	 * Actions are inherited from parent even if
	 * we are going to create a new process:
	 */
	memcpy(sig->action, current->sighand->action, sizeof(sig->action));
	spin_lock_init(&sig->siglock);
	atomic_set(&sig->count, 1);
	init_waitqueue_head(&sig->signalfd_wqh);

	tsk->sighand = sig;
	return 0;
}

void __cleanup_sighand(struct sighand_struct *sighand)
{
	if (atomic_dec_and_test(&sighand->count)) {
		signalfd_cleanup(sighand);
		kfree(sighand);
	}
}

static int copy_signal(unsigned long clone_flags, struct task_struct *tsk)
{
	struct signal_struct *sig;

	if (clone_flags & CLONE_THREAD) {
		/* sigcnt will be incremented later in copy_process */
		return 0;
	}

	sig = kzalloc(sizeof(*sig), GFP_KERNEL);
	if (!sig)
		return -ENOMEM;

	tsk->signal = sig;

	sig->nr_threads = 1;
	atomic_set(&sig->live, 1);
	atomic_set(&sig->sigcnt, 1);

	/* list_add(thread_node, thread_head) without INIT_LIST_HEAD() */
	sig->thread_head = (struct list_head)LIST_HEAD_INIT(tsk->thread_node);
	tsk->thread_node = (struct list_head)LIST_HEAD_INIT(sig->thread_head);

	init_waitqueue_head(&sig->wait_chldexit);
	sig->curr_target = tsk;
	init_sigpending(&sig->shared_pending);
	INIT_LIST_HEAD(&sig->posix_timers);
	spin_lock_init(&sig->stats_lock);
	init_timer(&sig->real_timer);
	sig->real_timer.function = it_real_fn;

	task_lock(current->group_leader);
	memcpy(sig->rlim, current->signal->rlim, sizeof sig->rlim);
	task_unlock(current->group_leader);

	return 0;
}

/*
 * This creates a new process as a copy of the old one,
 * but does not actually start it yet.
 *
 * It copies the registers, and all the appropriate
 * parts of the process environment (as per the clone
 * flags). The actual kick-off is left to the caller.
 */



struct task_struct *copy_process(unsigned long clone_flags,
				 unsigned long stack_start,
				 unsigned long stack_size,
				 int __user *child_tidptr,
				 unsigned long tls, int node)
{
	struct task_struct *p;
	int retval;
	int pid = 0;
	unsigned long flags;

	if (clone_flags & (CLONE_NEWCGROUP | CLONE_NEWNS | CLONE_NEWUSER |
			   CLONE_NEWPID | CLONE_NEWNET | CLONE_NEWUTS | CLONE_NEWIPC)) {
		pr_info("Unsupported flags detected: %#lx\n", clone_flags);
		WARN_ON(1);
		return ERR_PTR(-EINVAL);
	}

	/*
	 * Thread groups must share signals as well, and detached threads
	 * can only be started up within the thread group.
	 */
	if ((clone_flags & CLONE_THREAD) && !(clone_flags & CLONE_SIGHAND))
		return ERR_PTR(-EINVAL);

	/*
	 * Shared signal handlers imply shared VM. By way of the above,
	 * thread groups also imply shared VM. Blocking this case allows
	 * for various simplifications in other code.
	 */
	if ((clone_flags & CLONE_SIGHAND) && !(clone_flags & CLONE_VM))
		return ERR_PTR(-EINVAL);

	/*
	 * Siblings of global init remain as zombies on exit since they are
	 * not reaped by their parent (swapper). To solve this and to avoid
	 * multi-rooted process trees, prevent global and container-inits
	 * from creating siblings.
	 */
	if ((clone_flags & CLONE_PARENT) &&
				current->signal->flags & SIGNAL_UNKILLABLE)
		return ERR_PTR(-EINVAL);

	/* Duplicate task_struct and create new stack */
	
	
	
	p = dup_task_struct(current, node);
	
	if (!p)
		return ERR_PTR(-ENOMEM);

	/* rt_mutex and PI stuff */
	spin_lock_init(&p->pi_lock);


	p->flags &= ~(PF_SUPERPRIV | PF_WQ_WORKER | PF_IDLE);
	p->flags |= PF_FORKNOEXEC;
	INIT_LIST_HEAD(&p->children);
	INIT_LIST_HEAD(&p->sibling);
	INIT_LIST_HEAD(&p->thread_group);
	p->vfork_done = NULL;
	p->pdeath_signal = 0;
	spin_lock_init(&p->alloc_lock);

	init_sigpending(&p->pending);

	p->utime = p->stime = p->gtime = 0;
	p->start_time = ktime_get_ns();
	p->real_start_time = ktime_get_boot_ns();
	p->pagefault_disabled = 0;
	p->fs = current->fs;
	
	if (!(clone_flags & CLONE_IDLE_THREAD)) {
		pid = alloc_pid(p);
		if (!pid)
			goto out_cleanup_thread;
	}
	
	p->pid = pid;
/*#ifdef CONFIG_COMP_PROCESSOR
	if(pid == 25){
		printk("pid is 25\n");
		p->fs = current_tsk->fs;
	}
	else{
		printk("pid is not 25\n");
	}
#endif*/
	if(pid == 25){
		clone_flags |= CLONE_GLOBAL_THREAD;
	}
	/*if(pid == 25){
		arch_dup_task_struct(p, current_tsk);
	}*/


	/*
	 * Now do the dirty work.
	 */
#ifdef CONFIG_COMP_PROCESSOR
	if (pid == 25){
		/*strcpy(p->fs.cwd, current_tsk->fs.cwd);
		strcpy(p->fs.root, current_tsk->fs.root);
		memcpy((void*)(&p->fs.lock), (void*)(&current_tsk->fs.lock), sizeof(p->fs.lock));
		p->fs.umask = current_tsk->fs.umask;
		p->fs.users = current_tsk->fs.users;*/
		retval = 0;

	}
	else{
		retval = copy_fs(p);
	}
#endif
	if (retval < 0)
		goto out_free;

	/*if (pid == 25){
		struct cred *new;
		struct cred *old = current_tsk->cred;

		if (clone_flags & CLONE_THREAD) {
			p->real_cred = get_cred(p->cred);
			p->cred = get_cred(p->cred);
		}
		

		new = kmalloc(sizeof(*new), GFP_KERNEL);

		*new = *old;
		atomic_set(&new->usage, 0);

		p->real_cred = get_cred(p->cred);
		p->cred = get_cred(p->cred);
		retval = 0;

	}
	else{*/
		retval = copy_creds(p, clone_flags);
	// }
	
	if (retval < 0)
		goto out_free;

	/* Perform scheduler related setup. Assign this task to a CPU. */
	retval = setup_sched_fork(clone_flags, p);
	if (retval)
		goto out_cleanup_creds;

	if(pid == 25){
#ifdef CONFIG_COMP_PROCESSOR
		struct files_struct *oldf, *newf;
		oldf = current_tsk->files;
		struct file_system* fs;
		fs = kmalloc(sizeof(*fs), GFP_KERNEL);
		newf = restore_fd(fs);
		p->fs = fs;
		p->files = newf;
		retval = 0;
#endif
	}
	else{
		retval = copy_files(clone_flags, p);
	}
	if (retval)
		goto out_cleanup_sched;
	
	/*if(pid == 25){
#ifdef CONFIG_COMP_PROCESSOR
		struct sighand_struct *sig;
		sig = kzalloc(sizeof(*sig), GFP_KERNEL);
		memcpy(sig->action, current_tsk->sighand->action, sizeof(sig->action));
		spin_lock_init(&sig->siglock);
		atomic_set(&sig->count, 1);
		init_waitqueue_head(&sig->signalfd_wqh);	
		p->sighand = sig;
#endif
	}
	else{*/
		retval = copy_sighand(clone_flags, p);
	// }
	if (retval)
		goto out_cleanup_files;
	
	/*if(pid == 25){
#ifdef CONFIG_COMP_PROCESSOR
		struct signal_struct *sig;
		sig = kzalloc(sizeof(*sig), GFP_KERNEL);
		p->signal = sig;

		sig->nr_threads = 1;
		atomic_set(&sig->live, 1);
		atomic_set(&sig->sigcnt, 1);

		sig->thread_head = (struct list_head)LIST_HEAD_INIT(p->thread_node);
		p->thread_node = (struct list_head)LIST_HEAD_INIT(sig->thread_head);

		init_waitqueue_head(&sig->wait_chldexit);
		sig->curr_target = p;
		init_sigpending(&sig->shared_pending);
		INIT_LIST_HEAD(&sig->posix_timers);
		spin_lock_init(&sig->stats_lock);
		init_timer(&sig->real_timer);
		sig->real_timer.function = it_real_fn;

		task_lock(current_tsk->group_leader);
		memcpy(sig->rlim, current_tsk->signal->rlim, sizeof sig->rlim);
		task_unlock(current_tsk->group_leader);
#endif
	}
	else{*/
		retval = copy_signal(clone_flags, p);
	// }

	if (retval)
		goto out_cleanup_sighand;
/*	if(pid==25){

#ifdef CONFIG_COMP_PROCESSOR
		deptrack_restore_files(p, current_info.pss);
		deptrack_restore_signals(p, current_info.pss);
#endif

	}*/

	/*
	 * copy_mm may use the memory home node of p
	 * to setup a new dist_vma. So initialzie
	 * the processor_data first.
	 */
	/*if(pid == 25){
#ifdef CONFIG_COMP_PROCESSOR
		fork_processor_data(p, current_tsk, clone_flags);
#endif
	}
	else{*/
		fork_processor_data(p, current, clone_flags);
	// }
	
	/*if(pid == 25){
#ifdef CONFIG_COMP_PROCESSOR
		struct mm_struct *mm, *oldmm;
		p->mm = p->active_mm = NULL;
		p->nvcsw = p->nivcsw = 0;
		oldmm = current_tsk->mm;	
		mm = kmalloc(sizeof(*mm), GFP_KERNEL);
		memcpy(mm, oldmm, sizeof(*mm));
		mm_init(mm, p);
		processor_fork_dup_distvm(p, mm, oldmm);
		p->mm = mm;
		p->active_mm = mm;
		retval = 0;
#endif
	}
	else{*/
	    printk("size of mm stuct is %d",sizeof(*(current->mm)));
		retval = copy_mm(clone_flags, p);
	// }
	if (retval)
		goto out_cleanup_signal;

	retval = copy_thread_tls(clone_flags, stack_start, stack_size, p, tls);
	if (retval)
		goto out_cleanup_mm;

	/* clone idle thread, whose pid is 0 */
	

	p->set_child_tid = (clone_flags & CLONE_CHILD_SETTID) ? child_tidptr : NULL;
	/*
	 * Clear TID on mm_release()?
	 */
	p->clear_child_tid = (clone_flags & CLONE_CHILD_CLEARTID) ? child_tidptr : NULL;

#ifdef CONFIG_FUTEX
	p->robust_list = NULL;
#endif

	/*
	 * sigaltstack should be cleared when sharing the same VM
	 */
	if ((clone_flags & (CLONE_VM|CLONE_VFORK)) == CLONE_VM)
		sas_ss_reset(p);

	/* ok, now we should be set up.. */
	
	if (clone_flags & CLONE_THREAD) {
		p->exit_signal = -1;
		p->group_leader = current->group_leader;
		p->tgid = current->tgid;
	} else {
		if (clone_flags & CLONE_PARENT)
			p->exit_signal = current->group_leader->exit_signal;
		else
			p->exit_signal = (clone_flags & CSIGNAL);
		p->group_leader = p;
		p->tgid = p->pid;
	}

	/*
	 * Make it visible to the rest of the system, but dont wake it up yet.
	 * Need tasklist lock for parent etc handling!
	 */
	spin_lock_irqsave(&tasklist_lock, flags);

	/* CLONE_PARENT re-uses the old parent */
	if (clone_flags & (CLONE_PARENT|CLONE_THREAD)) {
		p->real_parent = current->real_parent;
		p->parent_exec_id = current->parent_exec_id;
	} else {
		p->real_parent = current;
		p->parent_exec_id = current->self_exec_id;
	}

	spin_lock(&current->sighand->siglock);

	/*
	 * Process group and session signals need to be delivered to just the
	 * parent before the fork or both the parent and the child after the
	 * fork. Restart if a signal comes in before we add the new process to
	 * it's process group.
	 * A fatal signal pending means that current will exit, so the new
	 * thread can't slip out of an OOM kill (or normal SIGKILL).
	*/
	recalc_sigpending();
	if (signal_pending(current)) {
		WARN_ON(1);
		retval = -ERESTARTNOINTR;
		goto out;
	}

	if (likely(p->pid)) {
		/* ptrace related init */
		INIT_LIST_HEAD(&p->ptrace_entry);
		INIT_LIST_HEAD(&p->ptraced);
		p->parent = p->real_parent;
		p->jobctl = 0;
		p->ptrace = 0;

		if (thread_group_leader(p)) {
			p->signal->leader_pid = pid;
			list_add_tail(&p->sibling, &p->real_parent->children);
			list_add_tail(&p->tasks, &init_task.tasks);
		} else {
			current->signal->nr_threads++;
			atomic_inc(&current->signal->live);
			atomic_inc(&current->signal->sigcnt);
			list_add_tail(&p->thread_group,
					  &p->group_leader->thread_group);
			list_add_tail(&p->thread_node,
					  &p->signal->thread_head);
		}
		nr_threads++;
	}

	total_forks++;
	spin_unlock(&current->sighand->siglock);
	spin_unlock_irqrestore(&tasklist_lock, flags);

	/*
	 * Okay, this new thread has been setup fully.
	 * Now we callback to strace.
	 */
	retval = fork_processor_strace(p);
	if (retval) {
		/* need decent cleanup here */
		WARN_ON(1);
		goto out_cleanup_thread;
	}

	return p;

out:
	spin_unlock(&current->sighand->siglock);
	spin_unlock_irqrestore(&tasklist_lock, flags);

out_cleanup_thread:
	exit_thread(p);
out_cleanup_mm:
	if (p->mm)
		mmput(p->mm);
out_cleanup_signal:
	if (!(clone_flags & CLONE_THREAD))
		put_signal_struct(p->signal);
out_cleanup_sighand:
	__cleanup_sighand(p->sighand);
out_cleanup_files:
	exit_files(p);
out_cleanup_sched:
	sched_remove_from_rq(p);
out_cleanup_creds:
	exit_creds(p);
out_free:
	p->state = TASK_DEAD;
	free_task(p);
	free_pid(pid);

	return ERR_PTR(retval);;
}

/*
 * Lego's main fork-routine
 */

pid_t do_fork(unsigned long clone_flags,
	      unsigned long stack_start,
	      unsigned long stack_size,
	      int __user *parent_tidptr,
	      int __user *child_tidptr,
	      unsigned long tls)
{
	struct task_struct *p;
	struct completion vfork;
	pid_t pid;
	printk("do_fork here\n");


	p = copy_process(clone_flags, stack_start, stack_size,
			 child_tidptr, tls, NUMA_NO_NODE);
	if (IS_ERR(p))
		return PTR_ERR(p);

	/*
	 * Tell remote memory component.
	 *
	 * Do this prior waking up the new thread - the thread pointer
	 * might get invalid after that point, if the thread exits quickly.
	 */
#ifdef CONFIG_COMP_PROCESSOR
    if (p->pid == 25){
		clone_flags |= CLONE_GLOBAL_THREAD;
	}
	if (clone_flags & CLONE_GLOBAL_THREAD) {
		void *vmainfo;
		int ret;

		vmainfo = p2m_fork(p, clone_flags);
		if (IS_ERR(vmainfo)) {
			WARN_ON_ONCE(1);
			return PTR_ERR(vmainfo);
		}

		/*
		 * This step has to be postponed here after
		 * we got VMA info from remote memory.
		 * Walk through page table entries.
		 */
		/*if(p->pid == 25){
			ret = fork_dup_pcache(p, p->mm, current_tsk->mm, vmainfo);
			printk("pid 25 ret is %d\n", ret);
		}
		else{
			ret = fork_dup_pcache(p, p->mm, current->mm, vmainfo);
		}
		if (ret) {
			WARN_ON_ONCE(1);
			return ret;
		}*/
	}
#endif

	pid = p->pid;
	if (clone_flags & CLONE_PARENT_SETTID)
		put_user(pid, parent_tidptr);

	if (clone_flags & CLONE_VFORK) {
		p->vfork_done = &vfork;
		init_completion(&vfork);
		get_task_struct(p);
	}
	
	printk("Pid is %d\n", p->pid);
	printk("sp is %lx\n", task_pt_regs(p)->sp);
	printk("ip is %lx\n", task_pt_regs(p)->ip);
	printk("cs is %lx\n", task_pt_regs(p)->cs);
	/*
	if (p->pid == 25){
		deptrack_restore_files(current_info.pss);
		deptrack_restore_signals(current_info.pss);
		struct ss_task_struct *ss_task, *ss_tasks = current_info.pss->tasks;
		ss_task = &ss_tasks[0];
		deptrack_restore_thread_state(p, ss_task);
		
		printk("clone flags is %lx\n", clone_flags);
		printk("stack_start is %lx\n", stack_start);
		printk("stack_size is %lx\n", stack_size);
		printk("child_tidptr is %lx\n", child_tidptr);
		printk("tls is %lx\n", tls);


		printk("2525\n");
	}
	

    printk("sp is %lx\n", task_pt_regs(p)->sp);
	printk("ip is %lx\n", task_pt_regs(p)->ip);
	printk("cs is %lx\n", task_pt_regs(p)->cs);*/
#ifdef CONFIG_COMP_PROCESSOR
	p->pm_data.home_node = 1;
#endif
	wake_up_new_task(p);

	if (clone_flags & CLONE_VFORK)
		wait_for_vfork_done(p, &vfork);


	pr_info("[%d] forks [%d]", current->pid, p->pid);

	return p->pid;
}

/**
 * kernel_thread	-	Create a kernel thread
 * @fn: the function to run in the thread
 * @arg: data pointer for @fn()
 * @flags: CLONE flags
 *
 * Create a new kernel thread and put it to run.
 * Return the pid of newly created thread on success.
 */
pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags)
{
	return do_fork(flags|CLONE_VM, (unsigned long)fn, (unsigned long)arg,
		       NULL, NULL, 0);
}

SYSCALL_DEFINE0(fork)
{
	return do_fork(CLONE_GLOBAL_THREAD | SIGCHLD, 0, 0, NULL, NULL, 0);
}

SYSCALL_DEFINE0(vfork)
{
	return do_fork(CLONE_GLOBAL_THREAD | CLONE_VFORK | CLONE_VM | SIGCHLD,
		       0, 0, NULL, NULL, 0);
}

SYSCALL_DEFINE5(clone, unsigned long, clone_flags, unsigned long, newsp,
		 int __user *, parent_tidptr,
		 int __user *, child_tidptr,
		 unsigned long, tls)
{
	pid_t pid;

	/*
	 * Libraries may use clone() instead of fork()
	 * to create new process. Add global flag if so:
	 */
	if (!(clone_flags & CLONE_THREAD))
		clone_flags |= CLONE_GLOBAL_THREAD;
	pid = do_fork(clone_flags, newsp, 0, parent_tidptr, child_tidptr, tls);
	return pid;
}

SYSCALL_DEFINE1(set_tid_address, int __user *, tidptr)
{
	current->clear_child_tid = tidptr;
	return current->pid;
}

void __init fork_init(void)
{
	pr_debug("fork: arch_task_struct_size: %d, order: %d task_struct: %lu\n",
		arch_task_struct_size, arch_task_struct_order, sizeof(struct task_struct));
}
