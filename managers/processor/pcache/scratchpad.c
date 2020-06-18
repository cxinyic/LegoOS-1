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
    pr_info("try_pin_one_page:debug0");

    old_pgd = pgd_offset(mm, virt_address);
    pr_info("try_pin_one_page:debug01");
    
    if(!pgd_none(*old_pgd)){
        pr_info("old_pgd:  %#llx\n", old_pgd);
        old_pud = pud_offset(old_pgd, virt_address);
        pr_info("try_pin_one_page:debug02");
	if (!pud_none(*old_pud)){
        pr_info("old_pud:  %#llx\n", old_pud);
        old_pmd = pmd_offset(old_pud, virt_address);
        pr_info("try_pin_one_page:debug03");
	    if (!pmd_none(*old_pmd)){
            pr_info("old_pmd:  %#llx\n", old_pmd);
            old_pte = pte_offset(old_pmd,virt_address);
            pr_info("try_pin_one_page:debug04");
        }

    }

    }
	
    //pr_info("old_pte:  %#llx\n", old_pte);
    pr_info("try_pin_one_page:debug1");
    
    pset = user_vaddr_to_pcache_set(virt_address);
    //pr_info("try_pin_one_page:debug12");
    /*
	if(atomic_read(&pset->nr_pinned)+1>pcache_max_pinned){
        pr_info("Error: exceed max pinned number");
		return -1;
	}*/
    //pr_info("try_pin_one_page:debug13");
    // data is in pcache
    
    if (likely(pte_present(*old_pte))){ 
        pr_info("try_pin_one_page:debug2");
        old_pcm = pte_to_pcache_meta(*old_pte);
        pr_info("try_pin_one_page:debug3");
        pr_info("virt_address3:  %#llx\n", virt_address);
        pr_info("old_pmd:  %#llx\n", old_pmd);
        pr_info("old_pte:  %#llx\n", old_pte);
        old_pcm->pin_flag = 1;
        pr_info("try_pin_one_page:debug4");
        pr_info("old_pcm:  %#llx\n", old_pcm);
        PROFILE_START(detach_from_lru);
        detach_from_lru(old_pcm);
        PROFILE_LEAVE(detach_from_lru);
        pr_info("try_pin_one_page:debug5");
    
    }
    // data is in remote memory
    //emulate a page fault
    else{
        pr_info("try_pin_one_page_remote:debug3");
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

        pr_info("try_pin_one_page:debug4_remote");
        

        pr_info("virt_address4:  %#llx\n", virt_address);
        pr_info("new_pmd:  %#llx\n", new_pmd);
        pr_info("new_pte:  %#llx\n", new_pte);

        unsigned long new_pa = pte_val(*new_pte) & PTE_PFN_MASK;
        pr_info("new_pa: %#llx\n", new_pa);
        
        
        
        PROFILE_START(pcache_handle_pte_fault);
        pr_info("try_pin_one_page_remote:debug5");
        

        ret= pcache_handle_pte_fault(mm, virt_address, new_pte, new_pmd, 0);
        pr_info("try_pin_one_page_remote:debug6");
        PROFILE_LEAVE(pcache_handle_pte_fault);
        if (ret!=0){
            pr_info("bad return: %lu\n", ret);
            return ret;
        }
        
        
        //new_pcm->pin_flag = 1;
        new_pcm = pte_to_pcache_meta(*new_pte);
        unsigned long new_pa1 = pte_val(*new_pte) & PTE_PFN_MASK;
        pr_info("new_pa: %#llx\n", new_pa1);
        pr_info("new_pcm:  %#llx\n", new_pcm);
        pr_info("try_pin_one_page_remote:debug7");
        PROFILE_START(detach_from_lru);
        detach_from_lru(new_pcm);
        PROFILE_LEAVE(detach_from_lru);
        //del_from_lru_list(new_pcm,pset);
        pr_info("try_pin_one_page_remote:debug8");
        
    }
    //atomic_inc(&pset->nr_pinned);

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
        if (old_pcm->pin_flag==0){
            return 0;
        }
        old_pcm->pin_flag = 0;
        pset = user_vaddr_to_pcache_set(virt_address);
        atomic_dec(&pset->nr_pinned);
        add_to_lru_list(old_pcm,pset);
        return 0;
    
    }
    else{
        return 0;
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
        pr_info("virt_address2:  %#llx\n", virt_address);
        PROFILE_START(try_pin_one_page);
        ret = try_pin_one_page(mm,virt_address+i*PAGE_SIZE);
        PROFILE_LEAVE(try_pin_one_page);
        if (ret!=0){
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