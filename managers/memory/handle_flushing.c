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
    p->regs.r15 = payload->common_registers.r15;
    p->regs.r14 = payload->common_registers.r14;
    p->regs.r13 = payload->common_registers.r13;
    p->regs.r12 = payload->common_registers.r12;
    p->regs.bp = payload->common_registers.bp;
    p->regs.bx = payload->common_registers.bx;
    p->regs.r11 = payload->common_registers.r11;
    p->regs.r10 = payload->common_registers.r10;
    p->regs.r9 = payload->common_registers.r9;
    p->regs.r8 = payload->common_registers.r8;
    p->regs.ax = payload->common_registers.ax;
    p->regs.cx = payload->common_registers.cx;
    p->regs.dx = payload->common_registers.dx;
    p->regs.si = payload->common_registers.si;
    p->regs.di = payload->common_registers.di;
    p->regs.orig_ax = payload->common_registers.orig_ax;
    p->regs.ip = payload->common_registers.ip;
    p->regs.cs = payload->common_registers.cs;
    p->regs.flags = payload->common_registers.flags;
    p->regs.sp = payload->common_registers.sp;
    p->regs.ss = payload->common_registers.ss;
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
