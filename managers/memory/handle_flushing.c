#include <lego/slab.h>
#include <lego/kernel.h>
#include <lego/uaccess.h>
#include <lego/fit_ibapi.h>
#include <lego/time.h>



#include <memory/vm.h>
#include <memory/pid.h>
#include <memory/thread_pool.h>


#include <asm/tlbflush.h>

void handle_p2m_flush_register(struct p2m_flush_register_payload *payload, 
        struct common_header *hdr, struct thpool_buffer *tb) {
    
    struct lego_task_struct *p;

    p = find_lego_task_by_pid(hdr->src_nid, payload->tgid);
    if (unlikely(!p)) {
		reply = -ESRCH;
		goto out;
	}

    lego_task_lock(p);
    p->regs.r15 = payload->r15;
    p->regs.r14 = payload->r14;
    p->regs.r13 = payload->r13;
    p->regs.r12 = payload->r12;
    p->regs.bp = payload->bp;
    p->regs.bx = payload->bx;
    p->regs.r11 = payload->r11;
    p->regs.r10 = payload->r10;
    p->regs.r9 = payload->r9;
    p->regs.r8 = payload->r8;
    p->regs.ax = payload->ax;
    p->regs.cx = payload->cx;
    p->regs.dx = payload->dx;
    p->regs.si = payload->si;
    p->regs.di = payload->di;
    p->regs.orig_ax = payload->orig_ax;
    p->regs.ip = payload->ip;
    p->regs.cs = payload->cs;
    p->regs.flags = payload->flags;
    p->regs.sp = payload->sp;
    p->regs.ss = payload->ss;
    p->regs.ds = payload->ds;
    p->regs.es = payload->es;
    p->regs.fs = payload->fs;
    p->regs.gs = payload->gs;
    p->version_id = payload->version_id;

    lego_task_unlock(p);
    reply = 0;

out:
    *(long *)thpool_buffer_tx(tb) = reply;
	tb_set_tx_size(tb, sizeof(long));

}
