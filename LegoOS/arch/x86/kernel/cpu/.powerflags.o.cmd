cmd_arch/x86/kernel/cpu/powerflags.o := gcc -Wp,-MD,arch/x86/kernel/cpu/.powerflags.o.d  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/7/include -I./arch/x86/include -I./arch/x86/include/generated -I./arch/x86/include/generated/uapi  -I./include -I./managers -I./managers -include ./include/lego/kconfig.h -D__KERNEL__ -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fno-pie -Werror-implicit-function-declaration -Wno-format-security -Werror=uninitialized -Werror=maybe-uninitialized -std=gnu89 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -m64 -falign-jumps=1 -falign-loops=1 -mno-80387 -mno-fp-ret-in-387 -mpreferred-stack-boundary=3 -mskip-rax-setup -mno-red-zone -mcmodel=kernel -funit-at-a-time -maccumulate-outgoing-args -pipe -Wno-sign-compare -fno-asynchronous-unwind-tables -fno-delete-null-pointer-checks -Wno-maybe-uninitialized -Wno-frame-address -O2 --param=allow-store-data-races=0 -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -g -fno-var-tracking-assignments -fno-stack-protector -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -Werror=implicit-int -Werror=strict-prototypes -Werror=date-time -Werror=incompatible-pointer-types -DCC_HAVE_ASM_GOTO -c -o arch/x86/kernel/cpu/powerflags.o arch/x86/kernel/cpu/powerflags.c

source_arch/x86/kernel/cpu/powerflags.o := arch/x86/kernel/cpu/powerflags.c

deps_arch/x86/kernel/cpu/powerflags.o := \
  arch/x86/include/asm/processor-features-flags.h \
    $(wildcard include/config/page/table/isolation.h) \
    $(wildcard include/config/x86/32.h) \

arch/x86/kernel/cpu/powerflags.o: $(deps_arch/x86/kernel/cpu/powerflags.o)

$(deps_arch/x86/kernel/cpu/powerflags.o):
