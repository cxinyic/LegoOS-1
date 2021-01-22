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

struct restorer_work_info {
	struct process_snapshot	*pss;
	struct task_struct	*result;
	struct completion	*done;

	struct list_head	list;
};

struct pt_regs * current_registers;
struct task_struct * current_tsk = NULL;


static inline void sleep(unsigned sec)
{
    __set_current_state(TASK_INTERRUPTIBLE);
    schedule_timeout(sec * HZ);
}


/*
static int dependency_track(void *unused){
    pgd_t *pgd;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    int index = 0;
    int i;
    int j;
    struct dp_info * curr_dp_info;
    struct dp_idx curr_dp_idx;
    struct dp_idx* tmp_dp_idx1;
    struct dp_idx* tmp_dp_idx2;
    struct dp_vector* tmp_dp_vector;

    while(1){
        spin_lock(&dp_spinlock);
        if(nr_dp_info != 0){
            for(index = 0; index < dp_vector_size(dp_info_list); index++){
                curr_dp_info = (struct dp_info*)dp_vector_Nth(dp_info_list, index);
                for(i=0;i<curr_dp_info->nr_pages;i++){
                    pgd = pgd_offset(curr_dp_info->user_mm, curr_dp_info->addr+i*PAGE_SIZE);
                    if(!pgd_none(*pgd)){
                        pud = pud_offset(pgd, curr_dp_info->addr+i*PAGE_SIZE);
                        if(!pud_none(*pud)){
                            pmd = pmd_offset(pud,curr_dp_info->addr+i*PAGE_SIZE);
                            if(!pmd_none(*pmd)){
                                pte = pte_offset(pmd,curr_dp_info->addr+i*PAGE_SIZE);
                                if(!pte_none(*pte) && pte_present(*pte) && pte_dirty(*pte)){
                                    curr_dp_info->dirty_page_list[i] = 1;
                                    *pte = pte_mkclean(*pte);
                                    curr_dp_idx.list_idx = index;
                                    curr_dp_idx.addr_idx = i;
                                    dp_vector_pushback(new_dirty_pages,&curr_dp_idx);
                                    if(*(curr_dp_info->pcm_list+i) == NULL){
                                        *(curr_dp_info->pcm_list+i) = pte_to_pcache_meta(*pte);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        
            if(nr_dp_info == 0){
                continue;
            }

            for(i=0; i<dp_vector_size(new_dirty_pages); i++){
                for(j=0; j<dp_vector_size(new_dirty_pages); j++){
                    if(i!=j){
                        tmp_dp_idx1 = dp_vector_Nth(new_dirty_pages,i);
                        tmp_dp_idx2 = dp_vector_Nth(new_dirty_pages,j);
                        curr_dp_info = (struct dp_info*) dp_vector_Nth(dp_info_list, tmp_dp_idx1->list_idx);
                        dp_vector_pushback(curr_dp_info->dp_pages+tmp_dp_idx1->addr_idx, tmp_dp_idx2);
                    }
                }
            }

            for(i=0; i<dp_vector_size(new_dirty_pages); i++){
                for(j=0; j<dp_vector_size(old_dirty_pages); j++){
                    tmp_dp_idx1 = dp_vector_Nth(new_dirty_pages,i);
                    tmp_dp_idx2 = dp_vector_Nth(new_dirty_pages,j);
                    if(!((tmp_dp_idx1->addr_idx == tmp_dp_idx2->addr_idx)&&(tmp_dp_idx1->list_idx == tmp_dp_idx2->list_idx))){
                        curr_dp_info = (struct dp_info*) dp_vector_Nth(dp_info_list, tmp_dp_idx1->list_idx);
                        dp_vector_pushback(curr_dp_info->dp_pages+tmp_dp_idx1->addr_idx, tmp_dp_idx2);
                    }
                }
            }
        
            tmp_dp_vector = old_dirty_pages;
            old_dirty_pages = new_dirty_pages;
            new_dirty_pages = tmp_dp_vector;
            while(dp_vector_size(new_dirty_pages)>0){
                dp_vector_delete(new_dirty_pages,0);
            }
            printk("DepTrack: in this perios, the number of dirty pages are %d\n", dp_vector_size(old_dirty_pages));
        }
        spin_unlock(&dp_spinlock);
        sleep(0.5);
    
    }
    return 0;
}*/

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
	struct file *f;
	int fd, ret;
	char *f_name = ss_f->f_name;

	fd = alloc_fd(current->files, f_name);
	if (unlikely(fd != ss_f->fd)) {
		pr_err("Unmactched fd: %d:%s\n",
			ss_f->fd, ss_f->f_name);
		return -EBADF;
	}

	f = fdget(fd);
	f->f_flags = ss_f->f_flags;
	f->f_mode = ss_f->f_mode;

	if (unlikely(proc_file(f_name)))
		ret = proc_file_open(f, f_name);
	else if (unlikely(sys_file(f_name)))
		ret = sys_file_open(f, f_name);


	if (ret) {
		free_fd(current->files, fd);
		goto put;
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
	struct files_struct *files = current->files;
	int fd, ret;
	struct file *f;
	struct ss_files *ss_f;

    for (fd = 0; fd < nr_files; fd++) {
		ss_f = &pss->files[fd];
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

		ret = deptrack_restore_sys_open(ss_f);
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
		goto done;
    // TODO: other threads
done:
    info->result = current;
}

static int deptrack_restorer_for_group_leader(void *_info)
{
    printk("Restore: step2\n");
    struct restorer_work_info *info = _info;
	struct process_snapshot *pss = info->pss;
    memcpy(current->comm, pss->comm, TASK_COMM_LEN);
    deptrack_restore_files(pss);
    deptrack_restore_signals(pss);

    deptrack_restore_thread_group(info);
	if (IS_ERR(info->result))
		goto err;
     printk("Restore: finished\n");
    return 0;

err:
    do_exit(-1);
    BUG();
    return 0;
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
	}
}

int deptrack_restore_worker_thread(void* unused)
{
    printk("Restore: begin\n");
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
    }*/
    struct restorer_work_info *info;
    info = kmalloc(sizeof(struct restorer_work_info), GFP_KERNEL);
    *info = current_info;
    if (info == NULL){
        printk("Restore: NULL\n");
    }
    deptrack_create_restorer(info);
    printk("Restore: begin1\n");


}


/*int deptrack_checkpoint_thread(struct task_struct *p){
    printk("DepTrack: deptrack_checkpoint_thread is called here\n");
    if (p!=current_tsk){
        return 0;
    }
    preempt_disable();

    struct pt_regs *src = task_pt_regs(p);

    current_registers->r15 = src->r15;
    current_registers->r14 = src->r14;
    current_registers->r13 = src->r13;
    current_registers->r12 = src->r12;
    current_registers->bp = src->bp;
    current_registers->bx = src->bx;
    current_registers->r11 = src->r11;
    current_registers->r10 = src->r10;
    current_registers->r9 = src->r9;
    current_registers->r8 = src->r8;
    current_registers->ax = src->ax;
    current_registers->cx = src->cx;
    current_registers->dx = src->dx;
    current_registers->si = src->si;
    current_registers->di = src->di;
    current_registers->orig_ax = src->orig_ax;
    current_registers->ip = src->ip;
    current_registers->cs = src->cs;
    current_registers->flags = src->flags;
    current_registers->sp = src->sp;
    current_registers->ss = src->ss;
    fs_base_value = p->thread.fsbase;
    gs_base_value = p->thread.gsbase;

    preempt_enable_no_resched();



    clear_tsk_thread_flag(p, TIF_NEED_CHECKPOINT);
    printk("DepTrack: finished this call\n");
    return 0;

}*/

static int get_register_value(void *unused){
    unsigned long retval;
    ssize_t retlen;
    ssize_t *retval_ptr;
    u32 len_retbuf, len_msg;
    void *retbuf, *msg;
    struct common_header *hdr;
    struct p2m_read_register_payload *payload;

    len_retbuf = sizeof(long);
    retbuf = kmalloc(len_retbuf, GFP_KERNEL);
    if (!retbuf)
        return -ENOMEM;
    len_msg = sizeof(*hdr) + sizeof(*payload);
    msg = kmalloc(len_msg, GFP_KERNEL);
    if (!msg) {
        kfree(retbuf);
        return -ENOMEM;
    }
    printk("Get:step1\n");
    hdr = msg;
    hdr->opcode = P2M_READ_REGISTER;
    hdr->src_nid = LEGO_LOCAL_NID;

    payload = msg + sizeof(*hdr);
    payload->pid = current_pid;
    payload->tgid = current_tgid;
    retlen= ibapi_send_reply_imm(1, msg, len_msg, retbuf, len_retbuf, false);


    if(retlen != len_retbuf){
        WARN_ON_ONCE(1);
        retval = -EIO;
        goto out;
    }
    printk("Get: step2\n");
    retval_ptr = retbuf;
    retval = *retval_ptr;
    printk("Get: step3, the value of ip is %lx\n",retval);

out:
    kfree(msg);
    kfree(retbuf);
    return 0;


}

static int flush_register_value(void *unused){
    long retval;
    ssize_t retlen;
    ssize_t *retval_ptr;
    u32 len_retbuf, len_msg;
    void *retbuf, *msg;
    struct common_header *hdr;
    struct p2m_flush_register_payload *payload;

    len_retbuf = sizeof(long);
    retbuf = kmalloc(len_retbuf, GFP_KERNEL);
    if (!retbuf)
        return -ENOMEM;
    len_msg = sizeof(*hdr) + sizeof(*payload);
    msg = kmalloc(len_msg, GFP_KERNEL);
    if (!msg) {
        kfree(retbuf);
        return -ENOMEM;
    }
    printk("Flush: step1\n");

    hdr = msg;
    hdr->opcode = P2M_FLUSH_REGISTER;
    hdr->src_nid = LEGO_LOCAL_NID;

    payload = msg + sizeof(*hdr);
    payload->pid = current_pid;
    payload->tgid = current_tgid;
    payload->r15 = current_registers->r15;
    payload->r14 = current_registers->r14;
    payload->r13 = current_registers->r13;
    payload->r12 = current_registers->r12;
    payload->bp = current_registers->bp;
    payload->bx = current_registers->bx;
    payload->r11 = current_registers->r11;
    payload->r10 = current_registers->r10;
    payload->r9 = current_registers->r9;
    payload->r8 = current_registers->r8;
    payload->ax = current_registers->ax;
    payload->cx = current_registers->cx;
    payload->dx = current_registers->dx;
    payload->si = current_registers->si;
    payload->di = current_registers->di;
    payload->orig_ax = current_registers->orig_ax;
    payload->ip = current_registers->ip;
    payload->cs = current_registers->cs;
    payload->flags = current_registers->flags;
    payload->sp = current_registers->sp;
    payload->ss = current_registers->ss;
    payload->fs_base = fs_base_value;
    payload->gs_base = gs_base_value;


    payload->ds = ds_value;
    payload->es = es_value;
    payload->fs = fs_value;
    payload->gs = gs_value;
    payload->version_id = 0;


    retlen= ibapi_send_reply_imm(1, msg, len_msg, retbuf, len_retbuf, false);

    // TODO: change based on the memory side return 
    if(retlen != len_retbuf){
        WARN_ON_ONCE(1);
        retval = -EIO;
        goto out;
    }
    printk("Flush: step2\n");

out:
    kfree(msg);
    kfree(retbuf);
    return 0;

}

struct pcache_dependency_info{
    struct pcache_meta * first_pcm;
    struct pcache_meta * last_pcm;
    int nr_dirty_pages;

}; 

static int __add_dependency_if_dirty(struct pcache_meta *pcm, struct pcache_rmap *rmap, void *arg)
{
    struct pcache_dependency_info * pdi = arg;
    pte_t *pte;
    struct pcache_meta * tmp_pcm;
    if (rmap->owner_process->pid == current_pid){
        pte = rmap->page_table;
        if (!pte_none(*pte) && pte_present(*pte)) {
            if (likely(pte_dirty(*pte))) {
                *pte = pte_mkclean(*pte);
                pcm->prev_dirty = 1;
                pdi->nr_dirty_pages +=1;
                if (pdi->first_pcm == NULL){
                    pdi->first_pcm = pcm;
                }
                if (pdi->last_pcm != NULL){
                    if (!dp_vector_in(pdi->last_pcm->dependency_list, &pcm))
                    {
                        dp_vector_pushback(pdi->last_pcm->dependency_list, &pcm);
                    }
                }
                pdi->last_pcm = pcm;
            }
        }
    }
    return PCACHE_RMAP_AGAIN;
}


int flush_flag = 0;

static int dependency_track(void *unused){
    struct pcache_meta *pcm;
    int nr = 0;
    int count = 0;
    struct pcache_dependency_info pdi;
    struct pt_regs *tmp;
    unsigned long flags;
    if (pin_current_thread()){
        printk("DepTrack: fail to pin dependency_track thread\n");
    }

    current_registers = (struct pt_regs*)kmalloc(sizeof(struct pt_regs), GFP_KERNEL);
    

    
    while (1){
        if(current_pid>0){
            spin_lock(&dp_spinlock);
            pdi.first_pcm = NULL;
            pdi.last_pcm = NULL;
            pdi.nr_dirty_pages = 0;

            struct rmap_walk_control rwc = {
                .arg = &pdi,
                .rmap_one = __add_dependency_if_dirty,
            }; 
            pcache_for_each_way(pcm, nr) {
                rmap_walk(pcm, &rwc);
            }
            
            if (pdi.first_pcm != NULL && pdi.last_pcm != NULL ){
                
                if (pdi.first_pcm != pdi.last_pcm && !dp_vector_in(pdi.last_pcm->dependency_list, &pdi.first_pcm)){
                    dp_vector_pushback(pdi.last_pcm->dependency_list, &pdi.first_pcm);
                }
                if (dirty_pcm_last_period != NULL){
                    if(pdi.first_pcm != dirty_pcm_last_period){
                        if (!dp_vector_in(pdi.first_pcm->dependency_list, &dirty_pcm_last_period )){
                            dp_vector_pushback(pdi.first_pcm->dependency_list, &dirty_pcm_last_period );
                        }
                    }
                    else if (pdi.last_pcm != dirty_pcm_last_period){
                        if (!dp_vector_in(pdi.last_pcm->dependency_list, &dirty_pcm_last_period )){
                            dp_vector_pushback(pdi.last_pcm->dependency_list, &dirty_pcm_last_period );
                        }
                    }
                }
                dirty_pcm_last_period = pdi.last_pcm;
            }

            /*tmp = task_pt_regs(current_tsk);
            current_registers->r15 = tmp->r15;
            current_registers->r14 = tmp->r14;
            current_registers->r13 = tmp->r13;
            current_registers->r12 = tmp->r12;
            current_registers->bp = tmp->bp;
            current_registers->bx = tmp->bx;
            current_registers->r11 = tmp->r11;
            current_registers->r10 = tmp->r10;
            current_registers->r9 = tmp->r9;
            current_registers->r8 = tmp->r8;
            current_registers->ax = tmp->ax;
            current_registers->cx = tmp->cx;
            current_registers->dx = tmp->dx;
            current_registers->si = tmp->si;
            current_registers->di = tmp->di;
            current_registers->orig_ax = tmp->orig_ax;
            current_registers->ip = tmp->ip;
            current_registers->cs = tmp->cs;
            current_registers->flags = tmp->flags;
            current_registers->sp = tmp->sp;
            current_registers->ss = tmp->ss;*/

            //gs fs es ds?

            if (flush_flag == 4){
                printk("DepTrack: in this periods, the number of dirty pages are %d\n", pdi.nr_dirty_pages);
                flush_flag+=1;
                current_pid = -1;
            }
            if (flush_flag == 3){
                 printk("DepTrack: in this periods, the number of dirty pages are %d\n", pdi.nr_dirty_pages);
                 deptrack_restore_worker_thread(NULL);
                 flush_flag+=1;
             }
            if (pdi.nr_dirty_pages>0 && pdi.nr_dirty_pages< 100 && flush_flag == 2){
               printk("DepTrack: in this periods, the number of dirty pages are %d\n", pdi.nr_dirty_pages);
               flush_flag +=1;
               kill_pid_info(SIGTERM, (struct siginfo *) 2, current_pid);
               printk("DepTrack: kill the process\n");

               
           }
           if (pdi.nr_dirty_pages>0 && pdi.nr_dirty_pages< 100 && flush_flag == 1){
               printk("DepTrack: in this periods, the number of dirty pages are %d\n", pdi.nr_dirty_pages);
               flush_flag +=1;
               
           }
            
           if (pdi.nr_dirty_pages>0 && pdi.nr_dirty_pages< 100 && flush_flag == 0){
               printk("DepTrack: in this periods, the number of dirty pages are %d\n", pdi.nr_dirty_pages);
               // printk("DepTrack: the ip value is %lu\n", current_registers->ip);
               //  flush_register_value(NULL);
               printk("DepTrack: checkpoint the process\n");
               spin_lock_irqsave(&tasklist_lock, flags);
               set_tsk_thread_flag(current_tsk, TIF_NEED_CHECKPOINT);
               if (!wake_up_state(current_tsk, TASK_ALL))
			        kick_process(current_tsk);
                spin_unlock_irqrestore(&tasklist_lock, flags);
                printk("DepTrack: finished the checkpoint\n");
                flush_flag +=1;
               
               // kill_pid_info(SIGCONT, (struct siginfo *) 2, current_pid);

               
               // printk("DepTrack: called flush_register_value successfully\n");
               
           }


           
           
           
           
           
           
       
        spin_unlock(&dp_spinlock);
        }
        
        sleep(0.8);

    }
    
    return 0;
}

#ifdef CONFIG_DEPENDENCY_TRACK
void dependency_track_init(void)
{
	struct task_struct *ret __maybe_unused;

	ret = kthread_run(dependency_track, NULL, "dependency_track");
	if (IS_ERR(ret))
		panic("Fail to create dependency_track thread");
	pr_info("dependency tracking up\n");
}
#endif

