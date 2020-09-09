#include <lego/kernel.h>
#include <lego/slab.h>
#include <lego/fit_ibapi.h>
#include <lego/uaccess.h>
#include <lego/err.h>
#include <processor/processor.h>
#include <processor/pcache.h>
#include <processor/distvm.h>
#include <lego/syscalls.h>

#if 0
static __u32 count_param(const char __user * const * __user argv,
             int max, __u32 *size)
{
    int i = 0;

    if (!argv || !size)
        return 0;

    for (;;) {
        const char *p;
        __u32 len;

        if (get_user(p, argv + i))
            return -EFAULT;

        if (!p)
            break;

        if (i >= max)
            return -E2BIG;

        /*
         * Vulnerable to read-after-check attack?
         */
        len = strnlen_user(p, MAX_ARG_STRLEN);
        if (unlikely(!len))
            return -EINVAL;

        *size += len;
        i++;
    }
    return i;
}

/* Copy strings from userspace to core-kernel paylaod */
static int copy_strings(__u32 argc, const char __user * const * __user argv,
            struct p2m_c_pushdown_payload_binary *payload, __u32 *array_oft)
{
    int i;
    long copied;
    char *dst;
    const char *src;

    BUG_ON(!argc || !argv || !payload || !array_oft);

    dst = (char *)&(payload->array) + *array_oft;
    for (i = 0; i < argc; i++) {
        if (get_user(src, argv + i))
            return -EFAULT;

        copied = strncpy_from_user(dst, src, MAX_ARG_STRLEN);
        if (unlikely(copied < 0))
            return -EFAULT;

        // pr_info("QZ: Copy from user: %s\n", dst); // Copy is good

        copied++; /* including terminal NULL */
        *array_oft += copied;
        dst += copied;
    }

    return 0;
}

/* Copy strings from userspace to core-kernel paylaod */
static int copy_strings_fix(__u32 argc, const char __user * const * __user argv,
            struct p2m_c_pushdown_payload_fix *payload, __u32 *array_oft)
{
    int i;
    long copied;
    char *dst;
    const char *src;

    BUG_ON(!argc || !argv || !payload || !array_oft);

    dst = payload->array + *array_oft;
    for (i = 0; i < argc; i++) {
        if (get_user(src, argv + i))
            return -EFAULT;

        copied = strncpy_from_user(dst, src, MAX_ARG_STRLEN);
        if (unlikely(copied < 0))
            return -EFAULT;

        // pr_info("QZ: Copy from user: %s\n", dst); // Copy is good

        copied++; /* including terminal NULL */
        *array_oft += copied;
        dst += copied;
    }

    return 0;
}

/*
 * Prepare the payload being sent to memory-component
 */
static void *prepare_msg(const char __user *filename,
                  const char __user * const * __user argv,
                  const char __user * const * __user envp,
                  __u32 *len_msg)
{
    __u32 argc, envc, size = 0, array_oft = 0, payload_size;
    long copied;
    struct common_header *hdr;
    struct p2m_c_pushdown_payload_binary *payload;
    void *msg;

    *len_msg = 0;
    /* Count the total payload size first */
    argc = count_param(argv, MAX_ARG_STRINGS, &size);
    if (argc < 0)
        return ERR_PTR(argc);

    envc = count_param(envp, MAX_ARG_STRINGS, &size);
    if (envc < 0)
        return ERR_PTR(envc);

    *len_msg += sizeof(*hdr);
    /* then allocate payload */
    payload_size = sizeof(*payload) + size - sizeof(char *);
    *len_msg += payload_size;
    msg = kzalloc(*len_msg, GFP_KERNEL);
    if (!msg)
        return ERR_PTR(-ENOMEM);

    hdr = msg;

    hdr->opcode = P2M_C_PUSHDOWN;
    hdr->src_nid = LEGO_LOCAL_NID;

    payload = msg + sizeof(*hdr);
    /* then copy strings and fill payload */
    payload->pid = current->tgid;
    payload->payload_size = payload_size;
    payload->argc = argc;
    payload->envc = envc;

    copied = strncpy_from_user(payload->filename, filename, MAX_FILENAME_LENGTH);
    if (unlikely(copied < 0))
        goto out;

    array_oft = 0;
    if (copy_strings(argc, argv, payload, &array_oft))
        goto out;

    if (copy_strings(envc, envp, payload, &array_oft))
        goto out;

    return msg;

out:
    kfree(payload);
    return ERR_PTR(-EFAULT);
}

/*
 * Prepare the payload being sent to memory-component
 */
static void *prepare_payload(const char __user *filename,
                  const char __user * const * __user argv,
                  const char __user * const * __user envp,
                  __u32 *payload_size)
{
    __u32 argc, envc, size = 0, array_oft = 0;
    long copied;
    struct p2m_c_pushdown_payload_binary *payload;

    /* Count the total payload size first */
    argc = count_param(argv, MAX_ARG_STRINGS, &size);
    if (argc < 0)
        return ERR_PTR(argc);

    envc = count_param(envp, MAX_ARG_STRINGS, &size);
    if (envc < 0)
        return ERR_PTR(envc);

    *payload_size = sizeof(*payload) + size - sizeof(char *);
    payload = kzalloc(*payload_size, GFP_KERNEL);
    if (!payload)
        return ERR_PTR(-ENOMEM);

    /* then copy strings and fill payload */
    payload->pid = current->tgid;
    payload->payload_size = *payload_size;
    payload->argc = argc;
    payload->envc = envc;

    copied = strncpy_from_user(payload->filename, filename, MAX_FILENAME_LENGTH);
    if (unlikely(copied < 0))
        goto out;

    array_oft = 0;
    if (copy_strings(argc, argv, payload, &array_oft))
        goto out;

    if (copy_strings(envc, envp, payload, &array_oft))
        goto out;

    return payload;

out:
    kfree(payload);
    return ERR_PTR(-EFAULT);
}

/*
 * Prepare the payload being sent to memory-component
 */
static int prepare_payload_fix(const char __user *filename,
                  const char __user * const * __user argv,
                  const char __user * const * __user envp,
                  struct p2m_c_pushdown_payload_fix *payload)
{
    __u32 argc, envc, size = 0, array_oft = 0;
    long copied;

    /* Count the total payload size first */
    argc = count_param(argv, MAX_ARG_STRINGS, &size);
    if (argc < 0)
        goto out;

    envc = count_param(envp, MAX_ARG_STRINGS, &size);
    if (envc < 0)
        goto out;

    payload->pid = current->tgid;
    payload->argc = argc;
    payload->envc = envc;

    copied = strncpy_from_user(payload->filename, filename, MAX_FILENAME_LENGTH);
    if (unlikely(copied < 0))
        goto out;

    array_oft = 0;
    if (copy_strings_fix(argc, argv, payload, &array_oft))
        goto out;

    if (copy_strings_fix(envc, envp, payload, &array_oft))
        goto out;

    return 0;

out:
    return -1;
}

/*
 * Push compute to memory node.
 * */
long sys_c_pushdown_backup(const char __user *filename,
        const char __user * const * __user argv,
        const char __user * const * __user envp) {
    long retval;
    ssize_t retlen;
    ssize_t *retval_ptr;
    __u32 len_retbuf;
    void *retbuf;
    int mem_node;   /* = pgcache_node if defined or memory homenode */
    struct p2m_c_pushdown_payload_fix payload;
    
    len_retbuf = sizeof(long);
    retbuf = kmalloc(len_retbuf, GFP_KERNEL);
    if (!retbuf)
        return -ENOMEM;

    if (prepare_payload_fix(filename, argv, envp, &payload) < 0) {
        kfree(retbuf);
        return -ENOMEM;
    }

    mem_node = current_pgcache_home_node();

    retlen = net_send_reply_timeout(mem_node, P2M_C_PUSHDOWN, &payload,
        sizeof(payload), retbuf, len_retbuf, false, FIT_MAX_TIMEOUT_SEC);

    if (likely(retlen > 0)) {
        if (retlen == len_retbuf) {
            retval_ptr = retbuf;
            retval = *retval_ptr;
        } else {
            retval = -1;
        }
    } else {
        retval = retlen;
    }

    kfree(retbuf);
    return retval;
}

long sys_c_pushdown_binary(const char __user *filename,
        const char __user * const * __user argv,
        const char __user * const * __user envp) {
    long retval;
    ssize_t retlen;
    ssize_t *retval_ptr;
    __u32 len_retbuf;
    __u32 len_msg;
    void *retbuf, *msg;
    int mem_node;   /* = pgcache_node if defined or memory homenode */
    
    len_retbuf = sizeof(long);
    retbuf = kmalloc(len_retbuf, GFP_KERNEL);
    if (!retbuf)
        return -ENOMEM;

    msg = prepare_msg(filename, argv, envp, &len_msg);
    if (IS_ERR(msg)) {
        kfree(retbuf);
        return -ENOMEM;
    }

    mem_node = current_pgcache_home_node();

    retlen = ibapi_send_reply_imm(mem_node, msg, len_msg,
            retbuf, len_retbuf, false);
    if (retlen != len_retbuf) {
        WARN_ON_ONCE(1);
        retval = -EIO;
        goto out;
    }

    retval_ptr = retbuf;
    retval = *retval_ptr;

    // copy_to_user(res, &retval, sizeof(long));

out:
    kfree(msg);
    kfree(retbuf);
    return retval;
}
#endif

// QZ: Flushing should be hinted by the user, instead of making it automatic.

///*
// * Flush all dirty pages
// * */
//static void flush_dirty_data_before_compute(void) {
//
//}
//
//static void flush_data_before_compute(unsigned long address) {
//    // Currently flush all dirty pages back to memory and clean page table
//    //
//    // Let's start with flushing the arg address first
//    struct pcache_meta* pcm;
//    bool dirty;
//
//    // pcm = __addr2meta(address); // This pcm is just the first cl in the set, so
//                                   // it might not be the actual pcm.
//    pgd_t *pgd = pgd_offset(current->mm, address);
//    pud_t *pud = pud_offset(pgd, address);
//    pmd_t *pmd = pmd_offset(pud, address);
//    pte_t *pte = pte_offset(pmd, address);
//    pcm = pte_to_pcache_meta(*pte);
//
//    dirty = pcache_try_to_unmap_reserve_check_dirty(pcm);
//    if(likely(dirty)) {
//        pr_debug("QZ (c_pushdown): page is dirty, so flushing...\n");
//        pcache_flush_one(pcm);
//    } else {
//        pr_debug("QZ (c_pushdown): page is not dirty, so good to go!\n");
//    }
//
//    pcache_free_reserved_rmap(pcm);
//
//    // pcache_try_to_unmap_reserve_check_dirty has flushed the tlb. no need to flush again
//    // flush_tlb_range(current->mm, address, address + PAGE_SIZE -1);
//}
//
//static void bring_data_after_compute(unsigned long address) {
//    // Issue a page fault
//    pcache_handle_fault(current->mm, address, FAULT_FLAG_KILLABLE | FAULT_FLAG_USER | FAULT_FLAG_WRITE);
//}

/* static inline long walk_pte_level_dfs(pmd_t* pmd) {
    pte_t *pte;
    long sub_page_count;
    long i;
    
    i = 0;
    sub_page_count = 0;
    pte = (pte_t *) pmd_page_vaddr(*pmd);
    while (i != PTRS_PER_PTE) {
        if(pte_none(*pte)) continue;
        if(!pte_present(*pte)) continue;
        sub_page_count++;
        // TODO: do something with the PTE. 1) check dirty 2) sync 
        pte++;
        i++;
    }
    return sub_page_count;
}

static inline long walk_pmd_level_dfs(pud_t* pud) {
    pmd_t *pmd;
    long sub_page_count;
    long i;
    
    i = 0;
    sub_page_count = 0;
    pmd = (pmd_t *) pud_page_vaddr(*pud);
    while(i != PTRS_PER_PTE) {
        if (pmd_none_or_clear_bad(pmd) && !pmd_present(*pmd)) { continue; }
        else {
            sub_page_count++;
            sub_page_count += walk_pte_level_dfs(pmd);
        }
        pmd++;
        i++;
    }
    return sub_page_count;
}

static inline long walk_pud_level_dfs(pgd_t* pgd) {
    pud_t *pud;
    long sub_page_count;
    long i;

    i = 0;
    sub_page_count = 0;
    pud = (pud_t *) pgd_page_vaddr(*pgd);
    while (i != PTRS_PER_PUD) {
        if (pud_none_or_clear_bad(pud) && !pud_present(*pud)) { continue; }
        else {
            sub_page_count++;
            sub_page_count += walk_pmd_level_dfs(pud);
        }
        pud++;
        i++;
    }
    return sub_page_count;
}

static long walk_pgd_level_dfs(void) {
    unsigned long start_addr
    pgd_t *pgd;
    long sub_page_count;
    long start_addr;
    long i;
    
    i = 0;
    start_addr = 0;
    sub_page_count = 0;
    pgd = pgd_offset(current->mm, start_addr);
    while (i != PTRS_PER_PGD) {
        if (pgd_none_or_clear_bad(pgd) && !pgd_present(*pgd)) { continue; } 
        else {
            sub_page_count++;
            sub_page_count += walk_pud_level_dfs(pgd);
        }
        pgd++;
        i++;
    }
    return sub_page_count;
}

static inline void walk_page_table_dfs(void) {
    long total_page_count;
    total_page_count = walk_pgd_level_dfs();
    printk("[ WALK PAGE TABLE DFS ] total_page_count = %ld\n", total_page_count);
} */

/* naive: sync every page preemptively */
static inline void sync_every_page(void) {
    struct pcache_meta *pcm;
    int nr = 0;
    int count = 0;

    // printk("[ SYNC EVERY PAGE ] sync every page is called!\n");
    
    pcache_for_each_way(pcm, nr) {
        pcache_flush_one(pcm);
        count++;
    }
    // printk("[NUMBER of page synced] %d\n", count);
}

static int __sync_back_pcm(struct pcache_meta *pcm, struct pcache_rmap *rmap, void *arg) {
    int *nr_flushed = arg;
    unsigned long vaddr = 0;
    void* va_cache;
    int dst_nid = 0;
    int len;
    struct p2m_pcache_miss_msg msg;
    
    if (rmap->owner_process->pid == current->pid) {
        vaddr = rmap->address;
        va_cache = pcache_meta_to_kva(pcm);
        dst_nid = get_memory_node(current, vaddr);

        fill_common_header(&msg, P2M_PCACHE_MISS);
        msg.has_flush_msg = 0;
        msg.pid = current->pid;
        msg.tgid = current->tgid;
        msg.flags = FAULT_FLAG_KILLABLE | FAULT_FLAG_USER;
        msg.missing_vaddr = vaddr;
        len = ibapi_send_reply_timeout(dst_nid, &msg, sizeof(msg), va_cache, PCACHE_LINE_SIZE, false, DEF_NET_TIMEOUT);
    }
    (*nr_flushed)++;
    return PCACHE_RMAP_AGAIN;
}

static inline void sync_back_every_page(void) {
    struct pcache_meta *pcm;
    int nr = 0;
    int nr_flushed = 0;
    struct rmap_walk_control rwc = {
        .arg = &nr_flushed,
        .rmap_one = __sync_back_pcm
    };

    pcache_for_each_way(pcm, nr) {
        rmap_walk(pcm, &rwc);
    }
}

/* find dirty pages */
static int __flush_only_if_dirty(struct pcache_meta *pcm, struct pcache_rmap *rmap, void *arg)
{
        int *nr_flushed = arg;
        pte_t *pte;

        if (rmap->owner_process->pid == current->pid) { // if same process
            pte = rmap->page_table;
            if (!pte_none(*pte) && pte_present(*pte)) {
                if (likely(pte_dirty(*pte))) {
                    (*nr_flushed)++;
                    pcache_flush_one(pcm);
                }
            }
        }
        return PCACHE_RMAP_AGAIN;
}

static inline void sync_dirty_pages_only(void) {
    struct pcache_meta *pcm;
    int nr = 0;
    int nr_flushed = 0;
    struct rmap_walk_control rwc = {
        .arg = &nr_flushed,
        .rmap_one = __flush_only_if_dirty,
    };
    printk("[ SYNC EVERY PAGE ] sync every page is called!\n");
    pcache_for_each_way(pcm, nr) {
        rmap_walk(pcm, &rwc);
    }
    printk("[NUMBER of page synced] %d\n", nr_flushed);
}

/* Build Page List 
struct pgtable_sync_arg {
    int index;
    struct p2m_page_info_kv_pair* pg_list_addr;
};

static int __insert_page_info(struct pcache_meta *pcm, struct pcache_rmap *rmap, void *arg)
{
    struct pgtable_sync_arg *sync_arg = arg;
    pte_t *pte;

    if (rmap->owner_process->pid == current->pid) { // if same process
        pte = rmap->page_table;
        if (!pte_none(*pte) && pte_present(*pte)) {
            (sync_arg->pg_list_addr)[sync_arg->index].addr = rmap->address;
            if (likely(pte_dirty(*pte))) {
                (sync_arg->pg_list_addr)[sync_arg->index].is_dirty = 1;
            } else {
                (sync_arg->pg_list_addr)[sync_arg->index].is_dirty = 0; 
            }
            (sync_arg->index)++;
        }
    }
    return PCACHE_RMAP_AGAIN;
}

static inline int sync_pgtable_info(void* list) {
    struct p2m_page_info_kv_pair* pg_list = (struct p2m_page_info_kv_pair*)list;
    struct pcache_meta *pcm;
    int nr = 0;
    struct pgtable_sync_arg arg = {
        .index = 0,
        .pg_list_addr = pg_list,
    };
    struct rmap_walk_control rwc = {
        .arg = &arg,
        .rmap_one = __insert_page_info,
    };
    printk("[ SYNC PGTABLE LIST ] sync pgtable list is called!\n");
    pcache_for_each_way(pcm, nr) {
        rmap_walk(pcm, &rwc);
    }
    printk("[NUMBER of page in list] %d\n", arg.index);
    return arg.index;
} */


asmlinkage long sys_c_pushdown(const char __user *ip, const char __user *sp, const char __user *arg) {
#ifdef CONFIG_COMP_PROCESSOR
    long retval;
    ssize_t retlen;
    ssize_t *retval_ptr;
    __u32 len_retbuf;
    __u32 len_msg;
    struct common_header *hdr;
    struct p2m_c_pushdown_payload *payload;
    void *retbuf, *msg;
    int mem_node;   /* = pgcache_node if defined or memory homenode */

    // Do necessary flush before push the compute to memory
    // flush_data_before_compute((__u64)arg);
    
    sync_every_page();
    //sync_dirty_pages_only();
    
    len_retbuf = sizeof(long);
    retbuf = kmalloc(len_retbuf, GFP_KERNEL);
    if (!retbuf)
        return -ENOMEM;

    len_msg = sizeof(*hdr) + sizeof(*payload);
    msg = kzalloc(len_msg, GFP_KERNEL);
    if (IS_ERR(msg)) {
        kfree(retbuf);
        return -ENOMEM;
    }

    hdr = msg;
    hdr->opcode = P2M_C_PUSHDOWN;
    hdr->src_nid = LEGO_LOCAL_NID;
    
    payload = msg + sizeof(*hdr);
    /* then copy strings and fill payload */
    payload->pid = current->pid;
    payload->tgid = current->tgid;
    payload->parent_tgid = current->real_parent->tgid;
    payload->ip = (__u64)ip;
    payload->sp = (__u64)sp;
    payload->arg = (__u64)arg;
    // payload->pg_count = (__u32)sync_pgtable_info(payload->pg_list);

    mem_node = current_pgcache_home_node();

    retlen = ibapi_send_reply_imm(mem_node, msg, len_msg,
            retbuf, len_retbuf, false);
    if (retlen != len_retbuf) {
        WARN_ON_ONCE(1);
        retval = -EIO;
        goto out;
    }

    retval_ptr = retbuf;
    retval = *retval_ptr;

    // pr_debug("c_pushdown is returned.\n");

    // bring the data back
    // bring_data_after_compute((__u64)arg);

    // copy_to_user(res, &retval, sizeof(long));

out:
    kfree(msg);
    kfree(retbuf);
    sync_back_every_page();
    return retval;
#else
    return -1;
#endif
}
