cmd_arch/x86/boot/video-bios.o := gcc -Wp,-MD,arch/x86/boot/.video-bios.o.d  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/4.8/include -I./arch/x86/include -I./arch/x86/include/generated -I./arch/x86/include/generated/uapi  -I./include -I./managers -I./managers -include ./include/lego/kconfig.h -D__KERNEL__ -m32 -Wa,./arch/x86/boot/code16gcc.h -g -Os -D__KERNEL__ -DDISABLE_BRANCH_PROFILING -Wall -Wstrict-prototypes -march=i386 -mregparm=3 -fno-strict-aliasing -fomit-frame-pointer -fno-pic -mno-mmx -mno-sse -ffreestanding -fno-stack-protector -D__SETUP__ -c -o arch/x86/boot/video-bios.o arch/x86/boot/video-bios.c

source_arch/x86/boot/video-bios.o := arch/x86/boot/video-bios.c

deps_arch/x86/boot/video-bios.o := \
  arch/x86/boot/boot.h \
  arch/x86/boot/stdarg.h \
  include/lego/stddef.h \
  include/lego/compiler.h \
    $(wildcard include/config/smp.h) \
  arch/x86/include/asm/cache.h \
  include/lego/types.h \
    $(wildcard include/config/64bit.h) \
  arch/x86/include/asm/types.h \
    $(wildcard include/config/x86/64.h) \
  include/lego/linkage.h \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/x86/32.h) \
    $(wildcard include/config/x86/alignment/16.h) \
  include/lego/stringify.h \
  arch/x86/include/asm/bootparam.h \
  arch/x86/include/asm/e820.h \
    $(wildcard include/config/nodes/shift.h) \
    $(wildcard include/config/x86/pmem/legacy.h) \
    $(wildcard include/config/intel/txt.h) \
  include/lego/screen_info.h \
  arch/x86/boot/video.h \
  arch/x86/include/asm/boot.h \
    $(wildcard include/config/physical/start.h) \
    $(wildcard include/config/physical/align.h) \

arch/x86/boot/video-bios.o: $(deps_arch/x86/boot/video-bios.o)

$(deps_arch/x86/boot/video-bios.o):
