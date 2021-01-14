#ifndef _LEGO_PROCESSOR_DEPENDENCY_TRACK_H
#define _LEGO_PROCESSOR_DEPENDENCY_TRACK_H

#include <lego/types.h>
#include <processor/dependency_vector.h>

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

/*
 * nr_dp_info: number of dp_info
 * dp_info_list: store those dp_info
 */

extern int nr_dp_info;
extern struct dp_vector* dp_info_list;
extern struct dp_vector* old_dirty_pages;
extern struct dp_vector* new_dirty_pages;
extern spinlock_t dp_spinlock;
extern pid_t current_pid;
extern struct pcache_meta * dirty_pcm_last_period;



#ifdef CONFIG_DEPENDENCY_TRACK

void dependency_track_init(void);

#else

static inline void dependency_track_init(void) {}

#endif

#endif /* _LEGO_PROCESSOR_DEPENDENCY_TRACK_H */