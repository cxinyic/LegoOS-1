cmd_arch/x86/lib/rwsem.o := gcc -Wp,-MD,arch/x86/lib/.rwsem.o.d  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/7/include -I./arch/x86/include -I./arch/x86/include/generated -I./arch/x86/include/generated/uapi  -I./include -I./managers -I./managers -include ./include/lego/kconfig.h -D__KERNEL__ -D__ASSEMBLY__ -m64 -Wa,-gdwarf-2 -DCC_HAVE_ASM_GOTO -c -o arch/x86/lib/rwsem.o arch/x86/lib/rwsem.S

source_arch/x86/lib/rwsem.o := arch/x86/lib/rwsem.S

deps_arch/x86/lib/rwsem.o := \
    $(wildcard include/config/x86/32.h) \
  include/lego/linkage.h \
  include/lego/compiler.h \
    $(wildcard include/config/smp.h) \
  arch/x86/include/asm/cache.h \
  include/lego/types.h \
    $(wildcard include/config/64bit.h) \
  arch/x86/include/asm/types.h \
    $(wildcard include/config/x86/64.h) \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/x86/alignment/16.h) \
  include/lego/stringify.h \
  arch/x86/include/asm/frame.h \
    $(wildcard include/config/frame/pointer.h) \
  arch/x86/include/asm/asm.h \
  arch/x86/include/asm/alternative-asm.h \

arch/x86/lib/rwsem.o: $(deps_arch/x86/lib/rwsem.o)

$(deps_arch/x86/lib/rwsem.o):
