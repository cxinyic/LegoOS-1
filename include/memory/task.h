/*
 * Copyright (c) 2016-2018 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef _LEGO_MEMORY_TASK_H_
#define _LEGO_MEMORY_TASK_H_

#include <lego/kernel.h>
#include <lego/spinlock.h>
#include <lego/hashtable.h>
#include <lego/comp_common.h>

#include <memory/mm.h>

struct _task_padding {
	char x[0];
} ____cacheline_aligned;
#define LEGO_TASK_PADDING(name)	struct _task_padding name;

struct comp_regs{
	unsigned long r15;
	unsigned long r14;
	unsigned long r13;
	unsigned long r12;
	unsigned long bp;
	unsigned long bx;
	unsigned long r11;
	unsigned long r10;
	unsigned long r9;
	unsigned long r8;
	unsigned long ax;
	unsigned long cx;
	unsigned long dx;
	unsigned long si;
	unsigned long di;
	unsigned long orig_ax;
	unsigned long ip;
	unsigned long cs;
	unsigned long flags;
	unsigned long sp;
	unsigned long ss;
	unsigned long fs_base;
	unsigned long gs_base;

	unsigned short ds;
	unsigned short es;
	unsigned short fs;
	unsigned short gs;

};

struct lego_task_struct {
	unsigned long gpid;

	struct lego_mm_struct *mm;

	unsigned int node;
	unsigned int pid;		/* User-level pid, or kernel-level tgid */
	unsigned int vnode_id;		/* Unique user vnode id */
	unsigned int parent_pid;

	int home_node;			/* process home node id */

	char comm[LEGO_TASK_COMM_LEN];	/* executable name excluding path
					 * - access with [gs]et_task_comm (which lock
					 *   it with task_lock())
					 * - initialized normally by setup_new_exec
					 */

	LEGO_TASK_PADDING(_pad1_)
	spinlock_t task_lock;

        struct hlist_node link;
	struct comp_regs regs;  /* the registers value in compute node */
	unsigned long version_id; /* the version id in flushing */
	
} ____cacheline_aligned;

void dump_lego_tasks(void);
struct lego_task_struct *alloc_lego_task_struct(void);
void free_lego_task_struct(struct lego_task_struct *tsk);

static inline void lego_task_lock(struct lego_task_struct *p)
{
	spin_lock(&p->task_lock);
}

static inline void lego_task_unlock(struct lego_task_struct *p)
{
	spin_unlock(&p->task_lock);
}

static inline void lego_set_task_comm(struct lego_task_struct *tsk,
				      const char *buf)
{
	lego_task_lock(tsk);
	strlcpy(tsk->comm, buf, sizeof(tsk->comm));
	lego_task_unlock(tsk);
}

static inline bool is_homenode(struct lego_task_struct *p)
{
	if (p->home_node == LEGO_LOCAL_NID)
		return true;
	return false;
}

static inline int mem_get_memory_home_node(struct lego_task_struct *p)
{
	return p->home_node;
}

static inline void mem_set_memory_home_node(struct lego_task_struct *p, int new)
{
	p->home_node = new;
}

#endif /* _LEGO_MEMORY_TASK_H_ */
