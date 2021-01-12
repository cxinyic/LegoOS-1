/*
 * Copyright (c) 2016-2018 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <lego/mm.h>
#include <lego/wait.h>
#include <lego/slab.h>
#include <lego/log2.h>
#include <lego/hash.h>
#include <lego/kernel.h>
#include <lego/pgfault.h>
#include <lego/syscalls.h>
#include <lego/random.h>
#include <lego/jiffies.h>
#include <lego/profile.h>
#include <processor/pcache.h>
#include <processor/processor.h>
#include <processor/dependency_track.h>

/**
 * evict_find_line
 * @pset: the pcache set in question
 *
 * This function will find a line to evict within a set.
 * The returned pcache line MUST be locked.
 */
static inline struct pcache_meta *
evict_find_line(struct pcache_set *pset)
{
#ifdef CONFIG_PCACHE_EVICT_RANDOM
	return evict_find_line_random(pset);
#elif defined(CONFIG_PCACHE_EVICT_FIFO)
	return evict_find_line_fifo(pset);
#elif defined(CONFIG_PCACHE_EVICT_LRU)
	return evict_find_line_lru(pset);
#endif
}

#ifdef CONFIG_PCACHE_EVICTION_VICTIM

DEFINE_PROFILE_POINT(evict_line_victim_prepare)
DEFINE_PROFILE_POINT(evict_line_victim_unmap)
DEFINE_PROFILE_POINT(evict_line_victim_finish)

static inline int evict_line_victim(struct pcache_set *pset,
				    struct pcache_meta *pcm, unsigned long address)
{
	struct pcache_victim_meta *victim;
	bool dirty;
	PROFILE_POINT_TIME(evict_line_victim_prepare)
	PROFILE_POINT_TIME(evict_line_victim_unmap)
	PROFILE_POINT_TIME(evict_line_victim_finish)

	PROFILE_START(evict_line_victim_prepare);
	victim = victim_prepare_insert(pset, pcm, address);
	PROFILE_LEAVE(evict_line_victim_prepare);
	if (IS_ERR(victim))
		return PTR_ERR(victim);

	/*
	 * Make sure other cpus can see the above
	 * updates before we do the unmap operations.
	 */
	smp_wmb();
	PROFILE_START(evict_line_victim_unmap);
	dirty = pcache_try_to_unmap_check_dirty(pcm);
	PROFILE_LEAVE(evict_line_victim_unmap);
	PCACHE_BUG_ON_PCM(pcache_mapped(pcm), pcm);

	PROFILE_START(evict_line_victim_finish);
	victim_finish_insert(victim, dirty);
	PROFILE_LEAVE(evict_line_victim_finish);

	return 0;
}
#endif

#ifdef CONFIG_PCACHE_EVICTION_WRITE_PROTECT
static inline int
evict_line_wrprotect(struct pcache_set *pset, struct pcache_meta *pcm)
{
	/* 1) write-protect from all threads */
	pcache_wrprotect(pcm);

	/* 2) Safely flush back */
	pcache_flush_one(pcm);

	/* 3) unmap all PTEs */
	pcache_try_to_unmap(pcm);

	return 0;
}
#endif

static inline int evict_line(struct pcache_set *pset, struct pcache_meta *pcm,
			     unsigned long address, enum piggyback_options piggyback)
{
#ifdef CONFIG_PCACHE_EVICTION_WRITE_PROTECT
	return evict_line_wrprotect(pset, pcm);
#elif defined(CONFIG_PCACHE_EVICTION_PERSET_LIST)
	return evict_line_perset_list(pset, pcm, piggyback);
#elif defined(CONFIG_PCACHE_EVICTION_VICTIM)
	return evict_line_victim(pset, pcm, address);
#endif
}

/*
 * These two profile points describes algorithm and mechanism.
 * They are the majority of this function.
 */
DEFINE_PROFILE_POINT(pcache_alloc_evict_do_find)
DEFINE_PROFILE_POINT(pcache_alloc_evict_do_evict)

/**
 * pcache_evict_line
 * @pset: the pcache set to find a line to evict
 * @address: the user virtual address who initalized this eviction
 *
 * 1)
 * This function will try to evict one cache line from @pset.
 * If succeed, the cache line will be flushed back to its backing memory.
 * This function can be called concurrently: the selection of cache line
 * is serialized by pset lock, the real eviction procedure can be overlapped.
 *
 * 2)
 * We clear pte and pcache in this sequence:
 * 	a) unmap pte
 * 	b) free pcache
 * This guarantees: if pgfault routines (pcache_do_wp_page) or some rmap walkers
 * use pte_to_pcache_meta() to get the corresponding pcm, and continues holding
 * the pte lock while doing something to this pcm, they are guaranteed this pcm
 * will not go away in the middle.
 *
 * Return 0 on success, otherwise on failures.
 */
int nr_evict_lines = 0;
int nr_flush_lines = 1;
int pcache_evict_line(struct pcache_set *pset, unsigned long address,
		      enum piggyback_options piggyback)
{
	if(nr_evict_lines%10000==0){
		printk("DepTrack: evict %d lines begin\n", nr_evict_lines);
	}
	struct pcache_meta *pcm;
	int nr_mapped;
	int ret;
	int index = 0;
	int i;
	int j;
	int size;
	struct dp_info * curr_dp_info;
	struct dp_info * tmp_dp_info;
	struct dp_idx * tmp_dp_idx;
	struct pcache_meta * pcm_to_flush;
	struct dp_vector * pcms_to_flush;
	struct dp_vector * dependency_queue;
	struct pcache_meta *tmp_pcm;
	PROFILE_POINT_TIME(pcache_alloc_evict_do_find)
	PROFILE_POINT_TIME(pcache_alloc_evict_do_evict)

	inc_pcache_event(PCACHE_EVICTION_TRIGGERED);

	/*
	 * Algorithm Hook.
	 *
	 * We mark this pcache set as Evicting, so sweep thread
	 * will try to avoid use it concurrently. Since lock is
	 * held within evict_find_line(), it is fine to clear it.
	 */
	PROFILE_START(pcache_alloc_evict_do_find);
	__SetPsetEvicting(pset);
	pcm = evict_find_line(pset);
	__ClearPsetEvicting(pset);
	PROFILE_LEAVE(pcache_alloc_evict_do_find);

	if (IS_ERR_OR_NULL(pcm)) {
		if (likely(PTR_ERR(pcm) == -EAGAIN)) {
			inc_pcache_event(PCACHE_EVICTION_EAGAIN_FREEABLE);
			return PCACHE_EVICT_EAGAIN_FREEABLE;
		} else {
			inc_pcache_event(PCACHE_EVICTION_FAILURE_FIND);
			return PCACHE_EVICT_FAILURE_FIND;
		}
	}

	PCACHE_BUG_ON_PCM(!PcacheLocked(pcm), pcm);
	PCACHE_BUG_ON_PCM(!PcacheReclaim(pcm), pcm);
	if (nr_dp_info!=0){
		spin_lock(&dp_spinlock);
		if(nr_evict_lines%1000==0){
			printk("DepTrack: enter flushing\n");
		}
		dependency_queue = (struct dp_vector*)kmalloc(sizeof(struct dp_vector), GFP_KERNEL);
		dp_vector_new(dependency_queue, sizeof(struct pcache_meta* ));
		pcms_to_flush = (struct dp_vector*)kmalloc(sizeof(struct dp_vector), GFP_KERNEL);
		dp_vector_new(pcms_to_flush, sizeof(struct pcache_meta* ));
		for (i=0; i<dp_vector_size(pcm->dependency_list) ; i++){
			dp_vector_pushback(dependency_queue, dp_vector_Nth(pcm->dependency_list,i));
		}
		while (dp_vector_size(pcm->dependency_list)>0){
			dp_vector_delete(pcm->dependency_list, 0);
		}
		while (dp_vector_size(dependency_queue)>0){
			tmp_pcm = dp_vector_Nth(dependency_queue, 0);
			printk("DepTrack: flush  pcm addr is %lx， tmp_pcm addr is %lx\n", pcm, tmp_pcm);
			dp_vector_delete(dependency_queue,0);
			if(nr_evict_lines%1000==0){
				printk("DepTrack: flush step1\n");
			}

			if (!dp_vector_in(pcms_to_flush, tmp_pcm)){
				if(nr_evict_lines%1000==0){
					printk("DepTrack: flush step2\n");
				}
				dp_vector_pushback(pcms_to_flush, tmp_pcm);
				if(nr_evict_lines%1000==0){
					printk("DepTrack: flush step3\n");
				}
				if (tmp_pcm->dependency_list == NULL){
					if(nr_evict_lines%1000==0){
						printk("DepTrack: flush NULL1, pcm addr is %lx\n", tmp_pcm);
					}
					continue;
				}
				if(nr_evict_lines%1000==0){
					printk("DepTrack: flush step3-1\n");
				}
				if(tmp_pcm == NULL){
					if(nr_evict_lines%1000==0){
						printk("DepTrack: flush NULL2\n");
					}
					continue;
				}
				
				printk("DepTrack: flush step3-2\n");
				
				size = dp_vector_size(tmp_pcm->dependency_list);
				
				printk("DepTrack: flush step4, size is %d\n", size);
				
				for (j=0; j<size; j++){
					printk("DepTrack: flush step4-1\n");
					dp_vector_pushback(dependency_queue, dp_vector_Nth(tmp_pcm->dependency_list,j));
					printk("DepTrack: flush step4-2\n");
				}
				while (dp_vector_size(tmp_pcm->dependency_list)>0){
					printk("DepTrack: flush step5-1\n");
					dp_vector_delete(tmp_pcm->dependency_list, 0);
					printk("DepTrack: flush step5-2\n");
				}
			}
			
		}

		while (dp_vector_size(pcms_to_flush)>0){
			if(nr_evict_lines%1000==0){
				printk("DepTrack: flush step6-1\n");
			}
			tmp_pcm = dp_vector_Nth(pcms_to_flush, 0);
			dp_vector_delete(pcms_to_flush,0);
			if (tmp_pcm->prev_dirty == 1 && tmp_pcm!=pcm){
				PROFILE_START(evict_line_perset_flush);
				pcache_flush_one(tmp_pcm);
				PROFILE_LEAVE(evict_line_perset_flush);
			}
			if(nr_evict_lines%1000==0){
				printk("DepTrack: flush step6-2\n");
			}
		}
		


		dp_vector_dispose(dependency_queue);
		kfree(dependency_queue);
		dp_vector_dispose(pcms_to_flush);
		kfree(pcms_to_flush);
		if(nr_evict_lines%1000==0){
			printk("DepTrack: flush step7\n");
		}
		spin_unlock(&dp_spinlock);
	}
	/*
	if (nr_dp_info!=0){
		spin_lock(&dp_spinlock);
		
		for (index = 0; index < dp_vector_size(dp_info_list); index++){
			curr_dp_info = (struct dp_info*)dp_vector_Nth(dp_info_list, index);
            for(i=0;i<curr_dp_info->nr_pages;i++){
				
				if(*(curr_dp_info->pcm_list+i) == pcm){
					// printk("DepTrack: find one\n");
					nr_flush_lines = 0;
					for (j=0; j<dp_vector_size(curr_dp_info->dp_pages+i); j++){
						tmp_dp_idx = dp_vector_Nth(curr_dp_info->dp_pages+i, j);
						tmp_dp_info = (struct dp_info*)dp_vector_Nth(dp_info_list, tmp_dp_idx->list_idx);
						if (tmp_dp_info->dirty_page_list[tmp_dp_idx->addr_idx] == 1 ){
							if (*(tmp_dp_info->pcm_list+tmp_dp_idx->addr_idx) !=NULL){
								pcm_to_flush = *(tmp_dp_info->pcm_list+tmp_dp_idx->addr_idx);
								PROFILE_START(evict_line_perset_flush);
								pcache_flush_one(pcm_to_flush);
								PROFILE_LEAVE(evict_line_perset_flush);
								nr_flush_lines+=1;
								tmp_dp_info->dirty_page_list[tmp_dp_idx->addr_idx] = 0;

							}
						}
					}
					if(nr_flush_lines>0)
					{printk("DepTrack: flush %d lines \n", nr_flush_lines);}
					
				}
			}
		}
		spin_unlock(&dp_spinlock);

	}*/

	/* we locked, it can not be unmapped by others */
	nr_mapped = pcache_mapcount(pcm);
	BUG_ON(nr_mapped < 1);
	PROFILE_START(pcache_alloc_evict_do_evict);
	ret = evict_line(pset, pcm, address, piggyback);
	PROFILE_LEAVE(pcache_alloc_evict_do_evict);

	if (ret) {
		/*
		 * Revert what algorithm has done:
		 * - Clear reclaim flag
		 * - add it back to lru list (update counter)
		 * - unlock
		 * - dec ref (may lead to free)
		 */
		ClearPcacheReclaim(pcm);
		add_to_lru_list(pcm, pset);
		unlock_pcache(pcm);
		put_pcache(pcm);

		inc_pcache_event(PCACHE_EVICTION_FAILURE_EVICT);
		return PCACHE_EVICT_FAILURE_EVICT;
	}

	/*
	 * After a successful eviction, @pcm has no rmap left
	 * which implies PcacheValid is cleared too.
	 */
	PCACHE_BUG_ON_PCM(pcache_mapped(pcm) || PcacheValid(pcm), pcm);

	/*
	 * Each rmap counts one refcount, plus the one grabbed
	 * during evict_find_line(), we should have (nr_mapped + 1)
	 * here if there are no any other users.
	 *
	 * Furthurmore, others can not go from munmap/mremap/wp to
	 * put_pcache() within pcache_zap_pte(), pcache_move_pte()
	 * or pcache_do_wp_page(). Thus the refcount must larger or
	 * equal to (nr_mapped + 1).
	 *
	 * But if there truly other users (refcount > nr_mapped + 1),
	 * then we should manually sub the refcount. The other users
	 * which are currently holding the ref, will free the pcache
	 * once it call put_pcache.
	 */
	PCACHE_BUG_ON_PCM(pcache_ref_count(pcm) < nr_mapped + 1, pcm);
	if (unlikely(!pcache_ref_freeze(pcm, nr_mapped + 1))) {
		if (unlikely(pcache_ref_sub_and_test(pcm, nr_mapped + 1))) {
			pr_info("BUG: pcm refcount, nr_mapped: %d\n", nr_mapped);
			dump_pcache_meta(pcm, "ref error");
			BUG();
		}

		ClearPcacheReclaim(pcm);
		add_to_lru_list(pcm, pset);
		unlock_pcache(pcm);

		inc_pcache_event(PCACHE_EVICTION_EAGAIN_CONCURRENT);
		return PCACHE_EVICT_EAGAIN_CONCURRENT;
	}

	/*
	 * Succeed: pcm is unmapped, and no other threads
	 * are using it. Simply free and return it to pset.
	 */
	__ClearPcacheReclaim(pcm);
	__ClearPcacheLocked(pcm);
	__put_pcache_nolru(pcm);

	inc_pset_event(pset, PSET_EVICTION);
	inc_pcache_event(PCACHE_EVICTION_SUCCEED);
	nr_evict_lines +=1;
	if(nr_evict_lines%10000==1){
		printk("DepTrack: evict %d lines finished\n", nr_evict_lines);
	}
	
	return PCACHE_EVICT_SUCCEED;
}
