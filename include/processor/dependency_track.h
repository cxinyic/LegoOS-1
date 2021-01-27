#ifndef _LEGO_PROCESSOR_DEPENDENCY_TRACK_H
#define _LEGO_PROCESSOR_DEPENDENCY_TRACK_H

#include <lego/types.h>
#include <processor/dependency_vector.h>
#include <lego/comp_common.h>
#include <lego/checkpoint.h>

/*
 * dp_info struct: 
 * for every mmap_track syscall, one of this struct will be created.
 *
 * user_pid: pid of the process
 * addr: begin address 
 * len: length
 * nr_pages: number of pages
 * pcm_list: array of the pcms of the pages
 * dirty_page_list: record whether the page is dirty.
 * dp_pages: one vector for every page: storing the pages that it is dependent on
 */

struct dp_info{
    pid_t user_pid;
    struct mm_struct * user_mm;
    unsigned long addr;
    unsigned long len;
    unsigned long nr_pages;
    struct pcache_meta ** pcm_list;
    int * dirty_page_list;
    struct dp_vector* dp_pages;
};

/*
 * list_idx: index in dp_info_list
 * addr_idx: index in dp_info
 */

struct dp_idx{
    int list_idx;
    int addr_idx;
}; 

struct restorer_work_info {
	struct process_snapshot	*pss;
	struct task_struct	*result;
	struct completion	*done;

	struct list_head	list;
};

/*
 * nr_dp_info: number of dp_info
 * dp_info_list: store those dp_info
 */

extern int nr_dp_info;
extern spinlock_t dp_spinlock;
extern pid_t current_pid;
extern pid_t current_tgid;
extern struct pcache_meta * dirty_pcm_last_period;
extern struct pt_regs * current_registers;
extern struct task_struct * current_tsk;
extern unsigned long fs_base_value;
extern unsigned long gs_base_value;
extern unsigned short ds_value;
extern unsigned short es_value;
extern unsigned short gs_value;
extern unsigned short fs_value;
extern struct restorer_work_info current_info;

int deptrack_restore_files(struct process_snapshot *pss);
void deptrack_restore_signals(struct process_snapshot *pss);




#ifdef CONFIG_DEPENDENCY_TRACK

void dependency_track_init(void);

#else

static inline void dependency_track_init(void) {}

#endif

#endif /* _LEGO_PROCESSOR_DEPENDENCY_TRACK_H */