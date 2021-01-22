#include <lego/mm.h>
#include <lego/syscalls.h>
#include <processor/fs.h>
#include <processor/pgtable.h>
#include <processor/processor.h>
#include <processor/distvm.h>
#include <processor/zerofill.h>
#include <processor/dependency_track.h>




asmlinkage long sys_mmap_track(unsigned long addr, unsigned long len,
        unsigned long prot, unsigned long flags, 
        unsigned long fd, unsigned long off)
{
	
#ifdef CONFIG_COMP_PROCESSOR
	current_pid = current->pid;
	current_tgid = current->tgid;
	current_tsk = current;
#endif
	return 0;

}
asmlinkage long sys_munmap_track(unsigned long addr, size_t len)
{
#ifdef CONFIG_COMP_PROCESSOR
	current_pid = -1;
	current_tgid = -1;
	current_tsk = NULL;
#endif
	return 0;
}