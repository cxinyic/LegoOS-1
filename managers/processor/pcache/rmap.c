/*
 * Copyright (c) 2016-2017 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <lego/mm.h>
#include <lego/slab.h>
#include <lego/log2.h>
#include <lego/kernel.h>
#include <lego/pgfault.h>
#include <lego/syscalls.h>
#include <lego/ratelimit.h>
#include <lego/comp_processor.h>

#include <asm/io.h>
#include <asm/tlbflush.h>

#include <processor/include/pcache.h>

int pcache_add_rmap(struct pcache_meta *pcm, pte_t *page_table,
		    unsigned long address)
{
	struct pcache_rmap *rmap, *pos;
	struct pcache_set *pset;

	rmap = kmalloc(sizeof(*rmap), GFP_KERNEL);
	if (!rmap)
		return -ENOMEM;
	rmap->page_table = page_table;
	rmap->address = address;
	rmap->owner = current;

	/*
	 * Use the lock of pcache set to protect
	 * all ways rmap operations:
	 */
	pset = pcache_meta_to_pcache_set(pcm);
	spin_lock(&pset->lock);
	if (likely(list_empty(&pcm->rmap)))
		goto add;

	list_for_each_entry(pos, &pcm->rmap, next)
		BUG_ON(pos->page_table == page_table);

add:
	list_add(&rmap->next, &pcm->rmap);
	atomic_inc(&pcm->mapcount);
	spin_unlock(&pset->lock);
	return 0;
}

/*
 * We are traversing the reverse mapping, which means
 * upper level mappings must exist and present. If not, BUG.
 */
static inline pmd_t *
rmap_get_pmd(struct mm_struct *mm, unsigned long address)
{
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;

	pgd = pgd_offset(mm, address);
	BUG_ON(!pgd && !pgd_present(*pgd));

	pud = pud_offset(pgd, address);
	BUG_ON(!pud && !pud_present(*pud));

	pmd = pmd_offset(pud, address);
	BUG_ON(!pmd && !pmd_present(*pmd));

	return pmd;
}

/*
 * To make it simple, it just return rmap->page_table.
 * But for safety reasons, we add several checkings here.
 *
 * Upon return, pte is locked. Because normally callers will
 * modify or even invalidate the PTEs. Those operations have
 * to be serialized with PTE establishment.
 */
static __always_inline pte_t *
rmap_get_checked_pte(struct pcache_meta *pcm, struct pcache_rmap *rmap,
		     spinlock_t **ptlp)	__acquires(*ptlp)
{
	pte_t *ptep;
	pmd_t *pmd;
	spinlock_t *ptl;
	struct mm_struct *mm = rmap->owner->mm;
	unsigned long address = rmap->address;

	pmd = rmap_get_pmd(mm, address);
	ptep = pte_offset(pmd, address);

#ifdef CONFIG_DEBUG_PCACHE_PARANOID
	if (unlikely(ptep != rmap->page_table)) {
		BUG();
	}

	if (unlikely(pcache_meta_to_pfn(pcm) != pte_pfn(*ptep))) {
		BUG();
	}
#endif

	ptl = pte_lockptr(mm, pmd);
	spin_lock(ptl);
	*ptlp = ptl;

	return ptep;
}

static inline void
pcache_paronoid_unmap_check(pte_t pte, struct pcache_meta *pcm,
			    struct pcache_rmap *rmap)
{
	unsigned long pcm_pfn, pgtable_pfn;

	pcm_pfn = pcache_meta_to_pfn(pcm);
	pgtable_pfn = pte_pfn(pte);
	if (unlikely(pcm_pfn != pgtable_pfn)) {
		pr_err("owner: %u pcm_pfn: %#lx, pte_pfn: %#lx\n",
			rmap->owner->pid, pcm_pfn, pgtable_pfn);
		BUG();
	}
}

static void dump_pcache_rmap(struct pcache_rmap *rmap)
{
	unsigned long va = rmap->address;
	struct task_struct *owner = rmap->owner;
	pte_t *ptep = rmap->page_table;

	pr_info("%s() owner:%u va:%#lx\n", FUNC, owner->pid, va);
	dump_pte(ptep, NULL);
}

static int pcache_try_to_unmap_one(struct pcache_meta *pcm,
				   struct pcache_rmap *rmap, void *arg)
{
	int ret = PCACHE_RMAP_AGAIN;
	spinlock_t *ptl = NULL;
	pte_t *pte;
	pte_t pteval;

	/* Get locked pte */
	pte = rmap_get_checked_pte(pcm, rmap, &ptl);

	dump_pcache_rmap(rmap);

	pteval = ptep_get_and_clear(0, pte);

	pcache_paronoid_unmap_check(pteval, pcm, rmap);

	if (pte_present(pteval))
		flush_tlb_mm_range(rmap->owner->mm,
				   rmap->address,
				   rmap->address + PAGE_SIZE -1);

	list_del(&rmap->next);
	kfree(rmap);
	atomic_dec(&pcm->mapcount);

	spin_unlock(ptl);
	return ret;
}

static int pcache_mapcount_is_zero(struct pcache_meta *pcm)
{
	return !pcache_mapcount(pcm);
}

/**
 * pcache_try_to_unmap
 * @pcm: the pcache to get unmapped
 *
 * Tries to remove all the page table entries which are mapping this
 * pcache, used in the pageout path.
 *
 * Return:
 *	PCACHE_RMAP_SUCCEED	- we succeeded in removing all mappings
 *	PCACHE_RMAP_AGAIN	- we missed a mapping, try again later
 */
int pcache_try_to_unmap(struct pcache_meta *pcm)
{
	int ret;

	struct rmap_walk_control rwc = {
		.rmap_one = pcache_try_to_unmap_one,
		.done = pcache_mapcount_is_zero,
	};

	ret = rmap_walk(pcm, &rwc);
	if (!pcache_mapcount(pcm))
		ret = PCACHE_RMAP_SUCCEED;
	return ret;
}

static int pcache_wrprotect_one(struct pcache_meta *pcm,
				struct pcache_rmap *rmap, void *arg)
{
	int *protected = arg;
	int ret = PCACHE_RMAP_AGAIN;
	spinlock_t *ptl = NULL;
	pte_t *pte;
	pte_t entry;

	/* Get locked pte */
	pte = rmap_get_checked_pte(pcm, rmap, &ptl);
	dump_pcache_rmap(rmap);

	if (!pte_write(*pte))
		goto out;

	/*
	 * Note: These operations are protected by the pte lock.
	 * If other core has a pgfault after we clear the PTE,
	 * the pgfault will end up with pcache_fill_page(). This is
	 * okay because we still hold the pte lock. Inside pgfault
	 * function, it will check pte again after acquires the pte lock,
	 * the case where PTE is already set back by us.
	 *
	 * Or, if other core has a pgfault after we set the pte to
	 * read-only, the pgfault will end up with pcache_do_wp_page().
	 * Then it will wait until pcache flush finishes.
	 */
	entry = ptep_get_and_clear(0, pte);
	entry = pte_wrprotect(entry);
	entry = pte_mkclean(entry);
	pte_set(pte, entry);

	if (pte_present(entry))
		flush_tlb_mm_range(rmap->owner->mm,
				   rmap->address,
				   rmap->address + PAGE_SIZE -1);

	(*protected)++;

out:
	spin_unlock(ptl);
	return ret;
}

/**
 * pcache_wrprotect
 * @pcm: pcache line to protect
 *
 * This function will write-protect PTEs mapped to @pcm.
 * Return the number of PTEs that have been marked read-only.
 */
int pcache_wrprotect(struct pcache_meta *pcm)
{
	int protected = 0;
	struct rmap_walk_control rwc = {
		.arg = &protected,
		.rmap_one = pcache_wrprotect_one,
	};

	BUG_ON(!PcacheLocked(pcm));

	if (!pcache_mapped(pcm))
		return 0;

	rmap_walk(pcm, &rwc);

	return protected;
}

int rmap_walk(struct pcache_meta *pcm, struct rmap_walk_control *rwc)
{
	struct pcache_rmap *rmap, *keeper;
	struct pcache_set *pset;
	int ret = PCACHE_RMAP_AGAIN;

	pset = pcache_meta_to_pcache_set(pcm);

	/* rmap addition is protected by the pset lock */
	spin_lock(&pset->lock);
	list_for_each_entry_safe(rmap, keeper, &pcm->rmap, next) {
		ret = rwc->rmap_one(pcm, rmap, rwc->arg);
		if (ret != PCACHE_RMAP_AGAIN)
			break;

		if (rwc->done && rwc->done(pcm))
			break;
	}
	spin_unlock(&pset->lock);

	return ret;
}
