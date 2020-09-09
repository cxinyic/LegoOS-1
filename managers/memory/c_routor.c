#include <lego/slab.h>
#include <lego/sched.h>
#include <lego/signal.h>
#include <lego/kthread.h>
#include <lego/spinlock.h>
#include <lego/completion.h>
#include <lego/mm.h>
#include <lego/ptrace.h>
#include <memory/task.h>
#include <memory/loader.h>
#include <memory/pid.h>
#include <memory/vm.h>

#include <processor/fs.h>
#include <asm/tlbflush.h>

#include <lego/dcroutors/c_routor.h>
#include <memory/dcr_vm.h>

static DEFINE_SPINLOCK(c_routor_instantiate_lock);
static LIST_HEAD(c_routor_instantiate_list);
struct task_struct *c_routor_task;

static void __c_routor_instantiate(struct c_routor_instantiate_info *inst)
{
    DEFINE_COMPLETION(done);

    init_completion(&done);
    inst->done = &done;

    spin_lock(&c_routor_instantiate_lock);
    list_add_tail(&inst->list, &c_routor_instantiate_list);
    spin_unlock(&c_routor_instantiate_lock);

    wake_up_process(c_routor_task);

    /*
     * Wait for completion in killable state, for I might be chosen by
     * the OOM killer while kthreadd is trying to allocate memory for
     * new kernel thread.
     */
    if (unlikely(wait_for_completion_killable(&done))) {
        /*
         * If I was SIGKILLed before kthreadd (or new kernel thread)
         * calls complete(), leave the cleanup of this structure to
         * that thread.
         */
        if (xchg(&inst->done, NULL))
            return;
        /*
         * instantiate_context() will call complete() on done.
         */
        wait_for_completion(&done);
    }
    kfree(inst);
}

/**
 *
 * Description: This helper function instantiates a context to execute
 * compute logic.
 */
void c_routor_instantiate(struct c_routor_instantiate_info *inst)
{
    __c_routor_instantiate(inst);
}

/*
 * This function makes sure the current process has its own signal table,
 * so that flush_signal_handlers can later reset the handlers without
 * disturbing other processes.  (Other processes might share the signal
 * table via the CLONE_SIGHAND option to clone().)
 */
static int de_thread(struct task_struct *tsk)
{
    struct signal_struct *sig = tsk->signal;
    struct sighand_struct *oldsighand = tsk->sighand;
    spinlock_t *lock = &oldsighand->siglock;

    if (thread_group_empty(tsk))
        goto no_thread_group;

    /* Kill all other threads in the thread group */
    spin_lock_irq(lock);
    if (signal_group_exit(sig)) {
        /*
         * Another group action in progress, just
         * return so that the signal is processed.
         */
        spin_unlock_irq(lock);
        return -EAGAIN;
    }

    sig->group_exit_task = tsk;
    sig->notify_count = zap_other_threads(tsk);
    if (!thread_group_leader(tsk))
        sig->notify_count--;

    while (sig->notify_count) {
        __set_current_state(TASK_KILLABLE);
        spin_unlock_irq(lock);
        schedule();
        if (unlikely(__fatal_signal_pending(tsk)))
            goto killed;
        spin_lock_irq(lock);
    }
    spin_unlock_irq(lock);

    /*
     * At this point all other threads have exited, all we have to
     * do is to wait for the thread group leader to become inactive,
     * and to assume its PID:
     */
    if (!thread_group_leader(tsk)) {
        struct task_struct *leader = tsk->group_leader;

        for (;;) {
            spin_lock_irq(&tasklist_lock);
            /*
             * Do this under tasklist_lock to ensure that
             * exit_notify() can't miss ->group_exit_task
             */
            sig->notify_count = -1;
            if (likely(leader->exit_state))
                break;
            __set_current_state(TASK_KILLABLE);
            spin_unlock_irq(&tasklist_lock);
            schedule();
            if (unlikely(__fatal_signal_pending(tsk)))
                goto killed;
        }

        tsk->start_time = leader->start_time;
        tsk->real_start_time = leader->real_start_time;

        BUG_ON(!same_thread_group(leader, tsk));
        BUG_ON(has_group_leader_pid(tsk));

        BUG();
    }

    sig->group_exit_task = NULL;
    sig->notify_count = 0;

no_thread_group:
    /* we have changed execution domain */
    tsk->exit_signal = SIGCHLD;

    exit_itimers(sig);
    flush_itimer_signals();

    if (atomic_read(&oldsighand->count) != 1) {
        struct sighand_struct *newsighand;
        /*
         * This ->sighand is shared with the CLONE_SIGHAND
         * but not CLONE_THREAD task, switch to the new one.
         */
        newsighand = kmalloc(sizeof(*newsighand), GFP_KERNEL);
        if (!newsighand)
            return -ENOMEM;

        atomic_set(&newsighand->count, 1);
        memcpy(newsighand->action, oldsighand->action,
               sizeof(newsighand->action));

        spin_lock_irq(&tasklist_lock);
        spin_lock(&oldsighand->siglock);
        tsk->sighand = newsighand;
        spin_unlock(&oldsighand->siglock);
        spin_unlock_irq(&tasklist_lock);

        __cleanup_sighand(oldsighand);
    }

    BUG_ON(!thread_group_leader(tsk));
    return 0;

killed:
    /* protects against exit_notify() and __exit_signal() */
    spin_lock(&tasklist_lock);
    sig->group_exit_task = NULL;
    sig->notify_count = 0;
    spin_unlock(&tasklist_lock);

    return -EAGAIN;
}

static int exec_mmap(void)
{
    struct mm_struct *new_mm;
    struct mm_struct *old_mm;
    struct task_struct *tsk;

    new_mm = mm_alloc();
    if (!new_mm)
        return -ENOMEM;

    /* Notify parent that we're no longer interested in the old VM */
    tsk = current;
    old_mm = current->mm;
    mm_release(tsk, old_mm);

    /*
     * We should do this before changing mm,
     * because pcache_process_exit() needs old_mm to clean up
     */
    mmput(old_mm);

    task_lock(tsk);
    tsk->mm = new_mm;
    tsk->active_mm = new_mm;
    activate_mm(old_mm, new_mm); // QZ: use the newly initialized new_mm to replace old_mm in the current task
    task_unlock(tsk);

    return 0;
}

static int flush_old_exec_mlocal(void)
{
    int ret;

    /*
     * Make sure we have a private signal table and that
     * we are unassociated from the previous thread group.
     */
    ret = de_thread(current);
    if (ret)
        goto out;

    /*
     * Switch mm, to switch the emulated page-table
     * User program also has its own one
     */
    ret = exec_mmap();
    if (ret)
        goto out;

    /*
     * THIS IS VERY IMPORTANT
     * This thread is no longer a kernel thread. If this flag
     * is not cleared, copy_process_tls() will falsely think
     * this is a kernel thread if later on this guy calls sys_clone():
     */
    current->flags &= ~(PF_KTHREAD | PF_NO_SETAFFINITY);
    flush_thread(); // QZ: clear thread local storage

    /*
     * We have to apply CLOEXEC before we change whether the process is
     * dumpable (in setup_new_exec) to avoid a race with a process in userspace
     * trying to access the should-be-closed file descriptors of a process
     * undergoing exec(2).
     */
    // do_close_on_exec(current->files);

    ret = 0;
out:
    return ret;
}

static void setup_new_exec_mlocal(const char *filename)
{
    /* This is the point of no return */
    current->sas_ss_sp = current->sas_ss_size = 0;

    set_task_comm(current, kbasename(filename));

    /*
     * An exec changes our domain.
     * We are no longer part of the thread group:
     */
    current->self_exec_id++;
    flush_signal_handlers(current, 0);
}


static int t_context_binary (void *_inst)
{
    struct c_routor_instantiate_info_binary *inst = _inst;
    struct completion *done;
    // Below are the arguments to construct an execution
    __u32 argc, envc;
    size_t len;
    unsigned long *argv_len, *envp_len;
    const char **argv, **envp;
    const char *filename, *str;
    __u32 tgid, parent_tgid;
    __u32 pid;
    struct lego_task_struct *tsk, *parent;
    struct lego_mm_struct *mm;

    unsigned int nid = inst->src_nid;
    int i, ret;
    __u64 new_ip, new_sp;
    struct pt_regs *regs = current_pt_regs();
    /*
       __u32 pid = inst->pid; // the process where this temporary context is created
       int (*compute)(void *arg) = inst->compute;
       void *arg = create->arg;
       struct completion *done;
       */

    done = xchg(&inst->done, NULL);

    // Create a temporary context to execute the pushed compute

    // pr_info("t_context(): filename=%s on CPU=%2d\n", inst->filename, smp_processor_id());
    // todo: Load the binary and execute

    pid = inst->pid;
    argc = inst->argc;
    envc = inst->envc;
    filename = inst->filename;

    // pr_info("t_context(): pid:%u,argc:%u,envc:%u,payload_size:%u,file:%s",
    //        pid, argc, envc, inst->payload_size, filename);

    // tsk = find_lego_task_by_pid(inst->src_nid, pid);

    /*
     * fork() first
     * need to construct a lego_task_struct
     * */
    pr_info("t_context(): Setting up fork()...\n");
    tgid = inst->pid + 1; // a new task
    parent_tgid = 1;
    parent = NULL;
    if (!parent && parent_tgid != 1)
        WARN_ONCE(1, "From processor-daemon?");

    tsk = alloc_lego_task_struct();
    if (!tsk) {
        goto out;
    }
    tsk->pid = tgid;
    tsk->parent_pid = parent_tgid;
    tsk->node = nid;
    mem_set_memory_home_node(tsk, LEGO_LOCAL_NID);
    lego_set_task_comm(tsk, inst->filename);

    mm = lego_mm_alloc(tsk, parent);
    if (!mm) {
        free_lego_task_struct(tsk);
        goto out;
    }
    tsk->mm = mm;

    if(ht_insert_lego_task(tsk)) { // Assuming compute node won't make a fork() call
        lego_mmput(tsk->mm);
        free_lego_task_struct(tsk);
        goto out;
    }
    pr_info("t_context(): Task has been set up. Setting up execv...\n");

    /*
     * now execv()
     * */
    argv = kzalloc(sizeof(*argv) * (argc + envc), GFP_KERNEL);
    if (!argv) {
        goto out;
    }

    argv_len = kzalloc(sizeof(*argv_len) * (argc + envc), GFP_KERNEL);
    if (!argv_len) {
        kfree(argv);
        goto out;
    }

    /* Prepare argv and envp */
    str = inst->array;
    for (i = 0; i < (argc + envc); i++) {
        argv[i] = str;

        len = strnlen(str, MAX_ARG_STRLEN);
        len++;  /* terminating NULL */
        str += len;

        /* this array of length including terminal NULL */
        argv_len[i] = len;
    }
    envp = &argv[argc];
    envp_len = &argv_len[argc];

    // QZ: debug args and env -- good
    /*
       for (i = 0; i < argc; i++)
       pr_debug("    argc[%u] (len:%3lu):  %s\n",
       i, argv_len[i], argv[i]);

       for (i = 0; i < envc; i++)
       pr_debug("    envc[%u] (len:%3lu):  %s\n",
       i, envp_len[i], envp[i]);
       */

    pr_info("t_context(): Loading binary to execute...\n");
    ret = exec_loader(tsk, filename, argc, argv, argv_len,
            envc, envp, envp_len,
            &new_ip, &new_sp
#ifdef CONFIG_DISTRIBUTED_VMA_MEMORY
            ,&reply->map
#endif
            );

    if (ret) {
        goto out;
    }

    ret = flush_old_exec_mlocal(); // QZ: context switch
    if (ret) {
        goto out;
    }

    pr_info("t_context(): Running the binary...\n");
    setup_new_exec_mlocal(inst->filename);
#ifdef ELF_PLAT_INIT
    /*
     * The ABI may specify that certain registers be set up in special
     * ways (on i386 %edx is the address of a DT_FINI function, for
     * example.  In addition, it may also specify (eg, PowerPC64 ELF)
     * that the e_entry field is the address of the function descriptor
     * for the startup routine, rather than the address of the startup
     * routine itself.  This macro performs whatever initialization to
     * the regs structure is required as well as any relocations to the
     * function descriptor entries when executing dynamically links apps.
     */
    ELF_PLAT_INIT(regs);
#endif
    start_thread(regs, new_ip, new_sp);
    ret = 0;

    pr_info("t_context(): User thread should have been started. Exiting...\n");
out:
    complete(done);
    return 0;
}

// QZ: for debug in copy from user
struct add_args {
    int a;
    int b;
    int res;
};

/*
 * Copy a lego vm to executable vm
 * */
static void from_lego_mm_to_mm(struct lego_mm_struct *src, struct mm_struct *dst,
        struct lego_task_struct *src_task, struct task_struct *dst_task)
{
    dst->task_size = src->task_size;
    dst->highest_vm_end = src->highest_vm_end;
    // dst->mm_users = src->mm_users;
    dst->map_count = src->map_count;
    dst->total_vm = src->total_vm;
    dst->data_vm = src->data_vm;
    dst->exec_vm = src->exec_vm;
    dst->stack_vm = src->stack_vm;
    dst->def_flags = src->def_flags;
    dst->start_code = src->start_code;
    dst->end_code = src->end_code;
    dst->start_data = src->start_data;
    dst->end_data = src->end_data;
    dst->start_brk = src->start_brk;
    dst->brk = src->brk;
    dst->start_stack = src->start_stack;
    dst->arg_start = src->arg_start;
    dst->arg_end = src->arg_end;
    dst->env_start = src->env_start;
    dst->env_end = src->env_end;
    // dst->mmap_base = src->mmap_base;
    // dst->mmap_cache = src->mmap;
    // dst->mm_rb = src->mm_rb;

    recover_mm_from_lego_mm(dst, src, dst_task, src_task);
}

/*
 * Specification of dcr compute
 * */
typedef long (*dcr_compute)(unsigned long);
typedef unsigned long dcr_arg;

static int t_context(void *_inst)
{
    struct c_routor_instantiate_info *inst = _inst;
    unsigned int nid = inst->src_nid;
    unsigned int pid = inst->pid;
    struct lego_task_struct *lego_tsk;
    struct lego_mm_struct *lego_mm;
    struct task_struct *tsk = current;
    struct mm_struct *old_mm, *new_mm;
    unsigned long new_ip=inst->ip;
    struct completion *done;
    dcr_compute compute;
    dcr_arg arg;
    // unsigned int tgid = inst->tgid;
    // void* vmainfo;
    // struct pt_regs *regs = current_pt_regs();
    // unsigned long new_ip=0x7ffff7ddb260, new_sp=0x7fffffffed90;
    // unsigned long new_sp=inst->sp;
    // struct add_args test_mem_mm, test_mem_lego;

    /*
    test_mem_mm.a = -1;
    test_mem_mm.b = -1;
    test_mem_mm.res = -1;
    
    test_mem_lego.a = 1;
    test_mem_lego.b = 2;
    test_mem_lego.res = 3;
    */

    done = xchg(&inst->done, NULL);

    // Fork args here
    // pr_debug("QZ (t_context): pid: %u, ip: %#Lx, sp: %#Lx, arg:%#Lx\n",
    //        inst->pid, inst->ip, inst->sp, inst->arg);

    lego_tsk = find_lego_task_by_pid(nid, pid);
    lego_mm = lego_tsk->mm;
    
    // Emualte flush_old_exec()
    // de_thread(tsk);
    // pr_debug("QZ (t_context): after de_thread\n");
    
    // -- Emulate exec_mmap()

    // We need to translate parent->mm (lego) to current->mm (linux)
    old_mm = tsk->mm;
    // Allocate a new struct mm
    new_mm = mm_alloc();
    if (!new_mm)
        return -ENOMEM;
    // pr_debug("QZ (t_context): after mm_alloc\n");
    
    // Convert lego_mm to mm
    from_lego_mm_to_mm(lego_mm, new_mm, lego_tsk, tsk);
    // pr_debug("QZ (t_context): after from_legomm_to_mm\n");

    // Switch VM
    mm_release(tsk, old_mm);
    mmput(old_mm);
    task_lock(tsk);
    tsk->mm = new_mm;
    tsk->active_mm = new_mm;
    activate_mm(old_mm, new_mm);
    task_unlock(tsk);
    
    // QZ: should call tlb_flush to disable old page table.
    flush_tlb();
	
    /*
    pr_debug("****    Dump new mm:\n");
	dump_all_vmas_simple(lego_mm);
	dump_lego_mm(lego_mm);
	pr_debug("****    Finish dump new mm\n");
    */
    // pr_debug("QZ (t_context): after context switch\n");

    /*
    lego_copy_to_user(lego_tsk, (const void __user *)(inst->arg), &test_mem_lego, sizeof(test_mem_lego));
    test_mem_lego.a = -1;
    test_mem_lego.b = -1;
    test_mem_lego.res = -1;
    // Test page table with copy_from_user
    copy_from_user(&test_mem_mm, (const void __user *)(inst->arg), sizeof(test_mem_mm));
    lego_copy_from_user(lego_tsk, &test_mem_lego, (const void __user *)(inst->arg), sizeof(test_mem_lego));
    pr_debug("QZ (t_context) memory test (data): mm (a=%d, b=%d, res=%d), lego (a=%d, b=%d, res=%d)\n",
            test_mem_mm.a, test_mem_mm.b, test_mem_mm.res, test_mem_lego.a, test_mem_lego.b, test_mem_lego.res);

    pr_debug("QZ (t_context) memory test (code): converting %#lx to function...\n", new_ip);
    test_mem_compute = (dcr_compute)new_ip;
    test_mem_compute();
    copy_from_user(&test_mem_mm, (const void __user *)(inst->arg), sizeof(test_mem_mm));
    pr_debug("QZ (t_context) memory test (code): mm (a=%d, b=%d, res=%d)\n",
            test_mem_mm.a, test_mem_mm.b, test_mem_mm.res);
    */
    // pr_debug("QZ (t_context): executing f(arg)...\n");

    compute = (dcr_compute)new_ip;
    arg = inst->arg;
    inst->result = compute(arg);

    // pr_debug("QZ (t_context): f(arg) is done with return value %ld, complete!\n", inst->result);
    complete(done);

    /*
    tsk->flags &= ~(PF_KTHREAD | PF_NO_SETAFFINITY);
    flush_thread(); // QZ: clear thread local storage
    
    pr_debug("QZ (t_context): after flush_thread\n");
    
    // Emulate setup_new_exec()

    tsk->sas_ss_sp = tsk->sas_ss_size = 0;

    set_task_comm(tsk, kbasename("dcr_t_context"));

    tsk->self_exec_id++;
    flush_signal_handlers(tsk, 0);
    
    pr_debug("QZ (t_context): after flush_signal_handlers\n");

#ifdef ELF_PLAT_INIT
    ELF_PLAT_INIT(regs);
    pr_debug("QZ (t_context): after ELF_PLAT_INIT\n");
#endif
    
    new_sp = new_mm->start_stack;
    start_thread(regs, new_ip, new_sp);
    // ret = 0;
    
    pr_info("t_context(): Exiting kernel thread. User code will start immediately after return.\n");
    */

    // Shouldn't complete here, as this kernel thread will exit to run user code.
    // complete(done);

    preempt_disable();
    do_task_dead();

    BUG(); // We don't want this context to return and jump to iret
    /*
     * This return will return to the point where t_context()
     * is invoked. The final return to user-space will happen
     * when this kernel thread finishes and merges into
     * the end of ret_from_fork().
     *
     * Check ret_from_fork() for more detail.
     */
    return 0;
}

/*
 * QZ: we instantiate the context here to execute the pushed compute.
 * Possible solutions:
 * 1. We switch current task to the targeted task VM, and execute.
 * 2. We create a new task which shares the targeted task VM, and execute.
 * 04/22/2020, Let's use 2 for now.
 * */
static void instantiate_context(struct c_routor_instantiate_info *inst)
{
    // struct task_struct* c;
    // unsigned long clone_flags;
    struct completion *done;
    pid_t pid;

    // clone_flags = CLONE_GLOBAL_THREAD;

    // pr_info("c_routor: going to spawn a kernel thread for t_context()...\n");
    // c = kthread_run_flags(t_context, inst, clone_flags, "t_context_%s", inst->filename);
    // c = kthread_run(t_context, inst, "t_context");
    pid = kernel_thread(t_context, inst, 0);
    // if (IS_ERR(c)) {
    if (pid < 0) {
        /* If user was SIGKILLed, I release the structure. */
        done = xchg(&inst->done, NULL);

        if (!done) {
            kfree(inst);
            return;
        }
        complete(done);
    }
    // pr_info("c_routor: after spawning t_context(). Wait for completion\n");
    // done = xchg(&inst->done, NULL);
    // wait_for_completion(done); 
    // pr_info("c_routor: t_context() completes.\n");
}

int c_routor(void *unused)
{
    struct task_struct *tsk = current;

    /* Setup a clean context for our children to inherit. */
    set_task_comm(tsk, "c_routor");

    /*
     * We don't want to be interrupted by any signals
     * Especially if some children exit while we are during copy_process(),
     * children will send signals to us.
     */
    ignore_signals(tsk);

    set_cpus_allowed_ptr(tsk, cpu_possible_mask);

    pr_info("%s(pid:%d/cpu:%d) is running as daemon\n",
            current->comm, current->pid, smp_processor_id());

    for (;;) {
        set_current_state(TASK_INTERRUPTIBLE);
        if (list_empty(&c_routor_instantiate_list))
            schedule();
        __set_current_state(TASK_RUNNING);

        spin_lock(&c_routor_instantiate_lock);
        while (!list_empty(&c_routor_instantiate_list)) {
            struct c_routor_instantiate_info *inst;

            inst = list_entry(c_routor_instantiate_list.next,
                    struct c_routor_instantiate_info, list);
            list_del_init(&inst->list);
            spin_unlock(&c_routor_instantiate_lock);

            // pr_info("c_routor: processing a compute...\n");
            instantiate_context(inst);

            spin_lock(&c_routor_instantiate_lock);
        }
        spin_unlock(&c_routor_instantiate_lock);
    }

    return 0;
}
