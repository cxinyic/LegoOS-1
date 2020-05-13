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
#include <processor/scratchpad.h>

unsigned long virt_sp_alloc(unsigned long len){
    struct p2m_sp_alloc_struct payload;
    struct p2m_sp_alloc_reply_struct reply;
    long ret_len, ret_addr;
    pr_info("Start:virt_sp_alloc");
    
    
    
    
	len = PAGE_ALIGN(len);
	if (!len)
		return -ENOMEM;
    payload.pid = current->pid;
    payload.len = len;
    ret_len = net_send_reply_timeout(current_memory_home_node(), P2M_SP_ALLOC,
			&payload, sizeof(payload), &reply, sizeof(reply),
			false, DEF_NET_TIMEOUT);
    if (likely(ret_len == sizeof(reply))) {
		if (likely(reply.ret == RET_OKAY))
            {pr_info("Continue1:virt_sp_alloc");
			ret_addr = reply.ret_sp;}
		else
			ret_addr = (s64)reply.ret;
	} else
		ret_addr = -EIO;
    return ret_addr;

    

}
static inline struct pcache_meta *
__dequeue_free_list_head(struct pcache_set *pset)
{
        struct pcache_meta *pcm;

        pcm = list_first_entry(&pset->free_head, struct pcache_meta, free_list);
        list_del(&pcm->free_list);
        return pcm;
}
static inline void prep_new_pcache_meta(struct pcache_meta *pcm)
{
	INIT_LIST_HEAD(&pcm->rmap);
	init_pcache_lru(pcm);
	pcache_mapcount_reset(pcm);
	init_pcache_ref_count(pcm);
}

struct pcache_meta *sp_alloc_one_pcm(void)
{
    struct pcache_meta *pcm;
    struct pcache_set *pset = sp_set_map;
    spin_lock(&pset->free_lock);
    if (list_empty(&pset->free_head)){
        spin_unlock(&pset->free_lock);
        return NULL;
    }
    pcm = __dequeue_free_list_head(pset);
    spin_unlock(&pset->free_lock);
    smp_store_mb(pcm->bits, 0);
    prep_new_pcache_meta(pcm);
    inc_sp_used();
    return pcm;
}

int sp_do_fill_page(unsigned long address, struct pcache_meta *pcm){
    pr_info("Start: sp_do_fill_page");
    struct p2m_pcache_miss_msg msg;
    void *kva = sp_meta_to_kva(pcm);
    int ret, len, dst_nid;

    dst_nid = get_memory_node(current, address);
    fill_common_header(&msg, P2M_PCACHE_MISS);
    msg.has_flush_msg = 0;
    msg.pid = current->pid;
    msg.tgid = current->tgid;
    msg.flags = 0;
    msg.missing_vaddr = address;

    len = ibapi_send_reply_timeout(dst_nid, &msg, sizeof(msg),
					       kva, PCACHE_LINE_SIZE, false,
					       DEF_NET_TIMEOUT);
    if (unlikely(len < (int)PCACHE_LINE_SIZE)) {
		if (likely(len == sizeof(int))) {
			ret = -EFAULT;
			goto out;
		} else if (len < 0) {
			ret = len;
			WARN_ON_ONCE(1);
			goto out;
		} else {
			WARN(1, "Invalid reply length: %d\n", len);
			ret = -EFAULT;
			goto out;
		}
	}

	ret = 0;
out:
	return ret;
}


int build_new_mapping_one_page(struct mm_struct *mm,
            unsigned long new_virt_address, unsigned long old_virt_address, struct pcache_meta *new_pcm){
    pr_info("Start: build_new_mapping_one_page");
    pgd_t *new_pgd;
	pud_t *new_pud;
	pmd_t *new_pmd;
	pte_t *new_pte;

    pgd_t *old_pgd;
	pud_t *old_pud;
	pmd_t *old_pmd;
	pte_t *old_pte;

    struct pcache_meta *old_pcm;
    void *old_kva;
    void *new_kva;
    int ret;

    pte_t *page_table;
    spinlock_t *ptl;




	
	old_pgd = pgd_offset(mm, old_virt_address);
    pr_info("S6: build_new_mapping_one_page");
	old_pud = pud_offset(old_pgd, old_virt_address);
    pr_info("S7: build_new_mapping_one_page");
	if (old_pud){
        pr_info("S8: build_new_mapping_one_page");
        old_pmd = pmd_offset(old_pud, old_virt_address);
	    if (old_pmd){
            pr_info("S9: build_new_mapping_one_page");
            old_pte = pte_offset(old_pmd,old_virt_address);
        }

    }
    pr_info("Start1: build_new_mapping_one_page");

    new_pgd = pgd_offset(mm, new_virt_address);
    pr_info("S0: build_new_mapping_one_page");
	new_pud = pud_alloc(mm, new_pgd, new_virt_address);
    pr_info("S1: build_new_mapping_one_page");
	if (!new_pud)
		return VM_FAULT_OOM;
    pr_info("S2 build_new_mapping_one_page");
	new_pmd = pmd_alloc(mm, new_pud, new_virt_address);
    pr_info("S3: build_new_mapping_one_page");
	if (!new_pmd)
		return VM_FAULT_OOM;
    pr_info("S4: build_new_mapping_one_page");
	new_pte = pte_alloc(mm, new_pmd, new_virt_address);
    pr_info("S5: build_new_mapping_one_page");
	if (!new_pte)
		return VM_FAULT_OOM;
    pr_info("Start0: build_new_mapping_one_page");
	
    
    pte_t entry;
    entry = sp_mk_pte(new_pcm, PAGE_SHARED_EXEC);
    pr_info("Continue0: build_new_mapping_one_page");
    
    pr_info("Continue1: build_new_mapping_one_page");
    /* data are in pcache: local*/
    if (likely(pte_present(*old_pte))){
        pr_info("Continue2: build_new_mapping_one_page");
        old_pcm = pte_to_pcache_meta(*old_pte);
        old_kva = pcache_meta_to_kva(old_pcm);
        new_kva = sp_meta_to_kva(new_pcm);
        /* copy from kernel virtual address to kernel virtual address*/
        memcpy(new_kva,old_kva,PAGE_SIZE);

    }
    /* data are in remote memory */
    else{
        pr_info("Continue3: build_new_mapping_one_page");
        ret = sp_do_fill_page(old_virt_address,new_pcm);
        if(unlikely(ret)){
            ret = VM_FAULT_SIGSEGV;
            goto out;
        }
    }
    pr_info("Continue4: build_new_mapping_one_page");
    
    pte_set(new_pte, entry);
    /* TODO: add rmap information */
    ret = sp_add_rmap(new_pcm, new_pte, old_virt_address,
			      mm, current->group_leader, RMAP_SP_COPY);
    if (unlikely(ret)) {
        pr_info("Continue5: build_new_mapping_one_page");
		pte_clear(page_table);
		ret = VM_FAULT_OOM;
		goto out;
	}
    pr_info("Continue6: build_new_mapping_one_page");
    ret = sp_add_rmap(new_pcm, new_pte, new_virt_address,
			      mm, current->group_leader, RMAP_SP_COPY);
    pr_info("Continue7: build_new_mapping_one_page");
    if (unlikely(ret)) {
        pr_info("Continue8: build_new_mapping_one_page");
		pte_clear(page_table);
		ret = VM_FAULT_OOM;
		goto out;
	}
    
    return 0;
            

out:
    put_pcache(new_pcm);
    
    return ret;
    
}
int build_new_mapping(struct mm_struct *mm, unsigned long new_virt_address, 
            unsigned long old_virt_address, unsigned long len)
{
    pr_info("Start: build_new_mapping");
    pr_info("old virt addr: %#llx\n", old_virt_address);
    pr_info("old virt addr: %#llx\n", new_virt_address);
    len = PAGE_ALIGN(len);
    if (!len){
        return -1;
    }
    struct pcache_meta *pcm;
    unsigned long sp_current_used = sp_used();
    int nr_pcm_alloc = len / PAGE_SIZE;
    int ret;
    
    
   
    /* no enough space to pin in scratch pad */
    if(nr_pcm_alloc>sp_nr_cachelines-sp_current_used){
        return -1;
    }
    int i;
    for (i=0;i<nr_pcm_alloc;i++){
        pr_info("Continue1: build_new_mapping");
        pcm = sp_alloc_one_pcm();
        pr_info("Continue2: build_new_mapping");
        ret = build_new_mapping_one_page(mm, new_virt_address+i*PAGE_SIZE, 
        old_virt_address+i*PAGE_SIZE,pcm);
        if (ret<0){
            pr_info("Fail: build_new_mapping");
            return -1;
        }
        
    }
    return 0;
}


