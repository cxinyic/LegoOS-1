cmd_arch/x86/boot/header.o := gcc -Wp,-MD,arch/x86/boot/.header.o.d  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/4.8/include -I./arch/x86/include -I./arch/x86/include/generated -I./arch/x86/include/generated/uapi  -I./include -I./managers -I./managers -include ./include/lego/kconfig.h -D__KERNEL__ -m32 -Wa,./arch/x86/boot/code16gcc.h -g -Os -D__KERNEL__ -DDISABLE_BRANCH_PROFILING -Wall -Wstrict-prototypes -march=i386 -mregparm=3 -fno-strict-aliasing -fomit-frame-pointer -fno-pic -mno-mmx -mno-sse -ffreestanding -fno-stack-protector -D__SETUP__ -D__ASSEMBLY__ -DSVGA_MODE=NORMAL_VGA -I./arch/x86/boot -c -o arch/x86/boot/header.o arch/x86/boot/header.S

source_arch/x86/boot/header.o := arch/x86/boot/header.S

deps_arch/x86/boot/header.o := \
    $(wildcard include/config/physical/align.h) \
    $(wildcard include/config/x86/64.h) \
  arch/x86/boot/boot.h \
  arch/x86/include/asm/boot.h \
    $(wildcard include/config/physical/start.h) \
  arch/x86/include/asm/setup.h \
  arch/x86/include/asm/bootparam.h \

arch/x86/boot/header.o: $(deps_arch/x86/boot/header.o)

$(deps_arch/x86/boot/header.o):
