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



struct pt_regs * current_registers;
struct task_struct * current_tsk = NULL;

int flush_flag = 0;


static inline void sleep(unsigned sec)
{
    __set_current_state(TASK_INTERRUPTIBLE);
    schedule_timeout(sec * HZ);
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


static int shadow_copy_begin1(void *unused)
{
    long retval;
    ssize_t retlen;
    ssize_t *retval_ptr;
    u32 len_retbuf, len_msg;
    void *retbuf, *msg;
    struct common_header *hdr;
    struct p2m_shadow_copy_begin_payload *payload;

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
    printk("shadow copy begin\n");

    hdr = msg;
    hdr->opcode = P2M_SHADOW_COPY_BEGIN;
    hdr->src_nid = LEGO_LOCAL_NID;
    payload = msg + sizeof(*hdr);
    payload->pid = current_pid;
    payload->tgid = current_tgid;
    payload->version_id = 1;

    retlen= ibapi_send_reply_imm(1, msg, len_msg, retbuf, len_retbuf, false);


    if(retlen != len_retbuf){
        WARN_ON_ONCE(1);
        retval = -EIO;
        goto out;
    }
out:
    kfree(msg);
    kfree(retbuf);
    return 0;

}

static int shadow_copy_end1(void *unused)
{
    long retval;
    ssize_t retlen;
    ssize_t *retval_ptr;
    u32 len_retbuf, len_msg;
    void *retbuf, *msg;
    struct common_header *hdr;
    struct p2m_shadow_copy_end_payload *payload;

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
    printk("shadow copy end\n");

    hdr = msg;
    hdr->opcode = P2M_SHADOW_COPY_END;
    hdr->src_nid = LEGO_LOCAL_NID;
    payload = msg + sizeof(*hdr);
    payload->pid = current_pid;
    payload->tgid = current_tgid;
    payload->version_id = 1;

    retlen= ibapi_send_reply_imm(1, msg, len_msg, retbuf, len_retbuf, false);


    if(retlen != len_retbuf){
        WARN_ON_ONCE(1);
        retval = -EIO;
        goto out;
    }
out:
    kfree(msg);
    kfree(retbuf);
    return 0;

}


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
                
                    /*if (pdi->nr_dirty_pages == 1){
                        shadow_copy_begin1(NULL);
                    }*/
                    
				    pcache_flush_one(pcm, 0);
				    printk("flush one pcm,addr is %lx\n", rmap->address);
                
            }
        }
    }
    return PCACHE_RMAP_AGAIN;
}






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
            if (flush_flag == 1){
               printk("DepTrack: in this periods, the number of dirty pages are %d\n", pdi.nr_dirty_pages);
               flush_flag +=1;
               struct task_struct *ret1 __maybe_unused;

	            ret1 = kthread_run(toy_func, NULL, "toy_func");
                if (IS_ERR(ret1))
		                panic("Fail to create toy func thread!");
                else{
                    printk("DepTrack: okk\n");
                }
                current_pid = -1;

               
           }
            
           if (flush_flag == 0){
               
               // printk("DepTrack: the ip value is %lu\n", current_registers->ip);
               //  flush_register_value(NULL);
               printk("DepTrack: checkpoint the process\n");
               spin_lock_irqsave(&tasklist_lock, flags);
               set_tsk_thread_flag(current_tsk, TIF_NEED_CHECKPOINT);
               if (!wake_up_state(current_tsk, TASK_ALL))
			        kick_process(current_tsk);
                spin_unlock_irqrestore(&tasklist_lock, flags);
                printk("DepTrack: finished the checkpoint\n");
                kill_pid_info(SIGSTOP, (struct siginfo *) 0, current_pid);
                printk("DepTrack: kill the process\n");

                flush_flag +=1;

               
           }

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

            if (flush_flag == 0)
                shadow_copy_end1(NULL);
            
            
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

            
            /*if (flush_flag == 3){
                 printk("DepTrack: in this periods, the number of dirty pages are %d\n", pdi.nr_dirty_pages);
                 deptrack_restore_worker_thread(NULL);
                 
                
                 kill_pid_info(SIGCONT, (struct siginfo *) 1, current_pid);
                 if (!wake_up_state(current_tsk, TASK_ALL))
			        kick_process(current_tsk);
                  printk("DepTrack: continue the process\n");
                 flush_flag+=1;
                 current_pid = -1;
             }
            if (flush_flag == 2){
               printk("DepTrack: in this periods, the number of dirty pages are %d\n", pdi.nr_dirty_pages);
               flush_flag +=1;
               kill_pid_info(SIGSTOP, (struct siginfo *) 0, current_pid);
               printk("DepTrack: kill the process\n");

               
           }*/
           
           printk("DepTrack: in this periods, the number of dirty pages are %d\n", pdi.nr_dirty_pages);


           
           
           
           
           
           
       
        spin_unlock(&dp_spinlock);
        }
        
        sleep(0.2);

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

