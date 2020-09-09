cmd_managers/memory/handle_checkpoint.o := gcc -Wp,-MD,managers/memory/.handle_checkpoint.o.d  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/7/include -I./arch/x86/include -I./arch/x86/include/generated -I./arch/x86/include/generated/uapi  -I./include -I./managers -I./managers -include ./include/lego/kconfig.h -D__KERNEL__ -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fno-pie -Werror-implicit-function-declaration -Wno-format-security -Werror=uninitialized -Werror=maybe-uninitialized -std=gnu89 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -m64 -falign-jumps=1 -falign-loops=1 -mno-80387 -mno-fp-ret-in-387 -mpreferred-stack-boundary=3 -mskip-rax-setup -mno-red-zone -mcmodel=kernel -funit-at-a-time -maccumulate-outgoing-args -pipe -Wno-sign-compare -fno-asynchronous-unwind-tables -fno-delete-null-pointer-checks -Wno-maybe-uninitialized -Wno-frame-address -O2 --param=allow-store-data-races=0 -Wno-unused-but-set-variable -Wno-unused-const-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -g -fno-var-tracking-assignments -fno-stack-protector -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -Werror=implicit-int -Werror=strict-prototypes -Werror=date-time -Werror=incompatible-pointer-types -DCC_HAVE_ASM_GOTO -c -o managers/memory/handle_checkpoint.o managers/memory/handle_checkpoint.c

source_managers/memory/handle_checkpoint.o := managers/memory/handle_checkpoint.c

deps_managers/memory/handle_checkpoint.o := \
  include/lego/kernel.h \
  /usr/lib/gcc/x86_64-linux-gnu/7/include/stdarg.h \
  include/lego/err.h \
  include/lego/errno.h \
  include/lego/compiler.h \
    $(wildcard include/config/smp.h) \
  arch/x86/include/asm/cache.h \
  include/lego/types.h \
    $(wildcard include/config/64bit.h) \
  arch/x86/include/asm/types.h \
    $(wildcard include/config/x86/64.h) \
  include/lego/stddef.h \
  include/lego/linkage.h \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/x86/32.h) \
    $(wildcard include/config/x86/alignment/16.h) \
  include/lego/stringify.h \
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
  include/lego/comp_common.h \
    $(wildcard include/config/default/mem/node.h) \
    $(wildcard include/config/default/storage/node.h) \
    $(wildcard include/config/soft/watchdog.h) \
  include/lego/distvm.h \
    $(wildcard include/config/debug/vma.h) \
    $(wildcard include/config/debug/vma/trace.h) \
    $(wildcard include/config/fit/nr/nodes.h) \
    $(wildcard include/config/vm/granularity/order.h) \
  include/lego/rpc/opcode.h \
  include/generated/unistd_64.h \
  include/lego/rpc/struct_common.h \
  include/lego/rpc/struct_p2m.h \
    $(wildcard include/config/distributed/vma.h) \
    $(wildcard include/config/mem/page/cache.h) \
  include/memory/replica_types.h \
  include/processor/pcache_config.h \
    $(wildcard include/config/h/.h) \
    $(wildcard include/config/pcache/line/size/shift.h) \
    $(wildcard include/config/pcache/associativity/shift.h) \
  include/lego/const.h \
  include/lego/mmap.h \
    $(wildcard include/config/mem/soft/dirty.h) \
  include/lego/rpc/struct_p2s.h \
  include/lego/stat.h \
  arch/x86/include/asm/stat.h \
  include/lego/time.h \
  include/lego/string.h \
  arch/x86/include/asm/string.h \
  arch/x86/include/asm/string_64.h \
  include/processor/statfs.h \
  include/lego/rpc/struct_m2m.h \
    $(wildcard include/config/distributed/vma/memory.h) \
  include/lego/rpc/struct_m2s.h \

managers/memory/handle_checkpoint.o: $(deps_managers/memory/handle_checkpoint.o)

$(deps_managers/memory/handle_checkpoint.o):
