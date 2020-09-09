cmd_kernel/kallsyms.o := gcc -Wp,-MD,kernel/.kallsyms.o.d  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/7/include -I./arch/x86/include -I./arch/x86/include/generated -I./arch/x86/include/generated/uapi  -I./include -I./managers -I./managers -include ./include/lego/kconfig.h -D__KERNEL__ -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fno-pie -Werror-implicit-function-declaration -Wno-format-security -Werror=uninitialized -Werror=maybe-uninitialized -std=gnu89 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -m64 -falign-jumps=1 -falign-loops=1 -mno-80387 -mno-fp-ret-in-387 -mpreferred-stack-boundary=3 -mskip-rax-setup -mno-red-zone -mcmodel=kernel -funit-at-a-time -maccumulate-outgoing-args -pipe -Wno-sign-compare -fno-asynchronous-unwind-tables -fno-delete-null-pointer-checks -Wno-maybe-uninitialized -Wno-frame-address -O2 --param=allow-store-data-races=0 -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -g -fno-var-tracking-assignments -fno-stack-protector -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -Werror=implicit-int -Werror=strict-prototypes -Werror=date-time -Werror=incompatible-pointer-types -DCC_HAVE_ASM_GOTO -c -o kernel/kallsyms.o kernel/kallsyms.c

source_kernel/kallsyms.o := kernel/kallsyms.c

deps_kernel/kallsyms.o := \
    $(wildcard include/config/kallsyms/all.h) \
    $(wildcard include/config/kallsyms/base/relative.h) \
    $(wildcard include/config/kallsyms/absolute/percpu.h) \
  include/lego/types.h \
    $(wildcard include/config/64bit.h) \
  arch/x86/include/asm/types.h \
    $(wildcard include/config/x86/64.h) \
  include/lego/compiler.h \
    $(wildcard include/config/smp.h) \
  arch/x86/include/asm/cache.h \
  include/lego/linkage.h \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/x86/32.h) \
    $(wildcard include/config/x86/alignment/16.h) \
  include/lego/stringify.h \
  include/lego/stddef.h \
  include/lego/kernel.h \
  /usr/lib/gcc/x86_64-linux-gnu/7/include/stdarg.h \
  include/lego/err.h \
  include/lego/errno.h \
  include/lego/log2.h \
  include/lego/bitops.h \
  arch/x86/include/asm/bitops.h \
    $(wildcard include/config/x86/cmov.h) \
  arch/x86/include/asm/asm.h \
  include/lego/hweight.h \
  include/lego/bug.h \
  include/lego/panic.h \
  include/lego/printk.h \
  include/lego/math64.h \
  arch/x86/include/asm/div64.h \
  include/lego/cputime.h \
  include/lego/sections.h \
  include/lego/byteorder.h \
  arch/x86/include/asm/byteorder.h \
  include/lego/swab.h \
  include/uapi/limits.h \
  include/lego/string.h \
  arch/x86/include/asm/string.h \
  arch/x86/include/asm/string_64.h \
  include/lego/kallsyms.h \

kernel/kallsyms.o: $(deps_kernel/kallsyms.o)

$(deps_kernel/kallsyms.o):
