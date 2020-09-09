/*
 * Copyright (c) 2016-2018 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/*
 * Virtual memory map management code
 * Based on mm/mmap.c and mm/mremap.c
 */

#include <lego/mm.h>
#include <lego/rwsem.h>
#include <lego/slab.h>
#include <lego/rbtree.h>
#include <lego/sched.h>
#include <lego/kernel.h>
#include <lego/netmacro.h>
#include <lego/fit_ibapi.h>

#include <memory/vm.h>
#include <memory/pid.h>
#include <memory/vm-pgtable.h>
#include <memory/distvm.h>
#include <memory/file_types.h>

#include <memory/dcr_vm.h>

#define LEGO_PGALLOC_GFP     (GFP_KERNEL | __GFP_ZERO)

/* 
 * Recover the page table in mm (virtual to physical)
 * from lego_mm (user virtual to kernel virtual).
 * */
int recover_mm_from_lego_mm(struct mm_struct *dst,
        struct lego_mm_struct *src, struct task_struct *dst_tsk,
        struct lego_task_struct *src_tsk) {
    //
    struct vm_area_struct *mpnt, *tmp, *prev, **pprev;
    struct rb_node **rb_link, *rb_parent;
    int ret = 0;
//    if (down_write_killable(&src->mmap_sem))
//        return -EINTR;
//    down_write(&dst->mmap_sem);
//
//    dst->total_vm = src->total_vm;
//    dst->data_vm = src->data_vm;
//    dst->exec_vm = src->exec_vm;
//    dst->stack_vm = src->stack_vm;

//    rb_link = &dst->mm_rb.rb_node;
//    rb_parent = NULL;
//    pprev = &dst->mmap;

//    prev = NULL;
    for (mpnt = src->mmap; mpnt; mpnt = mpnt->vm_next) {
//        struct lego_file *file;
//
//        tmp = kmalloc(sizeof(*tmp), GFP_KERNEL);
//        if (!tmp) {
//            ret = -ENOMEM;
//            goto out;
//        }
//
//        *tmp = *mpnt;
//        // tmp->vm_mm = dst;
//
//        tmp->vm_flags &=
//            ~(VM_LOCKED|VM_LOCKONFAULT|VM_UFFD_MISSING|VM_UFFD_WP);
//        tmp->vm_next = tmp->vm_prev = NULL;
//
//        file = tmp->vm_file;
//        if (file) {
//            /* Hold 1 more ref is enough now */
//            get_lego_file(file);
//        }
//
//        /*
//         * Link in the new vma and copy the page table entries.
//         */
//        *pprev = tmp;
//        pprev = &tmp->vm_next;
//        tmp->vm_prev = prev;
//        prev = tmp;
//
//        __vma_link_rb_mm(dst, tmp, rb_link, rb_parent); 
//
//        rb_link = &tmp->vm_rb.rb_right;
//        rb_parent = &tmp->vm_rb;
//
//        dst->map_count++;
        ret = copy_mm_from_lego(dst, src, mpnt, dst_tsk);

//        /*
//         * Callback to underlying fs hook if exists:
//         */
//        if (tmp->vm_ops && tmp->vm_ops->open)
//            tmp->vm_ops->open(tmp);
//
        if (ret)
            goto out;
    }

    ret = 0;
out:
//    up_write(&dst->mmap_sem);
//    up_write(&src->mmap_sem);
    return ret;
}
