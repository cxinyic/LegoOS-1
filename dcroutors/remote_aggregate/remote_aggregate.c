#include <lego/kernel.h>
#include <lego/slab.h>
#include <lego/fit_ibapi.h>
#include <lego/uaccess.h>
#include <processor/processor.h>

/*
 * Push aggreggate to memory node.
 * */
asmlinkage long sys_remote_aggregate(char __user *addr, size_t n_ele, char __user *res) {
#ifdef CONFIG_COMP_PROCESSOR
    long retval;
    ssize_t retlen;
    ssize_t *retval_ptr;
    u32 len_retbuf, len_msg;
    void *retbuf, *msg;
    struct common_header *hdr;
    struct p2m_remote_aggregate_payload *payload;
    int mem_node;   /* = pgcache_node if defined or memory homenode */

    len_retbuf = sizeof(long);
    retbuf = kmalloc(len_retbuf, GFP_KERNEL);
    if (!retbuf)
        return -ENOMEM;

    len_msg = sizeof(*hdr) + sizeof(*payload);
    msg = kmalloc(len_msg, GFP_KERNEL);
    if (!msg) {
        kfree(retbuf);
        return -ENOMEM;
    }

    /* Construct payload */
    hdr = msg;
    hdr->opcode = P2M_REMOTE_AGGREGATE;
    hdr->src_nid = LEGO_LOCAL_NID;

    payload = msg + sizeof(*hdr);
    payload->pid = current->pid;
    payload->tgid = current->tgid;
    payload->addr = addr;
    payload->n_ele = n_ele;
    payload->res = res;

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

    copy_to_user(res, &retval, sizeof(long));

out:
    kfree(msg);
    kfree(retbuf);
    return 0;
#else
    return -1;
#endif
}
