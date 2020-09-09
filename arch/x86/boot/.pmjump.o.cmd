cmd_arch/x86/boot/pmjump.o := gcc -Wp,-MD,arch/x86/boot/.pmjump.o.d  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/4.8/include -I./arch/x86/include -I./arch/x86/include/generated -I./arch/x86/include/generated/uapi  -I./include -I./managers -I./managers -include ./include/lego/kconfig.h -D__KERNEL__ -m32 -Wa,./arch/x86/boot/code16gcc.h -g -Os -D__KERNEL__ -DDISABLE_BRANCH_PROFILING -Wall -Wstrict-prototypes -march=i386 -mregparm=3 -fno-strict-aliasing -fomit-frame-pointer -fno-pic -mno-mmx -mno-sse -ffreestanding -fno-stack-protector -D__SETUP__ -D__ASSEMBLY__ -DSVGA_MODE=NORMAL_VGA -c -o arch/x86/boot/pmjump.o arch/x86/boot/pmjump.S

source_arch/x86/boot/pmjump.o := arch/x86/boot/pmjump.S

deps_arch/x86/boot/pmjump.o := \
  arch/x86/include/asm/boot.h \
    $(wildcard include/config/physical/start.h) \
    $(wildcard include/config/physical/align.h) \
  arch/x86/include/asm/segment.h \
  include/lego/const.h \
  arch/x86/include/asm/processor-flags.h \
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

arch/x86/boot/pmjump.o: $(deps_arch/x86/boot/pmjump.o)

$(deps_arch/x86/boot/pmjump.o):
