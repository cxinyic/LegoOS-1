#include <processor/dependency_track.h>
#include <processor/pcache.h>
#include <processor/node.h>
#include <processor/processor.h>
#include <processor/fs.h>


#include <lego/jiffies.h>
#include <lego/sched.h>
#include <lego/kthread.h>
#include <lego/comp_common.h>
#include <lego/fit_ibapi.h>
#include <lego/checkpoint.h>
#include <asm/prctl.h>

static LIST_HEAD(pss_list);
static DEFINE_SPINLOCK(pss_lock);
static LIST_HEAD(restorer_work_list);
static DEFINE_SPINLOCK(restorer_work_lock);
struct restorer_work_info current_info;
static struct task_struct *restorer_worker;

struct restorer_work_info {
	struct process_snapshot	*pss;
	struct task_struct	*result;
	struct completion	*done;

	struct list_head	list;
};

static inline void sleep(unsigned sec)
{
    __set_current_state(TASK_INTERRUPTIBLE);
    schedule_timeout(sec * HZ);
}

static void deptrack_save_thread_regs(struct task_struct *p, struct ss_task_struct *ss)
{
    struct pt_regs *src = task_pt_regs(p);
	struct ss_thread_gregs *dst = &(ss->user_regs.gregs);

#define COPY_REG(reg)	do { dst->reg = src->reg; } while (0)
	COPY_REG(r15);
	COPY_REG(r14);
	COPY_REG(r13);
	COPY_REG(r12);
	COPY_REG(bp);
	COPY_REG(bx);
	COPY_REG(r11);
	COPY_REG(r10);
	COPY_REG(r9);
	COPY_REG(r8);
	COPY_REG(ax);
	COPY_REG(cx);
	COPY_REG(dx);
	COPY_REG(si);
	COPY_REG(di);
	COPY_REG(orig_ax);
	COPY_REG(ip);
	COPY_REG(cs);
	COPY_REG(flags);
	COPY_REG(sp);
	COPY_REG(ss);
#undef COPY_REG

	dst->fs_base	= p->thread.fsbase;
	dst->gs_base	= p->thread.gsbase;

    // correct? not sure
    savesegment(ds, dst->ds);
    savesegment(es, dst->es);
    savesegment(fs, dst->fs);
    savesegment(gs, dst->gs);

}

int deptrack_save_files(struct task_struct *p, struct process_snapshot *ps)
{
    struct files_struct *files = p->files;
    struct ss_files *ss_files;
    unsigned int fd, nr_files;
    int i = 0;

    nr_files = bitmap_weight(files->fd_bitmap, NR_OPEN_DEFAULT);
    if (nr_files == 0) {
		ps->files = NULL;
		ps->nr_files = 0;
		return 0;
	}
    ss_files = kmalloc(sizeof(*ss_files) * nr_files, GFP_KERNEL);
	if (unlikely(!ss_files))
		return -ENOMEM;
    for_each_set_bit(fd, files->fd_bitmap, NR_OPEN_DEFAULT) {
		struct ss_files *ss_file = &ss_files[i];
		struct file *file = files->fd_array[fd];

		BUG_ON(!file);

		ss_file->fd		= fd;
		ss_file->f_mode		= file->f_mode;
		ss_file->f_flags	= file->f_flags;
		ss_file->f_pos		= file->f_pos;
		memcpy(ss_file->f_name, file->f_name, FILENAME_LEN_DEFAULT);

		i++;
	}
	BUG_ON(i != nr_files);

	ps->files = ss_files;
	ps->nr_files = nr_files;

	return 0;

}

int deptrack_save_signals(struct task_struct *p, struct process_snapshot *ps)
{
    struct k_sigaction *k_action = p->sighand->action;
	struct sigaction *src, *dst;
	int i;

	BUG_ON(p != p->group_leader);

	for (i = 0; i < _NSIG; i++) {
		src = &k_action[i].sa;
		dst = &ps->action[i];
		memcpy(dst, src, sizeof(*dst));
	}

	memcpy(&ps->blocked, &p->blocked, sizeof(sigset_t));

	return 0;
}

void deptrack_enqueue_pss(struct process_snapshot *pss)
{
	BUG_ON(!pss);
	spin_lock(&pss_lock);
	list_add_tail(&pss->list, &pss_list);
	spin_unlock(&pss_lock);
}

struct process_snapshot *deptrack_dequeue_pss(void)
{
	struct process_snapshot *pss = NULL;

	spin_lock(&pss_lock);
	if (!list_empty(&pss_list)) {
		pss = list_entry(pss_list.next, struct process_snapshot, list);
		list_del_init(&pss->list);
	}
	spin_unlock(&pss_lock);

	return pss;
}


struct task_struct *restore_process_snapshot(struct process_snapshot *pss)
{
	DEFINE_COMPLETION(done);
	struct restorer_work_info info;
	struct task_struct *result;

	/*
	 * Note:
	 * If we decide to make this function a-sync later,
	 * we need to allocate info instead of using stack.
	 */
	info.pss = pss;
	info.done = &done;

	spin_lock(&restorer_work_lock);
	list_add_tail(&info.list, &restorer_work_list);
	spin_unlock(&restorer_work_lock);

	wake_up_process(restorer_worker);
	wait_for_completion(&done);

	result = info.result;

	printk("finish restore processs snapshot\n");
	return result;
}

static int __deptrack_do_checkpoint_process(struct task_struct *leader)
{
    struct task_struct *t;
    struct process_snapshot *pss;
    struct ss_task_struct *ss_tasks, *ss_task;
    int ret = 0, i = 0;
    printk("DepTrack: __deptrack_do_checkpoint_process called\n");

    pss = kmalloc(sizeof(*pss), GFP_KERNEL);
    if (!pss)
        return -ENOMEM;
    pss->nr_tasks = leader->signal->nr_threads;
    ss_tasks = kmalloc(sizeof(*ss_tasks) * pss->nr_tasks, GFP_KERNEL);
	if (!ss_tasks) {
		kfree(pss);
		return -ENOMEM;
	}
    pss->tasks = ss_tasks;
    memcpy(pss->comm, leader->comm, TASK_COMM_LEN);

    ret = deptrack_save_files(leader, pss);
    if (ret)
        goto out;
    printk("DepTrack: __deptrack_do_checkpoint_process step1\n");
    
    ret =deptrack_save_signals(leader, pss);
    if (ret)
		goto free_files;
    printk("DepTrack: __deptrack_do_checkpoint_process step2\n");
    

    for_each_thread(leader, t){
        ss_task = &ss_tasks[i++];
		ss_task->pid = t->pid;
		ss_task->set_child_tid = t->set_child_tid;
		ss_task->clear_child_tid = t->clear_child_tid;
		ss_task->sas_ss_sp = t->sas_ss_sp;
		ss_task->sas_ss_size = t->sas_ss_size;
		ss_task->sas_ss_flags = t->sas_ss_flags;

		deptrack_save_thread_regs(t, ss_task);
    }
    printk("DepTrack: __deptrack_do_checkpoint_process step3\n");

    deptrack_enqueue_pss(pss);
    
    current_info.pss = pss;
    
    return 0;
free_files:
    kfree(pss->files);
out:
    kfree(ss_tasks);
    kfree(pss);
    return ret;

}

static int deptrack_do_checkpoint_process(struct task_struct *leader)
{
    int ret;
    preempt_disable();
    ret = __deptrack_do_checkpoint_process(leader);
    preempt_enable_no_resched();

    //restore_process_snapshot(deptrack_dequeue_pss());
    return ret;
}

static void deptrack_wake_up_thread_group(struct task_struct *leader)
{
    struct task_struct *t;
	unsigned long flags;

	spin_lock_irqsave(&tasklist_lock, flags);
	for_each_thread(leader, t) {
		if (leader == t)
			continue;
		if (!wake_up_state(t, TASK_CHECKPOINTING))
			WARN(1, "Fail to wake: %d-%d-state:%ld\n",
				t->pid, t->tgid, t->state);
	}
	spin_unlock_irqrestore(&tasklist_lock, flags);
}

int deptrack_checkpoint_thread(struct task_struct *p){
    printk("DepTrack: deptrack_checkpoint_thread is called here\n");
    struct task_struct *leader;
    long saved_state =p->state;

    leader = p->group_leader;
    if (p != leader){
        set_current_state(TASK_CHECKPOINTING);
        schedule();
        set_current_state(saved_state);
    }else{
        deptrack_do_checkpoint_process(p);

        deptrack_wake_up_thread_group(p);

    }
    
    clear_tsk_thread_flag(p, TIF_NEED_CHECKPOINT);
    return 0;

}

static int deptrack_restore_sys_open(struct ss_files *ss_f)
{
    printk("Restore: open1\n");
	struct file *f;
	int fd, ret;
	char *f_name = ss_f->f_name;
    printk("Restore: open2\n");

	fd = alloc_fd(current->files, f_name);
    printk("Restore: open3\n");
	if (unlikely(fd != ss_f->fd)) {
		pr_err("Unmactched fd: %d:%s\n",
			ss_f->fd, ss_f->f_name);
		return -EBADF;
	}
    printk("Restore: open4\n");

	f = fdget(fd);
    printk("Restore: open5\n");
	f->f_flags = ss_f->f_flags;
	f->f_mode = ss_f->f_mode;
    printk("Restore: open6\n");

	if (unlikely(proc_file(f_name)))
		ret = proc_file_open(f, f_name);
	else if (unlikely(sys_file(f_name)))
		ret = sys_file_open(f, f_name);
    else if (unlikely(dev_file(f_name)))
		ret = dev_file_open(f, f_name);
    else{
        ret = default_file_open(f, f_name);
    }
    printk("Restore: open6\n");


	if (ret) {
		free_fd(current->files, fd);
		goto put;
	}
    printk("Restore: open7\n");
    if (f->f_op == NULL){
        printk("Restore: open NULL\n");
    }

	BUG_ON(!f->f_op->open);
    
	ret = f->f_op->open(f);
	if (ret)
		free_fd(current->files, fd);

put:
	put_file(f);
	return ret;
}

static int deptrack_restore_files(struct process_snapshot *pss)
{
    printk("Restore: step3\n");
    unsigned int nr_files = pss->nr_files;
    printk("Restore: step31\n");
	struct files_struct *files = current->files;
    printk("Restore: step32\n");
	int fd, ret;
	struct file *f;
	struct ss_files *ss_f;
    printk("Restore: step33\n");
    printk("Restore: there are %d open files\n", nr_files);

    for (fd = 0; fd < nr_files; fd++) {
		ss_f = &pss->files[fd];
    printk("Restore: step34\n");
    if (fd < 3 && test_bit(fd, files->fd_bitmap)) {
			f = files->fd_array[fd];
			BUG_ON(!f);

			if (strncmp(ss_f->f_name, f->f_name,
				FILENAME_LEN_DEFAULT)) {
				WARN(1, "Pacth needed here!");
				ret = -EBADF;
				goto out;
			}
			continue;
		}
printk("Restore: step35\n");
		ret = deptrack_restore_sys_open(ss_f);
        printk("Restore: step36\n");
		if (ret)
			goto out;
	}

out:
	return ret;
}

static void deptrack_restore_signals(struct process_snapshot *pss)
{
     printk("Restore: step4\n");
	struct k_sigaction *k_action = current->sighand->action;
	struct sigaction *src, *dst;
	int i;

	for (i = 0; i < _NSIG; i++) {
		src = &pss->action[i];
		dst = &k_action[i].sa;
		memcpy(dst, src, sizeof(*dst));
	}

	memcpy(&current->blocked, &pss->blocked, sizeof(sigset_t));
}

static void deptrack_restore_thread_state(struct task_struct *p,
				 struct ss_task_struct *ss_task)
{
	struct pt_regs *dst = task_pt_regs(p);
	struct ss_thread_gregs *src = &(ss_task->user_regs.gregs);

	p->set_child_tid = ss_task->set_child_tid;
	p->clear_child_tid = ss_task->clear_child_tid;
	p->sas_ss_sp = ss_task->sas_ss_sp;
	p->sas_ss_size = ss_task->sas_ss_size;
	p->sas_ss_flags = ss_task->sas_ss_flags;

#define RESTORE_REG(reg)	do { dst->reg = src->reg; } while (0)
	RESTORE_REG(r15);
	RESTORE_REG(r14);
	RESTORE_REG(r13);
	RESTORE_REG(r12);
	RESTORE_REG(bp);
	RESTORE_REG(bx);
	RESTORE_REG(r11);
	RESTORE_REG(r10);
	RESTORE_REG(r9);
	RESTORE_REG(r8);
	RESTORE_REG(ax);
	RESTORE_REG(cx);
	RESTORE_REG(dx);
	RESTORE_REG(si);
	RESTORE_REG(di);
	RESTORE_REG(orig_ax);
	RESTORE_REG(ip);
	RESTORE_REG(cs);
	RESTORE_REG(flags);
	RESTORE_REG(sp);
	RESTORE_REG(ss);
#undef RESTORE_REG

	if (src->fs_base)
		do_arch_prctl(p, ARCH_SET_FS, src->fs_base);
	if (src->gs_base)
		do_arch_prctl(p, ARCH_SET_GS, src->gs_base);
}


static void deptrack_restore_thread_group(struct restorer_work_info *info)
{
     printk("Restore: step5\n");
    struct process_snapshot *pss = info->pss;
	struct ss_task_struct *ss_task, *ss_tasks = pss->tasks;
	struct task_struct *t;
	struct wait_info *wait;
	unsigned long clone_flags;
	int nr_threads = pss->nr_tasks;
	int i;

	ss_task = &ss_tasks[0];
	deptrack_restore_thread_state(current, ss_task);

	if (nr_threads == 1)
    {    printk("Restore: only one thread\n");
		goto done;}
    // TODO: other threads
done:
    info->result = current;
}

static int deptrack_restorer_for_group_leader(void *_info)
{
    struct restorer_work_info *info = _info;
    printk("Restore: step2\n");
    complete(info->done);
    return 0;
    /*struct restorer_work_info *info = _info;
	struct process_snapshot *pss = info->pss;
    // memcpy(current->comm, pss->comm, TASK_COMM_LEN);
    // deptrack_restore_files(pss);
    // deptrack_restore_signals(pss);
    current->pm_data.home_node = 1;

    // deptrack_restore_thread_group(info);
	if (IS_ERR(info->result))
		goto err;
     printk("Restore: finished\n");
    return 0;

err:
    do_exit(-1);
    BUG();
    return 0;*/
}

static void deptrack_create_restorer(struct restorer_work_info *info)
{
    int pid;
    printk("Restore: step 1\n");
    pid = do_fork(SIGCHLD, (unsigned long)deptrack_restorer_for_group_leader,
			(unsigned long)info, NULL, NULL, 0);
	if (pid < 0) {
		WARN_ON_ONCE(1);
		info->result = ERR_PTR(pid);
        complete(info->done);
	}
    printk("Restore: pid is %d\n", pid);
}

int deptrack_restore_worker_thread(void* unused)
{
    // set_cpus_allowed_ptr(current, cpu_possible_mask);
    if (pin_current_thread()){
        printk("TaskTrack: fail to pin task_track thread\n");
    }
    printk("Restore: begin\n");
    while (1)
    {
        /*set_current_state(TASK_UNINTERRUPTIBLE);
		if (list_empty(&restorer_work_list))
			schedule();
		__set_current_state(TASK_RUNNING);*/
        sleep(1);
        printk("Restore: wake up\n");
        spin_lock(&restorer_work_lock);
		while (!list_empty(&restorer_work_list)) {
			struct restorer_work_info *info;

			info = list_entry(restorer_work_list.next,
					struct restorer_work_info, list);
			list_del_init(&info->list);

			
			spin_unlock(&restorer_work_lock);

			deptrack_create_restorer(info);

			spin_lock(&restorer_work_lock);
		}
		spin_unlock(&restorer_work_lock);
    }
    return 0;
    /*if(!list_empty(&restorer_work_list)){
        struct restorer_work_info *info;
            printk("Restore: begin1\n");
			info = list_entry(restorer_work_list.next,
					struct restorer_work_info, list);
            printk("Restore: begin2\n");
			// list_del_init(&info->list);
            // printk("Restore: begin3\n");
            if (info == NULL){
                printk("Restore: NULL\n");
            }
            deptrack_create_restorer(info);
            printk("Restore: begin3\n");
    }
    struct restorer_work_info *info;
    info = kmalloc(sizeof(struct restorer_work_info), GFP_KERNEL);
    *info = current_info;
    if (info == NULL){
        printk("Restore: NULL\n");
    }
    deptrack_create_restorer(info);
    printk("Restore: begin1\n");*/


}

void  deptrack_checkpoint_init(void)
{
    restorer_worker = kthread_run(deptrack_restore_worker_thread, NULL, "task_restore");
	if (IS_ERR(restorer_worker))
		panic("Fail to create task restore thread!");

}