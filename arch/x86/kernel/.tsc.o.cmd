cmd_arch/x86/kernel/tsc.o := gcc -Wp,-MD,arch/x86/kernel/.tsc.o.d  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/4.8/include -I./arch/x86/include -I./arch/x86/include/generated -I./arch/x86/include/generated/uapi  -I./include -I./managers -I./managers -include ./include/lego/kconfig.h -D__KERNEL__ -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fno-pie -Werror-implicit-function-declaration -Wno-format-security -Werror=uninitialized -Werror=maybe-uninitialized -std=gnu89 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -m64 -falign-jumps=1 -falign-loops=1 -mno-80387 -mno-fp-ret-in-387 -mpreferred-stack-boundary=3 -mno-red-zone -mcmodel=kernel -funit-at-a-time -maccumulate-outgoing-args -pipe -Wno-sign-compare -fno-asynchronous-unwind-tables -fno-delete-null-pointer-checks -Wno-maybe-uninitialized -O2 --param=allow-store-data-races=0 -Wno-unused-but-set-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -g -fno-var-tracking-assignments -fno-stack-protector -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -Werror=implicit-int -Werror=strict-prototypes -DCC_HAVE_ASM_GOTO -c -o arch/x86/kernel/tsc.o arch/x86/kernel/tsc.c

source_arch/x86/kernel/tsc.o := arch/x86/kernel/tsc.c

deps_arch/x86/kernel/tsc.o := \
  include/lego/irq.h \
  arch/x86/include/asm/irq.h \
  arch/x86/include/asm/processor-flags.h \
  include/lego/const.h \
  arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/x86/local/apic.h) \
    $(wildcard include/config/x86/io/apic.h) \
    $(wildcard include/config/pci/msi.h) \
  arch/x86/include/asm/apic_types.h \
    $(wildcard include/config/x86/32.h) \
  include/lego/types.h \
    $(wildcard include/config/64bit.h) \
  arch/x86/include/asm/types.h \
    $(wildcard include/config/x86/64.h) \
  include/lego/compiler.h \
    $(wildcard include/config/smp.h) \
  arch/x86/include/asm/cache.h \
  include/lego/linkage.h \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/x86/alignment/16.h) \
  include/lego/stringify.h \
  include/lego/stddef.h \
  include/lego/typecheck.h \
  include/lego/delay.h \
  arch/x86/include/asm/delay.h \
  include/lego/kernel.h \
  /usr/lib/gcc/x86_64-linux-gnu/4.8/include/stdarg.h \
  include/lego/err.h \
  include/lego/errno.h \
  include/lego/log2.h \
  include/lego/bitops.h \
  arch/x86/include/asm/bitops.h \
    $(wildcard include/config/x86/cmov.h) \
  arch/x86/include/asm/asm.h \
  include/lego/hweight.h \
  include/lego/bug.h \
  include/lego/panic.h \
  include/lego/printk.h \
  include/lego/math64.h \
  arch/x86/include/asm/div64.h \
  include/lego/cputime.h \
  include/lego/sections.h \
  include/lego/byteorder.h \
  arch/x86/include/asm/byteorder.h \
  include/lego/swab.h \
  include/uapi/limits.h \
  include/lego/jiffies.h \
    $(wildcard include/config/hz.h) \
  include/lego/time.h \
  include/lego/string.h \
  arch/x86/include/asm/string.h \
  arch/x86/include/asm/string_64.h \
  include/lego/clocksource.h \
  arch/x86/include/asm/io.h \
  include/lego/resource.h \
  arch/x86/include/asm/tsc.h \
  arch/x86/include/asm/msr.h \
    $(wildcard include/config/tdp/nominal.h) \
    $(wildcard include/config/tdp/level/1.h) \
    $(wildcard include/config/tdp/level/2.h) \
    $(wildcard include/config/tdp/control.h) \
  arch/x86/include/asm/extable.h \
  arch/x86/include/asm/barrier.h \
  arch/x86/include/asm/cmpxchg.h \
  arch/x86/include/asm/alternative.h \
  arch/x86/include/asm/barrier-generic.h \
  arch/x86/include/asm/processor-features-flags.h \
    $(wildcard include/config/page/table/isolation.h) \
  arch/x86/include/asm/hpet.h \
    $(wildcard include/config/hpet/timer.h) \
  arch/x86/include/asm/timex.h \
  arch/x86/include/asm/processor.h \
  arch/x86/include/asm/tss.h \
  include/lego/percpu.h \
    $(wildcard include/config/have/setup/per/cpu/area.h) \
    $(wildcard include/config/debug/preempt.h) \
  include/lego/cpumask.h \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/hotplug/cpu.h) \
  include/lego/numa.h \
    $(wildcard include/config/nodes/shift.h) \
  include/lego/bitmap.h \
  arch/x86/include/asm/percpu.h \
    $(wildcard include/config/x86/64/smp.h) \
    $(wildcard include/config/x86/cmpxchg64.h) \
  arch/x86/include/asm/smp.h \
    $(wildcard include/config/numa.h) \
  arch/x86/include/asm/desc.h \
  arch/x86/include/asm/page.h \
  arch/x86/include/asm/page_types.h \
    $(wildcard include/config/physical/start.h) \
  arch/x86/include/asm/sparsemem.h \
    $(wildcard include/config/sparsemem.h) \
    $(wildcard include/config/x86/pae.h) \
  arch/x86/include/asm/segment.h \
  arch/x86/include/asm/ptrace.h \
  arch/x86/include/asm/uaccess.h \
  include/lego/extable.h \
  arch/x86/include/asm/current.h \
  arch/x86/include/asm/uaccess_64.h \
  arch/x86/include/asm/processor-features.h \
    $(wildcard include/config/x86/feature/names.h) \
  arch/x86/include/asm/fpu/types.h \
  arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/vm86.h) \
    $(wildcard include/config/compat.h) \
    $(wildcard include/config/ia32/emulation.h) \
  arch/x86/include/asm/intel-family.h \

arch/x86/kernel/tsc.o: $(deps_arch/x86/kernel/tsc.o)

$(deps_arch/x86/kernel/tsc.o):
