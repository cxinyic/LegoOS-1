#include <processor/dependency_track.h>
#include <processor/pcache.h>

#include <lego/jiffies.h>
#include <lego/sched.h>

static inline void sleep(unsigned sec)
{
    __set_current_state(TASK_INTERRUPTIBLE);
    schedule_timeout(sec * HZ);
}



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
                                    if(*(curr_dp_info->pcm_list+i)==NULL){
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
        sleep(1);
    
    }
    return 0;
}

void dependency_track_init(void)
{
#ifdef CONFIG_DEPENDENCY_TRACK
	struct task_struct *ret __maybe_unused;

	ret = kthread_run(dependency_track, NULL, "dependency_track");
	if (IS_ERR(ret))
		panic("Fail to create dependency_track thread");
	pr_info("dependency tracking up\n");
#endif
}

