	.file	"bounds.c"
# GNU C89 (Ubuntu 7.5.0-3ubuntu1~18.04) version 7.5.0 (x86_64-linux-gnu)
#	compiled by GNU C version 7.5.0, GMP version 6.1.2, MPFR version 4.0.1, MPC version 1.1.0, isl version isl-0.19-GMP

# GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
# options passed:  -nostdinc -I ./arch/x86/include
# -I ./arch/x86/include/generated -I ./arch/x86/include/generated/uapi
# -I ./include -I ./managers -I ./managers -imultiarch x86_64-linux-gnu
# -D __KERNEL__ -D CC_HAVE_ASM_GOTO
# -isystem /usr/lib/gcc/x86_64-linux-gnu/7/include
# -include ./include/lego/kconfig.h -MD kernel/.bounds.s.d kernel/bounds.c
# -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -m64 -mno-80387
# -mno-fp-ret-in-387 -mpreferred-stack-boundary=3 -mskip-rax-setup
# -mno-red-zone -mcmodel=kernel -maccumulate-outgoing-args -mtune=generic
# -march=x86-64 -auxbase-strip kernel/bounds.s -g -O2 -Wall -Wundef
# -Wstrict-prototypes -Wno-trigraphs -Werror=implicit-function-declaration
# -Wno-format-security -Werror=uninitialized -Werror=maybe-uninitialized
# -Wno-sign-compare -Wno-maybe-uninitialized -Wno-frame-address
# -Wno-unused-but-set-variable -Wunused-const-variable=0
# -Wdeclaration-after-statement -Wno-pointer-sign -Werror=implicit-int
# -Werror=strict-prototypes -Werror=date-time
# -Werror=incompatible-pointer-types -std=gnu90 -fno-strict-aliasing
# -fno-common -fno-pie -falign-jumps=1 -falign-loops=1 -funit-at-a-time
# -fno-asynchronous-unwind-tables -fno-delete-null-pointer-checks
# -fno-omit-frame-pointer -fno-optimize-sibling-calls
# -fno-var-tracking-assignments -fno-stack-protector -fno-strict-overflow
# -fconserve-stack -fverbose-asm --param allow-store-data-races=0
# options enabled:  -faggressive-loop-optimizations -falign-labels
# -fauto-inc-dec -fbranch-count-reg -fcaller-saves
# -fchkp-check-incomplete-type -fchkp-check-read -fchkp-check-write
# -fchkp-instrument-calls -fchkp-narrow-bounds -fchkp-optimize
# -fchkp-store-bounds -fchkp-use-static-bounds
# -fchkp-use-static-const-bounds -fchkp-use-wrappers -fcode-hoisting
# -fcombine-stack-adjustments -fcompare-elim -fcprop-registers
# -fcrossjumping -fcse-follow-jumps -fdefer-pop -fdevirtualize
# -fdevirtualize-speculatively -fdwarf2-cfi-asm -fearly-inlining
# -feliminate-unused-debug-types -fexpensive-optimizations
# -fforward-propagate -ffp-int-builtin-inexact -ffunction-cse -fgcse
# -fgcse-lm -fgnu-runtime -fgnu-unique -fguess-branch-probability
# -fhoist-adjacent-loads -fident -fif-conversion -fif-conversion2
# -findirect-inlining -finline -finline-atomics
# -finline-functions-called-once -finline-small-functions -fipa-bit-cp
# -fipa-cp -fipa-icf -fipa-icf-functions -fipa-icf-variables -fipa-profile
# -fipa-pure-const -fipa-ra -fipa-reference -fipa-sra -fipa-vrp
# -fira-hoist-pressure -fira-share-save-slots -fira-share-spill-slots
# -fisolate-erroneous-paths-dereference -fivopts -fkeep-static-consts
# -fleading-underscore -flifetime-dse -flra-remat -flto-odr-type-merging
# -fmath-errno -fmerge-constants -fmerge-debug-strings
# -fmove-loop-invariants -foptimize-strlen -fpartial-inlining -fpeephole
# -fpeephole2 -fplt -fprefetch-loop-arrays -free -freg-struct-return
# -freorder-blocks -freorder-functions -frerun-cse-after-loop
# -fsched-critical-path-heuristic -fsched-dep-count-heuristic
# -fsched-group-heuristic -fsched-interblock -fsched-last-insn-heuristic
# -fsched-rank-heuristic -fsched-spec -fsched-spec-insn-heuristic
# -fsched-stalled-insns-dep -fschedule-fusion -fschedule-insns2
# -fsemantic-interposition -fshow-column -fshrink-wrap
# -fshrink-wrap-separate -fsigned-zeros -fsplit-ivs-in-unroller
# -fsplit-wide-types -fssa-backprop -fssa-phiopt -fstdarg-opt
# -fstore-merging -fstrict-volatile-bitfields -fsync-libcalls
# -fthread-jumps -ftoplevel-reorder -ftrapping-math -ftree-bit-ccp
# -ftree-builtin-call-dce -ftree-ccp -ftree-ch -ftree-coalesce-vars
# -ftree-copy-prop -ftree-cselim -ftree-dce -ftree-dominator-opts
# -ftree-dse -ftree-forwprop -ftree-fre -ftree-loop-if-convert
# -ftree-loop-im -ftree-loop-ivcanon -ftree-loop-optimize
# -ftree-parallelize-loops= -ftree-phiprop -ftree-pre -ftree-pta
# -ftree-reassoc -ftree-scev-cprop -ftree-sink -ftree-slsr -ftree-sra
# -ftree-switch-conversion -ftree-tail-merge -ftree-ter -ftree-vrp
# -funit-at-a-time -fvar-tracking -fverbose-asm -fzero-initialized-in-bss
# -m128bit-long-double -m64 -maccumulate-outgoing-args -malign-stringops
# -mavx256-split-unaligned-load -mavx256-split-unaligned-store -mfxsr
# -mglibc -mieee-fp -mlong-double-80 -mno-fancy-math-387 -mno-red-zone
# -mno-sse4 -mpush-args -mskip-rax-setup -mtls-direct-seg-refs -mvzeroupper

	.text
.Ltext0:
	.cfi_sections	.debug_frame
	.p2align 4,,15
	.globl	wuklabisawesome
	.type	wuklabisawesome, @function
wuklabisawesome:
.LFB473:
	.file 1 "kernel/bounds.c"
	.loc 1 28 0
	.cfi_startproc
	pushq	%rbp	#
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp	#,
	.cfi_def_cfa_register 6
# kernel/bounds.c:30: 	DEFINE(MAX_NR_ZONES, __MAX_NR_ZONES);
	.loc 1 30 0
#APP
# 30 "kernel/bounds.c" 1
	
->MAX_NR_ZONES $4 __MAX_NR_ZONES	#
# 0 "" 2
# kernel/bounds.c:31: 	DEFINE(NR_PAGEFLAGS, __NR_PAGEFLAGS);
	.loc 1 31 0
# 31 "kernel/bounds.c" 1
	
->NR_PAGEFLAGS $10 __NR_PAGEFLAGS	#
# 0 "" 2
# kernel/bounds.c:33: 	DEFINE(NR_CPU_BITS, ilog2(CONFIG_NR_CPUS));
	.loc 1 33 0
# 33 "kernel/bounds.c" 1
	
->NR_CPU_BITS $6 ilog2(CONFIG_NR_CPUS)	#
# 0 "" 2
# kernel/bounds.c:35: 	DEFINE(SPINLOCK_SIZE, sizeof(spinlock_t));
	.loc 1 35 0
# 35 "kernel/bounds.c" 1
	
->SPINLOCK_SIZE $4 sizeof(spinlock_t)	#
# 0 "" 2
# kernel/bounds.c:36: 	DEFINE(STRUCT_PAGE_SIZE, sizeof(struct page));
	.loc 1 36 0
# 36 "kernel/bounds.c" 1
	
->STRUCT_PAGE_SIZE $64 sizeof(struct page)	#
# 0 "" 2
# kernel/bounds.c:37: }
	.loc 1 37 0
#NO_APP
	popq	%rbp	#
	.cfi_restore 6
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE473:
	.size	wuklabisawesome, .-wuklabisawesome
.Letext0:
	.file 2 "./arch/x86/include/asm/types.h"
	.file 3 "./include/lego/types.h"
	.file 4 "./arch/x86/include/asm/asm.h"
	.file 5 "./arch/x86/include/asm/page_types.h"
	.file 6 "./arch/x86/include/asm/pgtable_types.h"
	.file 7 "./include/lego/irq.h"
	.file 8 "./include/lego/sections.h"
	.file 9 "./include/lego/kernel.h"
	.file 10 "./include/lego/cpumask.h"
	.file 11 "./include/lego/percpu.h"
	.file 12 "./include/lego/preempt.h"
	.file 13 "./arch/x86/include/asm/spinlock.h"
	.file 14 "./arch/x86/include/asm/alternative.h"
	.file 15 "./include/lego/spinlock.h"
	.file 16 "./arch/x86/include/asm/pgtable.h"
	.file 17 "./include/lego/mm_zone.h"
	.file 18 "./include/lego/page-flags.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0x5c4
	.value	0x4
	.long	.Ldebug_abbrev0
	.byte	0x8
	.uleb128 0x1
	.long	.LASF105
	.byte	0x1
	.long	.LASF106
	.long	.LASF107
	.quad	.Ltext0
	.quad	.Letext0-.Ltext0
	.long	.Ldebug_line0
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.long	.LASF0
	.uleb128 0x3
	.long	0x2d
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.long	.LASF1
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.long	.LASF2
	.uleb128 0x4
	.string	"u8"
	.byte	0x2
	.byte	0x24
	.long	0x51
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.long	.LASF3
	.uleb128 0x2
	.byte	0x2
	.byte	0x5
	.long	.LASF4
	.uleb128 0x4
	.string	"u16"
	.byte	0x2
	.byte	0x27
	.long	0x6a
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.long	.LASF5
	.uleb128 0x5
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.long	.LASF6
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.long	.LASF7
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.long	.LASF8
	.uleb128 0x6
	.long	.LASF55
	.byte	0x10
	.byte	0x3
	.byte	0x51
	.long	0xb2
	.uleb128 0x7
	.long	.LASF9
	.byte	0x3
	.byte	0x52
	.long	0xb2
	.byte	0
	.uleb128 0x7
	.long	.LASF10
	.byte	0x3
	.byte	0x52
	.long	0xb2
	.byte	0x8
	.byte	0
	.uleb128 0x8
	.byte	0x8
	.long	0x8d
	.uleb128 0x9
	.long	.LASF12
	.byte	0x4
	.byte	0xbf
	.long	0x2d
	.uleb128 0xa
	.byte	0x8
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.long	.LASF11
	.uleb128 0x3
	.long	0xc5
	.uleb128 0x9
	.long	.LASF13
	.byte	0x5
	.byte	0x61
	.long	0x2d
	.uleb128 0xb
	.long	.LASF14
	.byte	0x6
	.value	0x11c
	.long	0x2d
	.uleb128 0xb
	.long	.LASF15
	.byte	0x6
	.value	0x11d
	.long	0x2d
	.uleb128 0xb
	.long	.LASF16
	.byte	0x6
	.value	0x11e
	.long	0x2d
	.uleb128 0xb
	.long	.LASF17
	.byte	0x6
	.value	0x11f
	.long	0x2d
	.uleb128 0xc
	.byte	0x8
	.byte	0x6
	.value	0x122
	.long	0x123
	.uleb128 0xd
	.string	"pte"
	.byte	0x6
	.value	0x122
	.long	0xdc
	.byte	0
	.byte	0
	.uleb128 0xb
	.long	.LASF18
	.byte	0x6
	.value	0x122
	.long	0x10c
	.uleb128 0xc
	.byte	0x8
	.byte	0x6
	.value	0x123
	.long	0x146
	.uleb128 0xd
	.string	"pmd"
	.byte	0x6
	.value	0x123
	.long	0xe8
	.byte	0
	.byte	0
	.uleb128 0xb
	.long	.LASF19
	.byte	0x6
	.value	0x123
	.long	0x12f
	.uleb128 0xc
	.byte	0x8
	.byte	0x6
	.value	0x124
	.long	0x169
	.uleb128 0xd
	.string	"pud"
	.byte	0x6
	.value	0x124
	.long	0xf4
	.byte	0
	.byte	0
	.uleb128 0xb
	.long	.LASF20
	.byte	0x6
	.value	0x124
	.long	0x152
	.uleb128 0xc
	.byte	0x8
	.byte	0x6
	.value	0x125
	.long	0x18c
	.uleb128 0xd
	.string	"pgd"
	.byte	0x6
	.value	0x125
	.long	0x100
	.byte	0
	.byte	0
	.uleb128 0xb
	.long	.LASF21
	.byte	0x6
	.value	0x125
	.long	0x175
	.uleb128 0xe
	.long	.LASF22
	.byte	0x6
	.value	0x1a1
	.long	0xdc
	.uleb128 0xf
	.long	0x5f
	.long	0x1b4
	.uleb128 0x10
	.long	0x2d
	.byte	0x7
	.byte	0
	.uleb128 0xe
	.long	.LASF23
	.byte	0x6
	.value	0x1c6
	.long	0x1a4
	.uleb128 0xf
	.long	0x47
	.long	0x1d0
	.uleb128 0x10
	.long	0x2d
	.byte	0x7
	.byte	0
	.uleb128 0xe
	.long	.LASF24
	.byte	0x6
	.value	0x1c7
	.long	0x1c0
	.uleb128 0x9
	.long	.LASF25
	.byte	0x7
	.byte	0x13
	.long	0x71
	.uleb128 0xf
	.long	0xc5
	.long	0x1f2
	.uleb128 0x11
	.byte	0
	.uleb128 0x9
	.long	.LASF26
	.byte	0x8
	.byte	0x13
	.long	0x1e7
	.uleb128 0x9
	.long	.LASF27
	.byte	0x8
	.byte	0x14
	.long	0x1e7
	.uleb128 0x9
	.long	.LASF28
	.byte	0x8
	.byte	0x14
	.long	0x1e7
	.uleb128 0x9
	.long	.LASF29
	.byte	0x8
	.byte	0x15
	.long	0x1e7
	.uleb128 0x9
	.long	.LASF30
	.byte	0x8
	.byte	0x15
	.long	0x1e7
	.uleb128 0x9
	.long	.LASF31
	.byte	0x8
	.byte	0x16
	.long	0x1e7
	.uleb128 0x9
	.long	.LASF32
	.byte	0x8
	.byte	0x16
	.long	0x1e7
	.uleb128 0x9
	.long	.LASF33
	.byte	0x8
	.byte	0x17
	.long	0x1e7
	.uleb128 0x9
	.long	.LASF34
	.byte	0x8
	.byte	0x17
	.long	0x1e7
	.uleb128 0x9
	.long	.LASF35
	.byte	0x8
	.byte	0x18
	.long	0x1e7
	.uleb128 0x9
	.long	.LASF36
	.byte	0x8
	.byte	0x18
	.long	0x1e7
	.uleb128 0x9
	.long	.LASF37
	.byte	0x8
	.byte	0x19
	.long	0x1e7
	.uleb128 0x9
	.long	.LASF38
	.byte	0x8
	.byte	0x19
	.long	0x1e7
	.uleb128 0x9
	.long	.LASF39
	.byte	0x8
	.byte	0x1a
	.long	0x1e7
	.uleb128 0x9
	.long	.LASF40
	.byte	0x8
	.byte	0x1a
	.long	0x1e7
	.uleb128 0x9
	.long	.LASF41
	.byte	0x8
	.byte	0x1b
	.long	0x1e7
	.uleb128 0x9
	.long	.LASF42
	.byte	0x8
	.byte	0x1c
	.long	0x1e7
	.uleb128 0x9
	.long	.LASF43
	.byte	0x8
	.byte	0x1c
	.long	0x1e7
	.uleb128 0x9
	.long	.LASF44
	.byte	0x8
	.byte	0x1c
	.long	0x1e7
	.uleb128 0xf
	.long	0xcc
	.long	0x2ce
	.uleb128 0x11
	.byte	0
	.uleb128 0x3
	.long	0x2c3
	.uleb128 0x9
	.long	.LASF45
	.byte	0x9
	.byte	0xee
	.long	0x2ce
	.uleb128 0x9
	.long	.LASF46
	.byte	0x9
	.byte	0xef
	.long	0x2ce
	.uleb128 0xe
	.long	.LASF47
	.byte	0x9
	.value	0x137
	.long	0x2ce
	.uleb128 0xe
	.long	.LASF48
	.byte	0x9
	.value	0x138
	.long	0x2ce
	.uleb128 0x12
	.long	.LASF86
	.byte	0x7
	.byte	0x4
	.long	0x78
	.byte	0x9
	.value	0x13d
	.long	0x332
	.uleb128 0x13
	.long	.LASF49
	.byte	0
	.uleb128 0x13
	.long	.LASF50
	.byte	0x1
	.uleb128 0x13
	.long	.LASF51
	.byte	0x2
	.uleb128 0x13
	.long	.LASF52
	.byte	0x3
	.uleb128 0x13
	.long	.LASF53
	.byte	0x4
	.byte	0
	.uleb128 0xe
	.long	.LASF54
	.byte	0x9
	.value	0x143
	.long	0x301
	.uleb128 0x6
	.long	.LASF56
	.byte	0x8
	.byte	0xa
	.byte	0x19
	.long	0x357
	.uleb128 0x7
	.long	.LASF57
	.byte	0xa
	.byte	0x1a
	.long	0x357
	.byte	0
	.byte	0
	.uleb128 0xf
	.long	0x2d
	.long	0x367
	.uleb128 0x10
	.long	0x2d
	.byte	0
	.byte	0
	.uleb128 0xf
	.long	0x34
	.long	0x37d
	.uleb128 0x10
	.long	0x2d
	.byte	0x40
	.uleb128 0x10
	.long	0x2d
	.byte	0
	.byte	0
	.uleb128 0x3
	.long	0x367
	.uleb128 0x9
	.long	.LASF58
	.byte	0xa
	.byte	0x27
	.long	0x37d
	.uleb128 0x9
	.long	.LASF59
	.byte	0xa
	.byte	0x42
	.long	0x71
	.uleb128 0x9
	.long	.LASF60
	.byte	0xa
	.byte	0x77
	.long	0x33e
	.uleb128 0x9
	.long	.LASF61
	.byte	0xa
	.byte	0x78
	.long	0x33e
	.uleb128 0x9
	.long	.LASF62
	.byte	0xa
	.byte	0x79
	.long	0x33e
	.uleb128 0x9
	.long	.LASF63
	.byte	0xa
	.byte	0x7a
	.long	0x33e
	.uleb128 0xf
	.long	0x33e
	.long	0x3da
	.uleb128 0x10
	.long	0x2d
	.byte	0x3f
	.uleb128 0x10
	.long	0x2d
	.byte	0
	.byte	0
	.uleb128 0xe
	.long	.LASF64
	.byte	0xa
	.value	0x220
	.long	0x3c4
	.uleb128 0xf
	.long	0x2d
	.long	0x3f6
	.uleb128 0x10
	.long	0x2d
	.byte	0x3f
	.byte	0
	.uleb128 0x9
	.long	.LASF65
	.byte	0xb
	.byte	0x22
	.long	0x3e6
	.uleb128 0xe
	.long	.LASF66
	.byte	0xb
	.value	0x34e
	.long	0x2d
	.uleb128 0xe
	.long	.LASF67
	.byte	0xb
	.value	0x364
	.long	0xc3
	.uleb128 0xe
	.long	.LASF68
	.byte	0xb
	.value	0x365
	.long	0x425
	.uleb128 0x8
	.byte	0x8
	.long	0x34
	.uleb128 0x9
	.long	.LASF69
	.byte	0xc
	.byte	0x20
	.long	0x71
	.uleb128 0x6
	.long	.LASF70
	.byte	0x4
	.byte	0xd
	.byte	0xd
	.long	0x44f
	.uleb128 0x7
	.long	.LASF71
	.byte	0xd
	.byte	0xe
	.long	0x78
	.byte	0
	.byte	0
	.uleb128 0x14
	.long	.LASF72
	.byte	0xd
	.byte	0xf
	.long	0x436
	.uleb128 0x9
	.long	.LASF73
	.byte	0xe
	.byte	0x21
	.long	0x71
	.uleb128 0x6
	.long	.LASF74
	.byte	0x4
	.byte	0xf
	.byte	0x21
	.long	0x47e
	.uleb128 0x7
	.long	.LASF75
	.byte	0xf
	.byte	0x22
	.long	0x44f
	.byte	0
	.byte	0
	.uleb128 0x14
	.long	.LASF76
	.byte	0xf
	.byte	0x28
	.long	0x465
	.uleb128 0xf
	.long	0x169
	.long	0x49a
	.uleb128 0x15
	.long	0x2d
	.value	0x1ff
	.byte	0
	.uleb128 0x9
	.long	.LASF77
	.byte	0x10
	.byte	0x16
	.long	0x489
	.uleb128 0x9
	.long	.LASF78
	.byte	0x10
	.byte	0x17
	.long	0x489
	.uleb128 0xf
	.long	0x146
	.long	0x4c1
	.uleb128 0x15
	.long	0x2d
	.value	0x1ff
	.byte	0
	.uleb128 0x9
	.long	.LASF79
	.byte	0x10
	.byte	0x18
	.long	0x4b0
	.uleb128 0x9
	.long	.LASF80
	.byte	0x10
	.byte	0x19
	.long	0x4b0
	.uleb128 0x9
	.long	.LASF81
	.byte	0x10
	.byte	0x1a
	.long	0x4b0
	.uleb128 0xf
	.long	0x123
	.long	0x4f3
	.uleb128 0x15
	.long	0x2d
	.value	0x1ff
	.byte	0
	.uleb128 0x9
	.long	.LASF82
	.byte	0x10
	.byte	0x1b
	.long	0x4e2
	.uleb128 0xf
	.long	0x18c
	.long	0x509
	.uleb128 0x11
	.byte	0
	.uleb128 0x9
	.long	.LASF83
	.byte	0x10
	.byte	0x1c
	.long	0x4fe
	.uleb128 0x9
	.long	.LASF84
	.byte	0x10
	.byte	0x20
	.long	0x47e
	.uleb128 0x9
	.long	.LASF85
	.byte	0x10
	.byte	0x21
	.long	0x8d
	.uleb128 0x16
	.long	.LASF87
	.byte	0x7
	.byte	0x4
	.long	0x78
	.byte	0x11
	.byte	0x35
	.long	0x55a
	.uleb128 0x13
	.long	.LASF88
	.byte	0
	.uleb128 0x13
	.long	.LASF89
	.byte	0x1
	.uleb128 0x13
	.long	.LASF90
	.byte	0x2
	.uleb128 0x13
	.long	.LASF91
	.byte	0x3
	.uleb128 0x13
	.long	.LASF92
	.byte	0x4
	.byte	0
	.uleb128 0x16
	.long	.LASF93
	.byte	0x7
	.byte	0x4
	.long	0x78
	.byte	0x12
	.byte	0x1d
	.long	0x5ae
	.uleb128 0x13
	.long	.LASF94
	.byte	0
	.uleb128 0x13
	.long	.LASF95
	.byte	0x1
	.uleb128 0x13
	.long	.LASF96
	.byte	0x2
	.uleb128 0x13
	.long	.LASF97
	.byte	0x3
	.uleb128 0x13
	.long	.LASF98
	.byte	0x4
	.uleb128 0x13
	.long	.LASF99
	.byte	0x5
	.uleb128 0x13
	.long	.LASF100
	.byte	0x6
	.uleb128 0x13
	.long	.LASF101
	.byte	0x7
	.uleb128 0x13
	.long	.LASF102
	.byte	0x8
	.uleb128 0x13
	.long	.LASF103
	.byte	0x9
	.uleb128 0x13
	.long	.LASF104
	.byte	0xa
	.byte	0
	.uleb128 0x17
	.long	.LASF108
	.byte	0x1
	.byte	0x1b
	.quad	.LFB473
	.quad	.LFE473-.LFB473
	.uleb128 0x1
	.byte	0x9c
	.byte	0
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x10
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x26
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x13
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3c
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0x21
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0x4
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x13
	.uleb128 0x28
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1c
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x14
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x15
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0x5
	.byte	0
	.byte	0
	.uleb128 0x16
	.uleb128 0x4
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x17
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0x19
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x2117
	.uleb128 0x19
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_aranges,"",@progbits
	.long	0x2c
	.value	0x2
	.long	.Ldebug_info0
	.byte	0x8
	.byte	0
	.value	0
	.value	0
	.quad	.Ltext0
	.quad	.Letext0-.Ltext0
	.quad	0
	.quad	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF85:
	.string	"pgd_list"
.LASF16:
	.string	"pudval_t"
.LASF77:
	.string	"level3_kernel_pgt"
.LASF10:
	.string	"prev"
.LASF46:
	.string	"hex_asc_upper"
.LASF50:
	.string	"SYSTEM_RUNNING"
.LASF8:
	.string	"long long unsigned int"
.LASF13:
	.string	"phys_base"
.LASF53:
	.string	"SYSTEM_RESTART"
.LASF87:
	.string	"zone_type"
.LASF51:
	.string	"SYSTEM_HALT"
.LASF41:
	.string	"__end"
.LASF24:
	.string	"__pte2cachemode_tbl"
.LASF9:
	.string	"next"
.LASF36:
	.string	"__einitdata"
.LASF104:
	.string	"__NR_PAGEFLAGS"
.LASF7:
	.string	"long long int"
.LASF2:
	.string	"signed char"
.LASF86:
	.string	"system_states"
.LASF21:
	.string	"pgd_t"
.LASF56:
	.string	"cpumask"
.LASF44:
	.string	"__per_cpu_end"
.LASF74:
	.string	"spinlock"
.LASF49:
	.string	"SYSTEM_BOOTING"
.LASF93:
	.string	"pageflags"
.LASF106:
	.string	"kernel/bounds.c"
.LASF73:
	.string	"alternatives_patched"
.LASF57:
	.string	"bits"
.LASF1:
	.string	"long int"
.LASF34:
	.string	"__einittext"
.LASF35:
	.string	"__sinitdata"
.LASF78:
	.string	"level3_ident_pgt"
.LASF76:
	.string	"spinlock_t"
.LASF30:
	.string	"__erodata"
.LASF105:
	.ascii	"GNU C89 7.5.0 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-av"
	.ascii	"x -m64 -mno-80387 -mno-fp-ret-in-387 -mpreferred-stack-bound"
	.ascii	"ary=3 -mskip-rax-setup -mno-red-zone -mcmodel=kernel -maccum"
	.ascii	"ulate-outgoing-args -mtune=generic -march=x86-64 -g -O2 -std"
	.ascii	"=gnu90 -fno-strict-aliasing -fno-common -fno-pie -falign-jum"
	.ascii	"ps=1 -falign-loops="
	.string	"1 -funit-at-a-time -fno-asynchronous-unwind-tables -fno-delete-null-pointer-checks -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-var-tracking-assignments -fno-stack-protector -fno-strict-overflow -fconserve-stack --param allow-store-data-races=0"
.LASF94:
	.string	"PG_locked"
.LASF75:
	.string	"arch_lock"
.LASF61:
	.string	"__cpu_online_mask"
.LASF97:
	.string	"PG_lru"
.LASF59:
	.string	"nr_cpu_ids"
.LASF23:
	.string	"__cachemode2pte_tbl"
.LASF95:
	.string	"PG_referenced"
.LASF33:
	.string	"__sinittext"
.LASF71:
	.string	"slock"
.LASF6:
	.string	"unsigned int"
.LASF100:
	.string	"PG_private"
.LASF63:
	.string	"__cpu_active_mask"
.LASF0:
	.string	"long unsigned int"
.LASF38:
	.string	"__bss_end"
.LASF31:
	.string	"__sdata"
.LASF5:
	.string	"short unsigned int"
.LASF22:
	.string	"__supported_pte_mask"
.LASF40:
	.string	"__brk_limit"
.LASF42:
	.string	"__per_cpu_load"
.LASF26:
	.string	"__text"
.LASF48:
	.string	"lego_proc_banner"
.LASF27:
	.string	"__stext"
.LASF72:
	.string	"arch_spinlock_t"
.LASF19:
	.string	"pmd_t"
.LASF17:
	.string	"pgdval_t"
.LASF54:
	.string	"system_state"
.LASF91:
	.string	"ZONE_MOVABLE"
.LASF18:
	.string	"pte_t"
.LASF68:
	.string	"pcpu_unit_offsets"
.LASF32:
	.string	"__edata"
.LASF58:
	.string	"cpu_bit_bitmap"
.LASF60:
	.string	"__cpu_possible_mask"
.LASF15:
	.string	"pmdval_t"
.LASF20:
	.string	"pud_t"
.LASF90:
	.string	"ZONE_NORMAL"
.LASF14:
	.string	"pteval_t"
.LASF65:
	.string	"__per_cpu_offset"
.LASF107:
	.string	"/hugedisk/sidharth/LegoOS"
.LASF3:
	.string	"unsigned char"
.LASF28:
	.string	"__etext"
.LASF4:
	.string	"short int"
.LASF88:
	.string	"ZONE_DMA"
.LASF83:
	.string	"init_level4_pgt"
.LASF102:
	.string	"PG_slab"
.LASF92:
	.string	"__MAX_NR_ZONES"
.LASF62:
	.string	"__cpu_present_mask"
.LASF66:
	.string	"this_cpu_off"
.LASF108:
	.string	"wuklabisawesome"
.LASF29:
	.string	"__srodata"
.LASF96:
	.string	"PG_dirty"
.LASF52:
	.string	"SYSTEM_POWER_OFF"
.LASF11:
	.string	"char"
.LASF81:
	.string	"level2_ident_pgt"
.LASF43:
	.string	"__per_cpu_start"
.LASF55:
	.string	"list_head"
.LASF37:
	.string	"__bss_start"
.LASF103:
	.string	"PG_slob_free"
.LASF84:
	.string	"pgd_lock"
.LASF99:
	.string	"PG_reserved"
.LASF47:
	.string	"lego_banner"
.LASF70:
	.string	"arch_spinlock"
.LASF67:
	.string	"pcpu_base_addr"
.LASF98:
	.string	"PG_active"
.LASF82:
	.string	"level1_fixmap_pgt"
.LASF45:
	.string	"hex_asc"
.LASF64:
	.string	"node_to_cpumask_map"
.LASF12:
	.string	"__force_order"
.LASF69:
	.string	"__preempt_count"
.LASF80:
	.string	"level2_fixmap_pgt"
.LASF25:
	.string	"nr_irqs"
.LASF89:
	.string	"ZONE_DMA32"
.LASF39:
	.string	"__brk_start"
.LASF79:
	.string	"level2_kernel_pgt"
.LASF101:
	.string	"PG_unevictable"
	.ident	"GCC: (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
