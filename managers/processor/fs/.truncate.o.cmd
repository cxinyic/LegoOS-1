cmd_managers/processor/fs/truncate.o := gcc -Wp,-MD,managers/processor/fs/.truncate.o.d  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/4.8/include -I./arch/x86/include -I./arch/x86/include/generated -I./arch/x86/include/generated/uapi  -I./include -I./managers -I./managers -include ./include/lego/kconfig.h -D__KERNEL__ -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fno-pie -Werror-implicit-function-declaration -Wno-format-security -Werror=uninitialized -Werror=maybe-uninitialized -std=gnu89 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -m64 -falign-jumps=1 -falign-loops=1 -mno-80387 -mno-fp-ret-in-387 -mpreferred-stack-boundary=3 -mno-red-zone -mcmodel=kernel -funit-at-a-time -maccumulate-outgoing-args -pipe -Wno-sign-compare -fno-asynchronous-unwind-tables -fno-delete-null-pointer-checks -Wno-maybe-uninitialized -O2 --param=allow-store-data-races=0 -Wno-unused-but-set-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -g -fno-var-tracking-assignments -fno-stack-protector -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -Werror=implicit-int -Werror=strict-prototypes -DCC_HAVE_ASM_GOTO -c -o managers/processor/fs/truncate.o managers/processor/fs/truncate.c

source_managers/processor/fs/truncate.o := managers/processor/fs/truncate.c

deps_managers/processor/fs/truncate.o := \
  include/lego/stat.h \
  arch/x86/include/asm/stat.h \
  include/lego/compiler.h \
    $(wildcard include/config/smp.h) \
  arch/x86/include/asm/cache.h \
  include/lego/types.h \
    $(wildcard include/config/64bit.h) \
  arch/x86/include/asm/types.h \
    $(wildcard include/config/x86/64.h) \
  include/lego/stddef.h \
  include/lego/linkage.h \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/x86/32.h) \
    $(wildcard include/config/x86/alignment/16.h) \
  include/lego/stringify.h \
  include/lego/time.h \
  include/lego/math64.h \
  arch/x86/include/asm/div64.h \
  include/lego/string.h \
  arch/x86/include/asm/string.h \
  arch/x86/include/asm/string_64.h \
  include/lego/slab.h \
    $(wildcard include/config/slab.h) \
    $(wildcard include/config/slub.h) \
    $(wildcard include/config/slob.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/debug/kmalloc/use/buddy.h) \
  include/lego/mm.h \
    $(wildcard include/config/sparsemem.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
    $(wildcard include/config/flatmem.h) \
  arch/x86/include/asm/page.h \
  arch/x86/include/asm/page_types.h \
    $(wildcard include/config/physical/start.h) \
  include/lego/const.h \
  arch/x86/include/asm/sparsemem.h \
    $(wildcard include/config/x86/pae.h) \
  arch/x86/include/asm/pgtable.h \
  arch/x86/include/asm/pgtable_types.h \
    $(wildcard include/config/x86/intel/memory/protection/keys.h) \
    $(wildcard include/config/mem/soft/dirty.h) \
  arch/x86/include/asm/asm.h \
  arch/x86/include/asm/cmpxchg.h \
  include/lego/bug.h \
  include/lego/panic.h \
  include/lego/printk.h \
  /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdarg.h \
  include/lego/spinlock.h \
    $(wildcard include/config/debug/spinlock.h) \
  include/lego/irq.h \
  arch/x86/include/asm/irq.h \
  arch/x86/include/asm/processor-flags.h \
  arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/x86/local/apic.h) \
    $(wildcard include/config/x86/io/apic.h) \
    $(wildcard include/config/pci/msi.h) \
  arch/x86/include/asm/apic_types.h \
  include/lego/typecheck.h \
  include/lego/preempt.h \
    $(wildcard include/config/preempt.h) \
  include/lego/percpu.h \
    $(wildcard include/config/have/setup/per/cpu/area.h) \
    $(wildcard include/config/debug/preempt.h) \
  include/lego/cpumask.h \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/hotplug/cpu.h) \
  include/lego/numa.h \
    $(wildcard include/config/nodes/shift.h) \
  include/lego/bitops.h \
  arch/x86/include/asm/bitops.h \
    $(wildcard include/config/x86/cmov.h) \
  include/lego/hweight.h \
  include/lego/bitmap.h \
  include/lego/kernel.h \
  include/lego/err.h \
  include/lego/errno.h \
  include/lego/log2.h \
  include/lego/cputime.h \
  include/lego/sections.h \
  include/lego/byteorder.h \
  arch/x86/include/asm/byteorder.h \
  include/lego/swab.h \
  include/uapi/limits.h \
  arch/x86/include/asm/percpu.h \
    $(wildcard include/config/x86/64/smp.h) \
    $(wildcard include/config/x86/cmpxchg64.h) \
  arch/x86/include/asm/spinlock.h \
  arch/x86/include/asm/barrier.h \
  arch/x86/include/asm/alternative.h \
  arch/x86/include/asm/barrier-generic.h \
  arch/x86/include/asm/pgalloc.h \
  arch/x86/include/asm/segment.h \
  arch/x86/include/asm/processor.h \
  arch/x86/include/asm/tss.h \
  arch/x86/include/asm/smp.h \
  arch/x86/include/asm/desc.h \
  arch/x86/include/asm/ptrace.h \
  arch/x86/include/asm/uaccess.h \
  include/lego/extable.h \
  arch/x86/include/asm/extable.h \
  arch/x86/include/asm/current.h \
  arch/x86/include/asm/uaccess_64.h \
  arch/x86/include/asm/processor-features.h \
    $(wildcard include/config/x86/feature/names.h) \
  arch/x86/include/asm/processor-features-flags.h \
    $(wildcard include/config/page/table/isolation.h) \
  arch/x86/include/asm/fpu/types.h \
  arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/vm86.h) \
    $(wildcard include/config/compat.h) \
    $(wildcard include/config/ia32/emulation.h) \
  include/lego/pfn.h \
  include/lego/atomic.h \
  arch/x86/include/asm/atomic.h \
  arch/x86/include/asm/atomic64.h \
  arch/x86/include/asm/rmwcc.h \
  arch/x86/include/asm/atomic-long.h \
  include/lego/nodemask.h \
  include/lego/page-flags-layout.h \
  include/generated/bounds.h \
  include/lego/mm_zone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/zone/dma32.h) \
    $(wildcard include/config/sparsemem/extreme.h) \
  include/lego/list.h \
    $(wildcard include/config/illegal/pointer/value.h) \
  include/lego/mm_types.h \
    $(wildcard include/config/comp/processor.h) \
    $(wildcard include/config/processor/split/ptlock/cpus.h) \
    $(wildcard include/config/processor/enable/split/pmd/ptlock.h) \
    $(wildcard include/config/distributed/vma/processor.h) \
  include/lego/rwsem.h \
    $(wildcard include/config/debug/lock/alloc.h) \
    $(wildcard include/config/rwsem/spin/on/owner.h) \
  arch/x86/include/asm/rwsem.h \
  include/lego/rbtree.h \
  include/lego/distvm.h \
    $(wildcard include/config/debug/vma.h) \
    $(wildcard include/config/debug/vma/trace.h) \
    $(wildcard include/config/fit/nr/nodes.h) \
    $(wildcard include/config/vm/granularity/order.h) \
  include/lego/mm_debug.h \
  include/lego/tracepoint.h \
  include/lego/mmap.h \
  include/lego/memory_model.h \
    $(wildcard include/config/discontigmem.h) \
  include/lego/page-flags.h \
  include/lego/gfp.h \
    $(wildcard include/config/highmem.h) \
  include/lego/uaccess.h \
    $(wildcard include/config/preempt/count.h) \
  include/lego/sched.h \
    $(wildcard include/config/schedstats.h) \
    $(wildcard include/config/futex.h) \
  include/lego/cred.h \
  include/lego/files.h \
    $(wildcard include/config/epoll.h) \
    $(wildcard include/config/mem/page/cache.h) \
  include/lego/mutex.h \
    $(wildcard include/config/debug/mutexes.h) \
    $(wildcard include/config/mutex/spin/on/owner.h) \
  arch/x86/include/asm/mutex.h \
  include/lego/fcntl.h \
  include/lego/llist.h \
    $(wildcard include/config/arch/have/nmi/safe/cmpxchg.h) \
  include/lego/magic.h \
  include/lego/signal.h \
  include/lego/wait.h \
  include/lego/rlimit.h \
  include/lego/siginfo.h \
  include/lego/ktime.h \
  include/lego/timer.h \
  arch/x86/include/asm/signal.h \
  include/lego/strace.h \
    $(wildcard include/config/strace.h) \
  include/lego/sched_prio.h \
  arch/x86/include/asm/tsc.h \
  arch/x86/include/asm/ucontext.h \
  arch/x86/include/asm/sigcontext.h \
  arch/x86/include/asm/switch_to.h \
  include/processor/processor_types.h \
    $(wildcard include/config/checkpoint.h) \
    $(wildcard include/config/gsm.h) \
  include/processor/vnode_types.h \
  include/lego/syscalls.h \
    $(wildcard include/config/debug/syscall.h) \
  include/lego/ptrace.h \
  include/lego/getcpu.h \
  include/lego/socket.h \
  arch/x86/include/asm/syscalls.h \
  include/generated/unistd_64.h \
  include/processor/fs.h \
  include/processor/processor.h \
    $(wildcard include/config/profiling/boot/rpc.h) \
  include/lego/comp_common.h \
    $(wildcard include/config/default/mem/node.h) \
    $(wildcard include/config/default/storage/node.h) \
    $(wildcard include/config/soft/watchdog.h) \
  include/lego/rpc/opcode.h \
  include/lego/rpc/struct_common.h \
  include/lego/rpc/struct_p2m.h \
    $(wildcard include/config/distributed/vma.h) \
  include/memory/replica_types.h \
  include/processor/pcache_config.h \
    $(wildcard include/config/h/.h) \
    $(wildcard include/config/pcache/line/size/shift.h) \
    $(wildcard include/config/pcache/associativity/shift.h) \
  include/lego/rpc/struct_p2s.h \
  include/processor/statfs.h \
  include/lego/rpc/struct_m2m.h \
    $(wildcard include/config/distributed/vma/memory.h) \
  include/lego/rpc/struct_m2s.h \
  include/processor/node.h \
  include/lego/fit_ibapi.h \
    $(wildcard include/config/fit/local/id.h) \
    $(wildcard include/config/fit.h) \
    $(wildcard include/config/counter/fit/ib.h) \
    $(wildcard include/config/comp/memory.h) \
    $(wildcard include/config/socket/o/ib.h) \
  include/net/arch/cc.h \
  include/uapi/fit.h \
    $(wildcard include/config/fit/max/rpc/timeout/sec.h) \

managers/processor/fs/truncate.o: $(deps_managers/processor/fs/truncate.o)

$(deps_managers/processor/fs/truncate.o):
