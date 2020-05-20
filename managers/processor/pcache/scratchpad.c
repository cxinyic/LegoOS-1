#include <lego/mm.h>
#include <lego/wait.h>
#include <lego/slab.h>
#include <lego/log2.h>
#include <lego/hash.h>
#include <lego/kernel.h>
#include <lego/pgfault.h>
#include <lego/syscalls.h>
#include <lego/jiffies.h>
#include <lego/profile.h>
#include <lego/fit_ibapi.h>
#include <lego/uaccess.h>
#include <lego/kernel.h>
#include <processor/pcache.h>
#include <processor/processor.h>
#include <processor/distvm.h>

int try_pin_one_page(struct mm_struct *mm, unsigned long virt_address)
{
    pgd_t *old_pgd;
	pud_t *old_pud;
	pmd_t *old_pmd;
	pte_t *old_pte;

    pgd_t *new_pgd;
	pud_t *new_pud;
	pmd_t *new_pmd;
	pte_t *new_pte;
    struct pcache_meta *old_pcm;
    struct pcache_meta *new_pcm;
    struct pcache_set *pset;
    int ret;

    old_pgd = pgd_offset(mm, virt_address);
	old_pud = pud_offset(old_pgd, virt_address);
	if (old_pud){
        old_pmd = pmd_offset(old_pud, virt_address);
	    if (old_pmd){
            old_pte = pte_offset(old_pmd,virt_address);
        }

    }
    
    pset = user_vaddr_to_pcache_set(virt_address);
	if(atomic_read(&pset->nr_pinned)+1>pcache_max_pinned){
        pr_info("Error: exceed max pinned number");
		return -1;
	}
    // data is in pcache
    if (likely(pte_present(*old_pte))){ 
        old_pcm = pte_to_pcache_meta(*old_pte);
        old_pcm->pin_flag = 1;
    
    }
    // data is in remote memory
    //emulate a page fault
    else{
        new_pgd = pgd_offset(mm, virt_address);
	    new_pud = pud_alloc(mm, new_pgd, virt_address);
	    if (!new_pud)
		    return VM_FAULT_OOM;
        new_pmd = pmd_alloc(mm, new_pud, virt_address);
	    if (!new_pmd)
		    return VM_FAULT_OOM;
	    new_pte = pte_alloc(mm, new_pmd, virt_address);
	    if (!new_pte)
		    return VM_FAULT_OOM;
        ret= pcache_handle_pte_fault(mm, virt_address, new_pte, new_pmd, 0);
        if (ret<0){
            return ret;
        }
        new_pcm = pte_to_pcache_meta(*new_pte);
        new_pcm->pin_flag = 1;
        
    }
    atomic_inc(&pset->nr_pinned);
    return 0;
}
int try_unpin_one_page(struct mm_struct *mm, unsigned long virt_address)
{
    pgd_t *old_pgd;
	pud_t *old_pud;
	pmd_t *old_pmd;
	pte_t *old_pte;
    
    struct pcache_meta *old_pcm;
    struct pcache_set *pset;

    old_pgd = pgd_offset(mm, virt_address);
	old_pud = pud_offset(old_pgd, virt_address);
	if (old_pud){
        old_pmd = pmd_offset(old_pud, virt_address);
	    if (old_pmd){
            old_pte = pte_offset(old_pmd,virt_address);
        }

    }
    if (likely(pte_present(*old_pte))){ 
        old_pcm = pte_to_pcache_meta(*old_pte);
        old_pcm->pin_flag = 0;
        pset = user_vaddr_to_pcache_set(virt_address);
        atomic_dec(&pset->nr_pinned);
        return 0;
    
    }
    else{
        return -1;
    }


}

int try_pin(struct mm_struct *mm, unsigned long virt_address, unsigned long len)
{
    unsigned long prev_len = len;
    unsigned long offset;
    int ret;
    len = PAGE_ALIGN(len);
    if (!len){
        return -1;
    }
    offset = offset_in_page(virt_address);
    if ((offset+prev_len)>len){
        len+=PAGE_SIZE;
    }

    int nr_pcm_pin = len / PAGE_SIZE;
    int i=0;
    for (i=0;i<nr_pcm_pin;i++){
        ret = try_pin_one_page(mm,virt_address+i*PAGE_SIZE);
        if (ret<0){
            return -1;
        }
    }
    return 0;
}

int try_unpin(struct mm_struct *mm, unsigned long virt_address, unsigned long len)
{
    unsigned long prev_len = len;
    unsigned long offset;
    int ret;
    len = PAGE_ALIGN(len);
    if (!len){
        return -1;
    }
    offset = offset_in_page(virt_address);
    if ((offset+prev_len)>len){
        len+=PAGE_SIZE;
    }

    int nr_pcm_unpin = len / PAGE_SIZE;
    int i=0;
    for (i=0;i<nr_pcm_unpin;i++){
        ret = try_unpin_one_page(mm,virt_address+i*PAGE_SIZE);
        if (ret<0){
            return -1;
        }
    }
    return 0;

}