#ifndef _LEGO_C_ROUTOR_H_
#define _LEGO_C_ROUTOR_H_

#include <lego/err.h>
#include <lego/sched.h>

struct c_routor_instantiate_info_binary {
    /* Information passed to t_context()---temporary context, thread, I suppose--- from c_routor. */
    /*
       pid_t   pid;
       int			(*compute)(void *arg);
       void			*arg;
       int			node;
       */

    /* Result passed back to c_routor_instantiate() from c_routor. */
    /*
       struct task_struct	*result;
       struct completion	*done;

       struct list_head	list;
       */

    // For binary execution
    __u32   src_nid;
    __u32   pid;
    __u32   payload_size;
    const char    *filename;
    __u32   argc;
    __u32   envc;
    const char    *array;

    struct completion   *done;
    struct list_head    list;
};

struct c_routor_instantiate_info {
    __u32   src_nid;
    __u32   pid;
    __u32   tgid;
    __u32   parent_tgid;
    __u64   ip;
    __u64   sp;
    __u64   arg;
    long     result;

    struct completion   *done;
    struct list_head    list;
};

int c_routor(void *unused);
extern struct task_struct *c_routor_task;

void c_routor_instantiate(struct c_routor_instantiate_info *inst);


#endif /* _LEGO_C_ROUTOR_H_ */
