cmd_arch/x86/realmode/piggy.o := gcc -Wp,-MD,arch/x86/realmode/.piggy.o.d  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/4.8/include -I./arch/x86/include -I./arch/x86/include/generated -I./arch/x86/include/generated/uapi  -I./include -I./managers -I./managers -include ./include/lego/kconfig.h -D__KERNEL__ -D__ASSEMBLY__ -m64 -Wa,-gdwarf-2 -DCC_HAVE_ASM_GOTO -c -o arch/x86/realmode/piggy.o arch/x86/realmode/piggy.S

source_arch/x86/realmode/piggy.o := arch/x86/realmode/piggy.S

deps_arch/x86/realmode/piggy.o := \
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

arch/x86/realmode/piggy.o: $(deps_arch/x86/realmode/piggy.o)

$(deps_arch/x86/realmode/piggy.o):
