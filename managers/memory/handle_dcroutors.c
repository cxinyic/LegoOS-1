#include <lego/slab.h>
#include <lego/kernel.h>
#include <lego/uaccess.h>
#include <lego/fit_ibapi.h>
#include <lego/time.h>

#include <lego/dcroutors/c_routor.h>

#include <memory/vm.h>
#include <memory/pid.h>
#include <memory/thread_pool.h>
#include <memory/dcr_syncmem.h>

/*
 *  * OPCODE: P2M_REMOTE_AGGREGATE
 *  */
struct lineitem {
    int orderkey;
    int partkey;
    int suppkey;
    int linenumber;
    int quantity;
    int extendedprice;
    int discount;
    double tax;
    char returnflag;
    char linestatus;
};

void execute_in_kernel(struct p2m_remote_aggregate_payload *payload,
        struct common_header *hdr, struct thpool_buffer *tb) {
    struct lineitem k_data;
    int avg_quan, i;
    size_t unit_size;
    long k_res;
    struct lego_task_struct *tsk;
    size_t n_ele;
    char __user *addr;
    char __user *res;
    struct timespec t_start, t_end;
    long t_diff;

    pr_debug("QZ (handle_p2m_remote_aggregate): pid: %u tgid: %u addr: %p n_ele: %zu, res: %p\n",
            payload->pid, payload->tgid, payload->addr, payload->n_ele,
            payload->res);

    t_start = current_kernel_time();
    tsk = find_lego_task_by_pid(hdr->src_nid, payload->tgid);
    if (unlikely(!tsk)) {
        k_res = 0;
        goto out;
    }

    n_ele = payload->n_ele;
    addr = payload->addr;
    res = payload->res;
    unit_size = sizeof(struct lineitem);
    avg_quan = 0;
    for (i = 0; i != n_ele; i++) {
        lego_copy_from_user(tsk, (void *)(&k_data), addr + unit_size*i, unit_size);
        avg_quan += k_data.quantity;
    }
    avg_quan = avg_quan / n_ele;
    k_res = 0;
    for (i = 0; i != n_ele; i++) {
        lego_copy_from_user(tsk, (void *)(&k_data), addr + unit_size*i, unit_size);
         k_res += (k_data.extendedprice * k_data.discount / avg_quan);
    }

    t_end = current_kernel_time();
    t_diff = (t_end.tv_sec - t_start.tv_sec) * 1000000000 + (t_end.tv_nsec - t_start.tv_nsec);
    pr_debug("QZ (handle_p2m_remote_aggregate): kernel executed in %ld ns.\n", t_diff);

    pr_debug("QZ (handle_p2m_remote_aggregate): k_res: %zu\n", k_res);
    // Copy back -- not safe as the memory may not be fresh
    // lego_copy_to_user(tsk, res, &k_res, sizeof(long));

out:
    tb_set_tx_size(tb, sizeof(long));
    *((long *)(tb->tx)) = k_res;
}

void handle_p2m_remote_aggregate(struct p2m_remote_aggregate_payload *payload,
        struct common_header *hdr, struct thpool_buffer *tb) {
    // Execute the aggregation in memory -- inefficient
    execute_in_kernel(payload, hdr, tb);
}

void handle_p2m_c_pushdown_binary(struct p2m_c_pushdown_payload_binary *payload,
        struct common_header *hdr, struct thpool_buffer *tb) {
    struct c_routor_instantiate_info_binary *inst;

    inst = kmalloc(sizeof(*inst), GFP_KERNEL);
    if(!inst)
        goto out;
    inst->src_nid = hdr->src_nid;
    inst->pid = payload->pid;
    inst->payload_size = payload->payload_size;
    inst->filename = payload->filename;
    inst->argc = payload->argc;
    inst->envc = payload->envc;
    inst->array = (const char *)&(payload->array);

    pr_debug("QZ (handle_p2m_c_pushdown): msg_len: %u, pid: %u, filename: %s\n",
        hdr->length, inst->pid, inst->filename);

    // QZ: depercated
    // c_routor_instantiate(inst);

out:
    tb_set_tx_size(tb, sizeof(long));
    *((long *)(tb->tx)) = 1l;
}

void handle_p2m_c_pushdown(struct p2m_c_pushdown_payload *payload,
        struct common_header *hdr, struct thpool_buffer *tb) {
    struct c_routor_instantiate_info *inst;

    inst = kmalloc(sizeof(*inst), GFP_KERNEL);
    if(!inst)
        goto out;
    inst->src_nid = hdr->src_nid;
    inst->pid = payload->pid;
    inst->tgid = payload->tgid;
    inst->parent_tgid = payload->parent_tgid;
    inst->ip = payload->ip;
    inst->sp = payload->sp;
    inst->arg = payload->arg;
    inst->result = 0;

    pr_debug("QZ (handle_p2m_c_pushdown): pid: %u, ip: %#Lx, sp: %#Lx, arg:%#Lx\n",
        inst->pid, inst->ip, inst->sp, inst->arg);

    c_routor_instantiate(inst);

out:
    tb_set_tx_size(tb, sizeof(long));
    *((long *)(tb->tx)) = inst->result;
}

static void syncmem_error(u32 retval, struct lego_task_struct *p,
                  u64 vaddr, struct thpool_buffer *tb)
{
    int *reply = thpool_buffer_tx(tb);

    *reply = retval;
    tb_set_tx_size(tb, sizeof(*reply));

    dump_lego_tasks();
    if (p) {
        pr_info("synmem error - src_nid:%u,pid:%u,vaddr:%#Lx\n", p->node, p->pid, vaddr);
        dump_all_vmas_simple(p->mm);
    }
    WARN_ON_ONCE(1);
}

static void do_handle_p2m_c_syncmem(struct lego_task_struct *p,
        u64 vaddr, u32 flags, u32 offset_head, u32 size, struct thpool_buffer *tb) {
    int ret;
    unsigned long new_page;

    ret = common_handle_p2m_miss(p, vaddr, flags, &new_page);
    if (unlikely(ret & VM_FAULT_ERROR)) {
        if (ret & VM_FAULT_OOM)
            ret = RET_ENOMEM;
        else if (ret & (VM_FAULT_SIGBUS | VM_FAULT_SIGSEGV))
            ret = RET_ESIGSEGV;

        syncmem_error(ret, p, vaddr, tb);
        return;
    }

    tb_set_private_tx(tb, ((void *)new_page + offset_head));
    tb_set_tx_size(tb, size);
}

void handle_p2m_c_syncmem(struct p2m_c_syncmem_m2p_msg *msg,
                        struct thpool_buffer *tb) {
    u32 tgid, flags, offset_head, size;
    u64 vaddr;
    unsigned int src_nid;
    struct lego_task_struct *p;

    src_nid = to_common_header(msg)->src_nid;
    tgid   = msg->tgid;
    flags  = msg->flags;
    vaddr  = msg->missing_vaddr;
    offset_head = msg->offset_head;
    size = msg->size;

    p = find_lego_task_by_pid(src_nid, tgid);
    if (unlikely(!p)) {
        pr_info("%s(): src_nid: %d tgid: %d\n", __func__, src_nid, tgid);
        syncmem_error(RET_ESRCH, p, vaddr, tb);
        return;
    }

    do_handle_p2m_c_syncmem(p, vaddr, flags, offset_head, size, tb);
}
