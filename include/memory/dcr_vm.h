#ifndef _DCR_MEMORY_VM_H_
#define _DCR_MEMORY_VM_H_

int recover_mm_from_lego_mm(struct mm_struct *dst,
        struct lego_mm_struct *src, struct task_struct *dst_tsk,
        struct lego_task_struct *src_tsk);

#endif
