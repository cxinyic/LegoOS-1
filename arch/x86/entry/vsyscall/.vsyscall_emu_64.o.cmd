cmd_arch/x86/entry/vsyscall/vsyscall_emu_64.o := gcc -Wp,-MD,arch/x86/entry/vsyscall/.vsyscall_emu_64.o.d  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/4.8/include -I./arch/x86/include -I./arch/x86/include/generated -I./arch/x86/include/generated/uapi  -I./include -I./managers -I./managers -include ./include/lego/kconfig.h -D__KERNEL__ -D__ASSEMBLY__ -m64 -Wa,-gdwarf-2 -DCC_HAVE_ASM_GOTO -c -o arch/x86/entry/vsyscall/vsyscall_emu_64.o arch/x86/entry/vsyscall/vsyscall_emu_64.S

source_arch/x86/entry/vsyscall/vsyscall_emu_64.o := arch/x86/entry/vsyscall/vsyscall_emu_64.S

deps_arch/x86/entry/vsyscall/vsyscall_emu_64.o := \
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
  arch/x86/include/asm/page.h \
  arch/x86/include/asm/page_types.h \
    $(wildcard include/config/physical/start.h) \
  include/lego/const.h \
  arch/x86/include/asm/sparsemem.h \
    $(wildcard include/config/sparsemem.h) \
    $(wildcard include/config/x86/pae.h) \
  include/generated/unistd_64.h \

arch/x86/entry/vsyscall/vsyscall_emu_64.o: $(deps_arch/x86/entry/vsyscall/vsyscall_emu_64.o)

$(deps_arch/x86/entry/vsyscall/vsyscall_emu_64.o):
