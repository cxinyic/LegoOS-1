cmd_arch/x86/lib/putuser.o := gcc -Wp,-MD,arch/x86/lib/.putuser.o.d  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/7/include -I./arch/x86/include -I./arch/x86/include/generated -I./arch/x86/include/generated/uapi  -I./include -I./managers -I./managers -include ./include/lego/kconfig.h -D__KERNEL__ -D__ASSEMBLY__ -m64 -Wa,-gdwarf-2 -DCC_HAVE_ASM_GOTO -c -o arch/x86/lib/putuser.o arch/x86/lib/putuser.S

source_arch/x86/lib/putuser.o := arch/x86/lib/putuser.S

deps_arch/x86/lib/putuser.o := \
    $(wildcard include/config/x86/32.h) \
  include/lego/errno.h \
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
  arch/x86/include/asm/percpu.h \
    $(wildcard include/config/x86/64/smp.h) \
    $(wildcard include/config/x86/cmpxchg64.h) \
  arch/x86/include/asm/asm.h \
  arch/x86/include/asm/extable.h \
  arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/vm86.h) \
    $(wildcard include/config/compat.h) \
    $(wildcard include/config/ia32/emulation.h) \
  include/generated/asm-offsets.h \

arch/x86/lib/putuser.o: $(deps_arch/x86/lib/putuser.o)

$(deps_arch/x86/lib/putuser.o):
