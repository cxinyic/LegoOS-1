/*
 * Copyright (c) 2016-2017 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <lego/bug.h>
#include <lego/mm.h>
#include <lego/spinlock.h>
#include <lego/comp_memory.h>
#include "../include/vm.h"
#include "../include/vm-pgtable.h"

/*
 * empty_zero_page is a shared zero-filled page.
 * Used to map to read-first anonymous page.
 */
static const unsigned long
empty_zero_page[PAGE_SIZE / sizeof(unsigned long)]
__attribute__((aligned(PAGE_SIZE)));

static inline unsigned long my_zero_vfn(void)
{
	return (unsigned long)empty_zero_page >> PAGE_SHIFT;
}

static int do_linear_fault(struct vm_area_struct *vma, unsigned long address,
			   unsigned int flags, pte_t *ptep, pmd_t *pmd,
			   pte_t entry)
{
	BUG();
	return 0;
}

static int do_swap_page(struct vm_area_struct *vma, unsigned long address,
			unsigned int flags, pte_t *ptep, pmd_t *pmd,
			pte_t entry)
{
	BUG();
	return 0;
}

static int do_wp_page(struct vm_area_struct *vma, unsigned long address,
		      unsigned int flags, pte_t *ptep, pmd_t *pmd, pte_t entry)
{
	BUG();
	return 0;
}

static int do_anonymous_page(struct vm_area_struct *vma, unsigned long address,
			     unsigned int flags, pte_t *page_table, pmd_t *pmd)
{
	pte_t entry;
	spinlock_t *ptl;
	unsigned long vaddr;
	struct lego_mm_struct *mm = vma->vm_mm;

	/* Use the zero-page for reads */
	if (!(flags & FAULT_FLAG_WRITE)) {
		entry = lego_vfn_pte(my_zero_vfn(),
				     vma->vm_page_prot);

		/* Someone else set it */
		page_table = lego_pte_offset_lock(mm, pmd, address, &ptl);
		if (!pte_none(*page_table))
			goto unlock;
		goto setpte;
	}

	vaddr = __get_free_page(GFP_KERNEL);
	if (!vaddr)
		return VM_FAULT_OOM;

	entry = lego_vfn_pte(vaddr >> PAGE_SHIFT, vma->vm_page_prot);
	if (vma->vm_flags & VM_WRITE)
		entry = pte_mkwrite(pte_mkdirty(entry));

	page_table = lego_pte_offset_lock(mm, pmd, address, &ptl);
	if (!pte_none(*page_table))
		goto unlock;

setpte:
	pte_set(page_table, entry);
unlock:
	lego_pte_unlock(page_table, ptl);
	return 0;
}

static int handle_pte_fault(struct vm_area_struct *vma, unsigned long address,
			    unsigned int flags, pte_t *pte, pmd_t *pmd)
{
	pte_t entry;
	spinlock_t *ptl;
	struct lego_mm_struct *mm = vma->vm_mm;

	entry = *pte;
	if (!pte_present(entry)) {
		if (pte_none(entry)) {
			if (vma->vm_ops && vma->vm_ops->fault)
				return do_linear_fault(vma, address, flags,
						       pte, pmd, entry);
			else
				return do_anonymous_page(vma, address, flags,
							 pte, pmd);
		}
		return do_swap_page(vma, address, flags, pte, pmd, entry);
	}

	ptl = lego_pte_lockptr(mm, pmd);
	spin_lock(ptl);
	if (unlikely(!pte_same(*pte, entry)))
		goto unlock;

	if (flags & FAULT_FLAG_WRITE) {
		if (!pte_write(entry))
			return do_wp_page(vma, address, flags, pte, pmd, entry);

		/* Update pte */
		entry = pte_mkdirty(entry);
		if (!pte_same(*pte, entry))
			*pte = entry;
	}

unlock:
	lego_pte_unlock(pte, ptl);
	return 0;
}

/*
 * Given a missing address, this function will establish the process's
 * virtual memory page table mapping.
 *
 * Note that:
 * Traditional page table:
 *	[process virtual address --> machine physical address]
 * Lego page table:
 *	[process virtual address --> kernel virtual address]
 *
 * Why we are using kernel virtual address here?
 * 1) Lego page table is not used by hardware walker.
 * 2) Kernel virtual address is sufficient for us to find the exact page
 *    in memory component. If we are using machine physical address, we
 *    will need another conversion from pa->va, after this function returns.
 *    (Since memory manager is running in kernel mode only, we have to use
 *     kernel virtual address to reference memory.)
 *
 * TODO:
 *	We are reusing the pud_offset etc. macros, which are using physical
 * 	address to fill page table. This is useless extra cost for us.
 *	Hence, we need special lego_pud_offset etc. macros to use kernel
 *	virtual address only.
 */
int handle_lego_mm_fault(struct vm_area_struct *vma, unsigned long address,
			 unsigned int flags, unsigned long *ret_va)
{
	struct lego_mm_struct *mm = vma->vm_mm;
	int ret;
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	pte_t *pte;

	pgd = lego_pgd_offset(mm, address);
	pud = lego_pud_alloc(mm, pgd, address);
	if (!pud)
		return VM_FAULT_OOM;
	pmd = lego_pmd_alloc(mm, pud, address);
	if (!pmd)
		return VM_FAULT_OOM;
	pte = lego_pte_alloc(mm, pmd, address);
	if (!pte)
		return VM_FAULT_OOM;

	ret = handle_pte_fault(vma, address, flags, pte, pmd);
	if (unlikely(ret))
		return ret;

	/*
	 * Return the kernel virtual address of the new
	 * allocated page:
	 */
	*ret_va = pte_val(*pte) & PTE_PFN_MASK;

	return 0;
}
