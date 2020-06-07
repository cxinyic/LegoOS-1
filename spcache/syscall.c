#include <lego/mm.h>
#include <lego/slab.h>
#include <lego/log2.h>
#include <lego/kernel.h>
#include <lego/pgfault.h>
#include <lego/syscalls.h>
#include <lego/memblock.h>

#include <processor/pcache.h>
#include <processor/processor.h>


asmlinkage long sys_sp_pin(unsigned long addr,unsigned long len, unsigned long try_best_flag) {
#ifdef CONFIG_COMP_PROCESSOR

    int ret;
    pr_info("pin_syscall:debug1");
    ret = try_pin(current->mm, addr, len);
    if (try_best_flag==1 && ret == -1){
        ret = 0;
    }
    if (try_best_flag==0 && ret == -1){
        ret = try_unpin(current->mm, addr, len);
    }
    return ret;

#else
    return -1;
#endif
}

asmlinkage long sys_sp_unpin(unsigned long old_addr, unsigned long len){
#ifdef CONFIG_COMP_PROCESSOR

    int ret;
    ret = try_unpin(current->mm, old_addr, len);
    return ret;



#else
    return -1;
#endif
}