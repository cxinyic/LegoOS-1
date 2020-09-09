cmd_managers/memory/ramfs/piggy.o := gcc -Wp,-MD,managers/memory/ramfs/.piggy.o.d  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/7/include -I./arch/x86/include -I./arch/x86/include/generated -I./arch/x86/include/generated/uapi  -I./include -I./managers -I./managers -include ./include/lego/kconfig.h -D__KERNEL__ -D__ASSEMBLY__ -m64 -Wa,-gdwarf-2 -DCC_HAVE_ASM_GOTO -c -o managers/memory/ramfs/piggy.o managers/memory/ramfs/piggy.S

source_managers/memory/ramfs/piggy.o := managers/memory/ramfs/piggy.S

deps_managers/memory/ramfs/piggy.o := \
    $(wildcard include/config/use/ramfs.h) \
    $(wildcard include/config/ramfs/object/file.h) \
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

managers/memory/ramfs/piggy.o: $(deps_managers/memory/ramfs/piggy.o)

$(deps_managers/memory/ramfs/piggy.o):
