cmd_arch/x86/realmode/rm/trampoline.o := gcc -Wp,-MD,arch/x86/realmode/rm/.trampoline.o.d  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/7/include -I./arch/x86/include -I./arch/x86/include/generated -I./arch/x86/include/generated/uapi  -I./include -I./managers -I./managers -include ./include/lego/kconfig.h -D__KERNEL__ -m16 -g -Os -D__KERNEL__ -DDISABLE_BRANCH_PROFILING -Wall -Wstrict-prototypes -march=i386 -mregparm=3 -fno-strict-aliasing -fomit-frame-pointer -fno-pic -mno-mmx -mno-sse -ffreestanding -fno-stack-protector -D__ASSEMBLY__ -c -o arch/x86/realmode/rm/trampoline.o arch/x86/realmode/rm/trampoline.S

source_arch/x86/realmode/rm/trampoline.o := arch/x86/realmode/rm/trampoline.S

deps_arch/x86/realmode/rm/trampoline.o := \
    $(wildcard include/config/physical/start.h) \
  include/lego/linkage.h \
  include/lego/compiler.h \
    $(wildcard include/config/smp.h) \
  arch/x86/include/asm/cache.h \
  include/lego/types.h \
    $(wildcard include/config/64bit.h) \
  arch/x86/include/asm/types.h \
    $(wildcard include/config/x86/64.h) \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/x86/32.h) \
    $(wildcard include/config/x86/alignment/16.h) \
  include/lego/stringify.h \
  arch/x86/include/asm/msr.h \
    $(wildcard include/config/tdp/nominal.h) \
    $(wildcard include/config/tdp/level/1.h) \
    $(wildcard include/config/tdp/level/2.h) \
    $(wildcard include/config/tdp/control.h) \
  arch/x86/include/asm/page.h \
  arch/x86/include/asm/page_types.h \
  include/lego/const.h \
  arch/x86/include/asm/sparsemem.h \
    $(wildcard include/config/sparsemem.h) \
    $(wildcard include/config/x86/pae.h) \
  arch/x86/include/asm/setup.h \
  arch/x86/include/asm/segment.h \
  arch/x86/include/asm/processor-flags.h \

arch/x86/realmode/rm/trampoline.o: $(deps_arch/x86/realmode/rm/trampoline.o)

$(deps_arch/x86/realmode/rm/trampoline.o):
