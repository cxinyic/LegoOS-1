/*
 * Copyright (c) 2016-2017 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <lego/stat.h>
#include <lego/slab.h>
#include <lego/uaccess.h>
#include <lego/files.h>
#include <lego/spinlock.h>
#include <lego/comp_processor.h>

#include "../internal.h"

extern struct file_operations proc_stat_ops;
extern struct file_operations proc_meminfo_ops;

struct proc_file_struct {
	char f_name[FILENAME_LEN_DEFAULT];
	const struct file_operations *f_op;
};

static struct proc_file_struct proc_files[] = {
	{
		.f_name	= "/proc/stat",
		.f_op = &proc_stat_ops,
	},
	{
		.f_name = "/proc/meminfo",
		.f_op = &proc_meminfo_ops,
	},
};

int proc_file_open(struct file *f, char *f_name)
{
	struct proc_file_struct *proc_file;
	int i, ret;

	ret = -EBADF;
	for (i = 0; i < ARRAY_SIZE(proc_files); i++) {
		proc_file = &proc_files[i];
		if (f_name_equal(f_name, proc_file->f_name)) {
			f->f_op = proc_file->f_op;
			ret = 0;
			break;
		}
	}

	return ret;
}