cmd_arch/x86/boot/compressed/head_64.o := gcc -Wp,-MD,arch/x86/boot/compressed/.head_64.o.d  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/7/include -I./arch/x86/include -I./arch/x86/include/generated -I./arch/x86/include/generated/uapi  -I./include -I./managers -I./managers -include ./include/lego/kconfig.h -D__KERNEL__ -m64 -D__KERNEL__ -I./arch/x86/include -I./arch/x86/include/generated -I./arch/x86/include/generated/uapi -I./include -I./managers -I./managers -include ./include/lego/kconfig.h -O2 -fno-strict-aliasing -mcmodel=small -mno-mmx -mno-sse -ffreestanding -fno-stack-protector -D__ASSEMBLY__ -c -o arch/x86/boot/compressed/head_64.o arch/x86/boot/compressed/head_64.S

source_arch/x86/boot/compressed/head_64.o := arch/x86/boot/compressed/head_64.S

deps_arch/x86/boot/compressed/head_64.o := \
    $(wildcard include/config/physical/start.h) \
  arch/x86/include/asm/msr.h \
    $(wildcard include/config/x86/64.h) \
    $(wildcard include/config/tdp/nominal.h) \
    $(wildcard include/config/tdp/level/1.h) \
    $(wildcard include/config/tdp/level/2.h) \
    $(wildcard include/config/tdp/control.h) \
  arch/x86/include/asm/boot.h \
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
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/x86/32.h) \
    $(wildcard include/config/x86/alignment/16.h) \
  include/lego/stringify.h \

arch/x86/boot/compressed/head_64.o: $(deps_arch/x86/boot/compressed/head_64.o)

$(deps_arch/x86/boot/compressed/head_64.o):
