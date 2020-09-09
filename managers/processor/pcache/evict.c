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
//	if (address == 0x7fff3cc9b000 || address == 0x7fff3f3fd000)
//		pr_info("QZ: %s() use wrprotect to evict.\n", __func__);
	return evict_line_wrprotect(pset, pcm);
#elif defined(CONFIG_PCACHE_EVICTION_PERSET_LIST)
	return evict_line_perset_list(pset, pcm, piggyback);
	// QZ: debug
//	if (address == 0x7fff3cc9b000 || address == 0x7fff3f3fd000)
//		pr_info("QZ: %s() use perset list to evict.\n", __func__);
//	return evict_line_perset_list(pset, pcm, address, piggyback); // QZ: add address to debug
#elif defined(CONFIG_PCACHE_EVICTION_VICTIM)
//	if (address == 0x7fff3cc9b000 || address == 0x7fff3f3fd000)
//		pr_info("QZ: %s() use victim to evict.\n", __func__);
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
int pcache_evict_line(struct pcache_set *pset, unsigned long address,
		      enum piggyback_options piggyback)
{
	struct pcache_meta *pcm;
	int nr_mapped;
	int ret;
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
//	if (address == 0x7fff3cc9b000 || address == 0x7fff3f3fd000)
//		pr_info("QZ: %s() before set evicting on pset for addr = %#lx\n", __func__, address);
	__SetPsetEvicting(pset);
//	if (address == 0x7fff3cc9b000 || address == 0x7fff3f3fd000)
//		pr_info("QZ: %s() before find the line to evict.\n", __func__);
	pcm = evict_find_line(pset);
//	if (address == 0x7fff3cc9b000 || address == 0x7fff3f3fd000)
//		pr_info("QZ: %s() find the line to evict.\n", __func__);
	__ClearPsetEvicting(pset);
//	if (address == 0x7fff3cc9b000 || address == 0x7fff3f3fd000)
//		pr_info("QZ: %s() after clear evicting on paset.\n", __func__);
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

//	if (address == 0x7fff3cc9b000 || address == 0x7fff3f3fd000)
//		pr_info("QZ: %s() lock pcm.\n", __func__);
	PCACHE_BUG_ON_PCM(!PcacheLocked(pcm), pcm);
//	if (address == 0x7fff3cc9b000 || address == 0x7fff3f3fd000)
//		pr_info("QZ: %s() reclaim the pcm.\n", __func__);
	PCACHE_BUG_ON_PCM(!PcacheReclaim(pcm), pcm);

	/* we locked, it can not be unmapped by others */
	nr_mapped = pcache_mapcount(pcm);
	BUG_ON(nr_mapped < 1);

	PROFILE_START(pcache_alloc_evict_do_evict);
//	if (address == 0x7fff3cc9b000 || address == 0x7fff3f3fd000)
//		pr_info("QZ: %s() before evict_line.\n", __func__);
	ret = evict_line(pset, pcm, address, piggyback);
//	if (address == 0x7fff3cc9b000 || address == 0x7fff3f3fd000)
//		pr_info("QZ: %s() after evict_line.\n", __func__);
	PROFILE_LEAVE(pcache_alloc_evict_do_evict);
	if (ret) {
		/*
		 * Revert what algorithm has done:
		 * - Clear reclaim flag
		 * - add it back to lru list (update counter)
		 * - unlock
		 * - dec ref (may lead to free)
		 */
//		if (address == 0x7fff3cc9b000 || address == 0x7fff3f3fd000)
//			pr_info("QZ: %s() evict failed. Before clear reclaim flag.\n", __func__);
		ClearPcacheReclaim(pcm);
//		if (address == 0x7fff3cc9b000 || address == 0x7fff3f3fd000)
//			pr_info("QZ: %s() evict failed. Before add to lru list.\n", __func__);
		add_to_lru_list(pcm, pset);
//		if (address == 0x7fff3cc9b000 || address == 0x7fff3f3fd000)
//			pr_info("QZ: %s() evict failed. Before unlock pcm.\n", __func__);
		unlock_pcache(pcm);
//		if (address == 0x7fff3cc9b000 || address == 0x7fff3f3fd000)
//			pr_info("QZ: %s() evict failed. Before put_pcache on pcm.\n", __func__);
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

//		if (address == 0x7fff3cc9b000 || address == 0x7fff3f3fd000)
//			pr_info("QZ: %s() ref coutn is not right.\n", __func__);
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
//	if (address == 0x7fff3cc9b000 || address == 0x7fff3f3fd000)
//		pr_info("QZ: %s() pcm is successfully unmapped. Before clear reclaim.\n", __func__);
	__ClearPcacheReclaim(pcm);
//	if (address == 0x7fff3cc9b000 || address == 0x7fff3f3fd000)
//		pr_info("QZ: %s() pcm is successfully unmapped. Before unlock pcm.\n", __func__);
	__ClearPcacheLocked(pcm);
//	if (address == 0x7fff3cc9b000 || address == 0x7fff3f3fd000)
//		pr_info("QZ: %s() pcm is successfully unmapped. Before put pcm to nolru.\n", __func__);
	__put_pcache_nolru(pcm);

	inc_pset_event(pset, PSET_EVICTION);
	inc_pcache_event(PCACHE_EVICTION_SUCCEED);
//	if (address == 0x7fff3cc9b000 || address == 0x7fff3f3fd000)
//		pr_info("QZ: %s() exiting....\n", __func__);
	return PCACHE_EVICT_SUCCEED;
}
