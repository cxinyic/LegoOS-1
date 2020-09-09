	.file	"bounds.c"
# GNU C (Ubuntu 4.8.4-2ubuntu1~14.04.4) version 4.8.4 (x86_64-linux-gnu)
#	compiled by GNU C version 4.8.4, GMP version 5.1.3, MPFR version 3.1.2-p3, MPC version 1.0.1
# GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
# options passed:  -nostdinc -I ./arch/x86/include
# -I ./arch/x86/include/generated -I ./arch/x86/include/generated/uapi
# -I ./include -I ./managers -I ./managers -imultiarch x86_64-linux-gnu
# -D __KERNEL__ -D CC_HAVE_ASM_GOTO
# -isystem /usr/lib/gcc/x86_64-linux-gnu/4.8/include
# -include ./include/lego/kconfig.h -MD kernel/.bounds.s.d kernel/bounds.c
# -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -m64 -mno-80387
# -mno-fp-ret-in-387 -mpreferred-stack-boundary=3 -mno-red-zone
# -mcmodel=kernel -maccumulate-outgoing-args -mtune=generic -march=x86-64
# -auxbase-strip kernel/bounds.s -g -O2 -Wall -Wundef -Wstrict-prototypes
# -Wno-trigraphs -Werror=implicit-function-declaration -Wno-format-security
# -Werror=uninitialized -Werror=maybe-uninitialized -Wno-sign-compare
# -Wno-maybe-uninitialized -Wno-unused-but-set-variable
# -Wdeclaration-after-statement -Wno-pointer-sign -Werror=implicit-int
# -Werror=strict-prototypes -std=gnu90 -fno-strict-aliasing -fno-common
# -fno-pie -falign-jumps=1 -falign-loops=1 -funit-at-a-time
# -fno-asynchronous-unwind-tables -fno-delete-null-pointer-checks
# -fno-omit-frame-pointer -fno-optimize-sibling-calls
# -fno-var-tracking-assignments -fno-stack-protector -fno-strict-overflow
# -fconserve-stack -fverbose-asm --param allow-store-data-races=0
# options enabled:  -faggressive-loop-optimizations -fauto-inc-dec
# -fbranch-count-reg -fcaller-saves -fcombine-stack-adjustments
# -fcompare-elim -fcprop-registers -fcrossjumping -fcse-follow-jumps
# -fdefer-pop -fdevirtualize -fdwarf2-cfi-asm -fearly-inlining
# -feliminate-unused-debug-types -fexpensive-optimizations
# -fforward-propagate -ffunction-cse -fgcse -fgcse-lm -fgnu-runtime
# -fgnu-unique -fguess-branch-probability -fhoist-adjacent-loads -fident
# -fif-conversion -fif-conversion2 -findirect-inlining -finline
# -finline-atomics -finline-functions-called-once -finline-small-functions
# -fipa-cp -fipa-profile -fipa-pure-const -fipa-reference -fipa-sra
# -fira-hoist-pressure -fira-share-save-slots -fira-share-spill-slots
# -fivopts -fkeep-static-consts -fleading-underscore -fmath-errno
# -fmerge-constants -fmerge-debug-strings -fmove-loop-invariants
# -foptimize-register-move -foptimize-strlen -fpartial-inlining -fpeephole
# -fpeephole2 -fprefetch-loop-arrays -free -freg-struct-return -fregmove
# -freorder-blocks -freorder-functions -frerun-cse-after-loop
# -fsched-critical-path-heuristic -fsched-dep-count-heuristic
# -fsched-group-heuristic -fsched-interblock -fsched-last-insn-heuristic
# -fsched-rank-heuristic -fsched-spec -fsched-spec-insn-heuristic
# -fsched-stalled-insns-dep -fschedule-insns2 -fshow-column -fshrink-wrap
# -fsigned-zeros -fsplit-ivs-in-unroller -fsplit-wide-types
# -fstrict-volatile-bitfields -fsync-libcalls -fthread-jumps
# -ftoplevel-reorder -ftrapping-math -ftree-bit-ccp -ftree-builtin-call-dce
# -ftree-ccp -ftree-ch -ftree-coalesce-vars -ftree-copy-prop
# -ftree-copyrename -ftree-cselim -ftree-dce -ftree-dominator-opts
# -ftree-dse -ftree-forwprop -ftree-fre -ftree-loop-if-convert
# -ftree-loop-im -ftree-loop-ivcanon -ftree-loop-optimize
# -ftree-parallelize-loops= -ftree-phiprop -ftree-pre -ftree-pta
# -ftree-reassoc -ftree-scev-cprop -ftree-sink -ftree-slp-vectorize
# -ftree-slsr -ftree-sra -ftree-switch-conversion -ftree-tail-merge
# -ftree-ter -ftree-vect-loop-version -ftree-vrp -funit-at-a-time
# -fvar-tracking -fverbose-asm -fzero-initialized-in-bss
# -m128bit-long-double -m64 -maccumulate-outgoing-args -malign-stringops
# -mfxsr -mglibc -mieee-fp -mlong-double-80 -mno-fancy-math-387
# -mno-red-zone -mno-sse4 -mpush-args -mtls-direct-seg-refs

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
	.loc 1 30 0
#APP
# 30 "kernel/bounds.c" 1
	
->MAX_NR_ZONES $4 __MAX_NR_ZONES	#
# 0 "" 2
	.loc 1 31 0
# 31 "kernel/bounds.c" 1
	
->NR_PAGEFLAGS $10 __NR_PAGEFLAGS	#
# 0 "" 2
	.loc 1 33 0
# 33 "kernel/bounds.c" 1
	
->NR_CPU_BITS $6 ilog2(CONFIG_NR_CPUS)	#
# 0 "" 2
	.loc 1 35 0
# 35 "kernel/bounds.c" 1
	
->SPINLOCK_SIZE $4 sizeof(spinlock_t)	#
# 0 "" 2
	.loc 1 36 0
# 36 "kernel/bounds.c" 1
	
->STRUCT_PAGE_SIZE $64 sizeof(struct page)	#
# 0 "" 2
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
	.file 3 "./include/lego/mm_zone.h"
	.file 4 "./include/lego/page-flags.h"
	.file 5 "./arch/x86/include/asm/asm.h"
	.file 6 "./arch/x86/include/asm/page_types.h"
	.file 7 "./arch/x86/include/asm/pgtable_types.h"
	.file 8 "./include/lego/sections.h"
	.file 9 "./include/lego/kernel.h"
	.file 10 "./include/lego/cpumask.h"
	.file 11 "./include/lego/preempt.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.long	0x28b
	.value	0x4
	.long	.Ldebug_abbrev0
	.byte	0x8
	.uleb128 0x1
	.long	.LASF48
	.byte	0x1
	.long	.LASF49
	.long	.LASF50
	.quad	.Ltext0
	.quad	.Letext0-.Ltext0
	.long	.Ldebug_line0
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.long	.LASF0
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.long	.LASF1
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.long	.LASF2
	.uleb128 0x3
	.string	"u8"
	.byte	0x2
	.byte	0x24
	.long	0x4c
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.long	.LASF3
	.uleb128 0x2
	.byte	0x2
	.byte	0x5
	.long	.LASF4
	.uleb128 0x3
	.string	"u16"
	.byte	0x2
	.byte	0x27
	.long	0x65
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.long	.LASF5
	.uleb128 0x4
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
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.long	.LASF9
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.long	.LASF10
	.uleb128 0x5
	.long	.LASF11
	.byte	0x7
	.value	0x11c
	.long	0x2d
	.uleb128 0x6
	.long	.LASF51
	.byte	0x8
	.byte	0xa
	.byte	0x19
	.long	0xbb
	.uleb128 0x7
	.long	.LASF52
	.byte	0xa
	.byte	0x1a
	.long	0xbb
	.byte	0
	.byte	0
	.uleb128 0x8
	.long	0x2d
	.long	0xcb
	.uleb128 0x9
	.long	0x88
	.byte	0
	.byte	0
	.uleb128 0xa
	.long	.LASF17
	.byte	0x4
	.byte	0x3
	.byte	0x35
	.long	0xf6
	.uleb128 0xb
	.long	.LASF12
	.sleb128 0
	.uleb128 0xb
	.long	.LASF13
	.sleb128 1
	.uleb128 0xb
	.long	.LASF14
	.sleb128 2
	.uleb128 0xb
	.long	.LASF15
	.sleb128 3
	.uleb128 0xb
	.long	.LASF16
	.sleb128 4
	.byte	0
	.uleb128 0xa
	.long	.LASF18
	.byte	0x4
	.byte	0x4
	.byte	0x1d
	.long	0x145
	.uleb128 0xb
	.long	.LASF19
	.sleb128 0
	.uleb128 0xb
	.long	.LASF20
	.sleb128 1
	.uleb128 0xb
	.long	.LASF21
	.sleb128 2
	.uleb128 0xb
	.long	.LASF22
	.sleb128 3
	.uleb128 0xb
	.long	.LASF23
	.sleb128 4
	.uleb128 0xb
	.long	.LASF24
	.sleb128 5
	.uleb128 0xb
	.long	.LASF25
	.sleb128 6
	.uleb128 0xb
	.long	.LASF26
	.sleb128 7
	.uleb128 0xb
	.long	.LASF27
	.sleb128 8
	.uleb128 0xb
	.long	.LASF28
	.sleb128 9
	.uleb128 0xb
	.long	.LASF29
	.sleb128 10
	.byte	0
	.uleb128 0xc
	.long	.LASF53
	.byte	0x1
	.byte	0x1b
	.quad	.LFB473
	.quad	.LFE473-.LFB473
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0xd
	.long	.LASF30
	.byte	0x5
	.byte	0xbf
	.long	0x2d
	.uleb128 0xd
	.long	.LASF31
	.byte	0x6
	.byte	0x61
	.long	0x2d
	.uleb128 0xe
	.long	.LASF32
	.byte	0x7
	.value	0x1a1
	.long	0x96
	.uleb128 0x8
	.long	0x5a
	.long	0x190
	.uleb128 0x9
	.long	0x88
	.byte	0x7
	.byte	0
	.uleb128 0xe
	.long	.LASF33
	.byte	0x7
	.value	0x1c6
	.long	0x180
	.uleb128 0x8
	.long	0x42
	.long	0x1ac
	.uleb128 0x9
	.long	0x88
	.byte	0x7
	.byte	0
	.uleb128 0xe
	.long	.LASF34
	.byte	0x7
	.value	0x1c7
	.long	0x19c
	.uleb128 0x8
	.long	0x8f
	.long	0x1c3
	.uleb128 0xf
	.byte	0
	.uleb128 0xd
	.long	.LASF35
	.byte	0x8
	.byte	0x14
	.long	0x1b8
	.uleb128 0xd
	.long	.LASF36
	.byte	0x8
	.byte	0x14
	.long	0x1b8
	.uleb128 0xd
	.long	.LASF37
	.byte	0x8
	.byte	0x17
	.long	0x1b8
	.uleb128 0xd
	.long	.LASF38
	.byte	0x8
	.byte	0x17
	.long	0x1b8
	.uleb128 0xd
	.long	.LASF39
	.byte	0x9
	.byte	0xee
	.long	0x1fa
	.uleb128 0x10
	.long	0x1b8
	.uleb128 0xd
	.long	.LASF40
	.byte	0x9
	.byte	0xef
	.long	0x20a
	.uleb128 0x10
	.long	0x1b8
	.uleb128 0x8
	.long	0x2d
	.long	0x225
	.uleb128 0x9
	.long	0x88
	.byte	0x40
	.uleb128 0x9
	.long	0x88
	.byte	0
	.byte	0
	.uleb128 0xd
	.long	.LASF41
	.byte	0xa
	.byte	0x27
	.long	0x230
	.uleb128 0x10
	.long	0x20f
	.uleb128 0xd
	.long	.LASF42
	.byte	0xa
	.byte	0x77
	.long	0xa2
	.uleb128 0xd
	.long	.LASF43
	.byte	0xa
	.byte	0x78
	.long	0xa2
	.uleb128 0xd
	.long	.LASF44
	.byte	0xa
	.byte	0x79
	.long	0xa2
	.uleb128 0xd
	.long	.LASF45
	.byte	0xa
	.byte	0x7a
	.long	0xa2
	.uleb128 0x8
	.long	0xa2
	.long	0x277
	.uleb128 0x9
	.long	0x88
	.byte	0x3f
	.uleb128 0x9
	.long	0x88
	.byte	0
	.byte	0
	.uleb128 0xe
	.long	.LASF46
	.byte	0xa
	.value	0x220
	.long	0x261
	.uleb128 0xd
	.long	.LASF47
	.byte	0xb
	.byte	0x20
	.long	0x6c
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
	.uleb128 0x4
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
	.uleb128 0x5
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
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0x4
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
	.uleb128 0xb
	.uleb128 0x28
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1c
	.uleb128 0xd
	.byte	0
	.byte	0
	.uleb128 0xc
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
	.uleb128 0xd
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
	.uleb128 0x21
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0x26
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
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
.LASF33:
	.string	"__cachemode2pte_tbl"
.LASF23:
	.string	"PG_active"
.LASF24:
	.string	"PG_reserved"
.LASF14:
	.string	"ZONE_NORMAL"
.LASF36:
	.string	"__etext"
.LASF38:
	.string	"__einittext"
.LASF19:
	.string	"PG_locked"
.LASF21:
	.string	"PG_dirty"
.LASF44:
	.string	"__cpu_present_mask"
.LASF51:
	.string	"cpumask"
.LASF29:
	.string	"__NR_PAGEFLAGS"
.LASF16:
	.string	"__MAX_NR_ZONES"
.LASF41:
	.string	"cpu_bit_bitmap"
.LASF28:
	.string	"PG_slob_free"
.LASF20:
	.string	"PG_referenced"
.LASF34:
	.string	"__pte2cachemode_tbl"
.LASF17:
	.string	"zone_type"
.LASF11:
	.string	"pteval_t"
.LASF25:
	.string	"PG_private"
.LASF0:
	.string	"long unsigned int"
.LASF5:
	.string	"short unsigned int"
.LASF3:
	.string	"unsigned char"
.LASF49:
	.string	"kernel/bounds.c"
.LASF31:
	.string	"phys_base"
.LASF37:
	.string	"__sinittext"
.LASF30:
	.string	"__force_order"
.LASF35:
	.string	"__stext"
.LASF18:
	.string	"pageflags"
.LASF45:
	.string	"__cpu_active_mask"
.LASF6:
	.string	"unsigned int"
.LASF8:
	.string	"long long unsigned int"
.LASF53:
	.string	"wuklabisawesome"
.LASF47:
	.string	"__preempt_count"
.LASF40:
	.string	"hex_asc_upper"
.LASF42:
	.string	"__cpu_possible_mask"
.LASF9:
	.string	"sizetype"
.LASF7:
	.string	"long long int"
.LASF22:
	.string	"PG_lru"
.LASF46:
	.string	"node_to_cpumask_map"
.LASF10:
	.string	"char"
.LASF26:
	.string	"PG_unevictable"
.LASF27:
	.string	"PG_slab"
.LASF52:
	.string	"bits"
.LASF13:
	.string	"ZONE_DMA32"
.LASF50:
	.string	"/users/cxinyic/LegoOS"
.LASF4:
	.string	"short int"
.LASF39:
	.string	"hex_asc"
.LASF32:
	.string	"__supported_pte_mask"
.LASF1:
	.string	"long int"
.LASF15:
	.string	"ZONE_MOVABLE"
.LASF2:
	.string	"signed char"
.LASF43:
	.string	"__cpu_online_mask"
.LASF48:
	.ascii	"GNU C 4.8.4 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx "
	.ascii	"-m64 -mno-80387 -mno-fp-ret-in-387 -mpreferred-stack-boundar"
	.ascii	"y=3 -mno-red-zone -mcmodel=kernel -maccumulate-outgoing-args"
	.ascii	" -mtune=generic -march=x86-64 -g -O2 -std=gnu90 -fno-strict-"
	.ascii	"aliasing -fno-common -fno-pie -falign-jumps=1 -falign-loops="
	.string	"1 -funit-at-a-time -fno-asynchronous-unwind-tables -fno-delete-null-pointer-checks -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-var-tracking-assignments -fno-stack-protector -fno-strict-overflow -fconserve-stack --param allow-store-data-races=0"
.LASF12:
	.string	"ZONE_DMA"
	.ident	"GCC: (Ubuntu 4.8.4-2ubuntu1~14.04.4) 4.8.4"
	.section	.note.GNU-stack,"",@progbits
