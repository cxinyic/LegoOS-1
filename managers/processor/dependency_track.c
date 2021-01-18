#include <processor/dependency_track.h>
#include <processor/pcache.h>
#include <processor/node.h>


#include <lego/jiffies.h>
#include <lego/sched.h>
#include <lego/kthread.h>
#include <lego/comp_common.h>
#include <lego/fit_ibapi.h>

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

            tmp = task_pt_regs(current_tsk);
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
            current_registers->ss = tmp->ss;

            //gs fs es ds?

            
            
           if (pdi.nr_dirty_pages>0 && pdi.nr_dirty_pages< 100 && flush_flag == 0){
               printk("DepTrack: in this periods, the number of dirty pages are %d\n", pdi.nr_dirty_pages);
               flush_register_value(NULL);
               printk("DepTrack: called flush_register_value successfully\n");
               flush_flag = 1;
           }
       
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

