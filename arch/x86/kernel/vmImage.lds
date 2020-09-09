/*
 * Copyright (c) 2016-2018 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
/*
 * This file is included by all LegoOS source files.
 * Because it is passed as a parameter into GCC.
 */
/*
 *
 * Automatically generated file; DO NOT EDIT.
 * LegoOS/x86 4.0.0 Kernel Configuration
 *
 */
/*
 * Helper macros to use CONFIG_ options in C/CPP expressions. Note that
 * these only work with boolean and tristate options.
 */
/*
 * Getting something that works in C and CPP for an arg that may or may
 * not be defined is tricky.  Here, if we have "#define CONFIG_BOOGER 1"
 * we match on the placeholder define, insert the "0," for arg1 and generate
 * the triplet (0, 1, 0).  Then the last step cherry picks the 2nd arg (a one).
 * When CONFIG_BOOGER is not defined, we generate a (... 1, 0) pair, and when
 * the last step cherry picks the 2nd arg, we get a zero.
 */
/*
 * IS_BUILTIN(CONFIG_FOO) evaluates to 1 if CONFIG_FOO is set to 'y', 0
 * otherwise. For boolean options, this is equivalent to
 * IS_ENABLED(CONFIG_FOO).
 */
/*
 * IS_MODULE(CONFIG_FOO) evaluates to 1 if CONFIG_FOO is set to 'm', 0
 * otherwise.
 */
/*
 * IS_REACHABLE(CONFIG_FOO) evaluates to 1 if the currently compiled
 * code can call a function defined in code compiled based on CONFIG_FOO.
 * This is similar to IS_ENABLED(), but returns false when invoked from
 * built-in code when CONFIG_FOO is set to 'm'.
 */
/*
 * IS_ENABLED(CONFIG_FOO) evaluates to 1 if CONFIG_FOO is set to 'y' or 'm',
 * 0 otherwise.
 */
/*
 * Copyright (c) 2016-2018 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * vmImage.lds.S: ld script for the x86 kernel.
 */
/*
 * Copyright (c) 2016-2018 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
/*
 * Copyright (c) 2016-2018 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
/*
 * Copyright (c) 2016-2018 Wuklab, Purdue University. All rights reserved.
 *
 * Macros for dealing with constants.
 */
/*
 * Some constant macros are used in both assembler and
 * C code. Therefore we cannot annotate them always with
 * 'UL' and other type specifiers unilaterally. We use
 * the following macros to deal with this.
 *
 * Similarly, _AT() will cast an expression with a type in C, but
 * leave it unchanged in asm.
 */
/*
 * Copyright (c) 2016-2018 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
/*
 * Copyright (c) 2016-2018 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
/*
 * Copyright (c) 2016-2018 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
/*
 * Copyright (c) 2016-2018 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
/* L1 cache line size */
/*
 * Copyright (c) 2016-2018 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
/*
 * Copyright (c) 2016-2018 Wuklab, Purdue University. All rights reserved.
 */
/*
 * Copyright (c) 2016-2018 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
/*
 * Copyright (c) 2016-2018 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
/*
 * Indirect stringification. Doing two levels allows the parameter to be a
 * macro itself.  For example, compile with -DFOO=bar, __stringify(FOO)
 * converts to "bar".
 */
/* Some toolchains use other characters (e.g. '`') to mark new line in macro */
/*
 * For assembly routines.
 *
 * Note when using these that you must specify the appropriate
 * alignment directives yourself
 */
/* If symbol 'name' is treated as a subroutine (gets called, and returns)
 * then please use ENDPROC to mark 'name' as STT_FUNC for the benefit of
 * static analysis tools such as stack depth analyzer.
 */
/*
 * Barrier for Compiler.
 * Prevent GCC from reordering memory accesses.
 */
/*
 * Sections.
 * Work with your ld script.
 */
/*
 * ____cacheline_aligned just make the marked data cache line aligned
 * __cacheline_aligned will also put the data into a specific section
 */
/*
 * When used with Link Time Optimization, gcc can optimize away C functions or
 * variables which are referenced only from assembly code.  __visible tells the
 * optimizer that something else uses this function or variable, thus preventing
 * this.
 */
/*
 * Generic GCC Function and Variable attribute.
 * Please consult GCC manual for more details.
 */
/*
 * Rather then using noinline to prevent stack consumption, use
 * noinline_for_stack instead.  For documentation reasons.
 */
/*
 * Tell gcc if a function is cold. The compiler will assume any path
 * directly leading to the call is unlikely.
 */
/*
 * Mark a position in code as unreachable.  This can be used to
 * suppress control flow warnings after asm blocks that transfer
 * control elsewhere.
 */
/*
 * When used with Link Time Optimization, gcc can optimize away C functions or
 * variables which are referenced only from assembly code.  __visible tells the
 * optimizer that something else uses this function or variable, thus preventing
 * this.
 */
/*
 * GCC 'asm goto' miscompiles certain code sequences:
 * http://gcc.gnu.org/bugzilla/show_bug.cgi?id=58670
 * (asm goto is automatically volatile - the naming reflects this.)
 */
/*
 * __assume_aligned(n, k): Tell the optimizer that the returned
 * pointer can be assumed to be k modulo n. The second argument is
 * optional (default 0), so we use a variadic macro to make the
 * shorthand.
 *
 * Beware: Do not apply this to functions which may return
 * ERR_PTRs. Also, it is probably unwise to apply it to functions
 * returning extra information in the low bits (but in that case the
 * compiler should see some alignment anyway, when the return value is
 * massaged by 'flags = ptr & 3; ptr &= ~3;').
 */
/*
 * Prevent the compiler from merging or refetching reads or writes. The
 * compiler is also forbidden from reordering successive instances of
 * READ_ONCE, WRITE_ONCE and ACCESS_ONCE (see below), but only when the
 * compiler is aware of some particular ordering.  One way to make the
 * compiler aware of ordering is to put the two invocations of READ_ONCE,
 * WRITE_ONCE or ACCESS_ONCE() in different C statements.
 *
 * In contrast to ACCESS_ONCE these two macros will also work on aggregate
 * data types like structs or unions. If the size of the accessed data
 * type exceeds the word size of the machine (e.g., 32 bits or 64 bits)
 * READ_ONCE() and WRITE_ONCE()  will fall back to memcpy and print a
 * compile-time warning.
 *
 * Their two major use cases are: (1) Mediating communication between
 * process-level code and irq/NMI handlers, all running on the same CPU,
 * and (2) Ensuring that the compiler does not  fold, spindle, or otherwise
 * mutilate accesses that either do not require ordering or that interact
 * with an explicit memory barrier or atomic instruction that provides the
 * required ordering.
 *
 * If possible use READ_ONCE()/WRITE_ONCE() instead.
 */
/**
 * compiletime_assert - break build and emit msg if condition is false
 * @condition: a compile-time constant condition to check
 * @msg:       a message to emit if condition is false
 *
 * In tradition of POSIX assert, this macro will break the build if the
 * supplied condition is *false*, emitting the supplied error message if the
 * compiler has support to do so.
 */
/* Is this type a native word size -- useful for atomic operations */
/*
 * A trick to suppress uninitialized variable warning without generating any
 * code
 */
/*
 * This macro obfuscates arithmetic on a variable address so that gcc
 * shouldn't recognize the original var, and make assumptions about it.
 *
 * This is needed because the C standard makes it undefined to do
 * pointer arithmetic on "objects" outside their boundaries and the
 * gcc optimizers assume this is the case. In particular they
 * assume such arithmetic does not wrap.
 *
 * A miscompilation has been observed because of this on PPC.
 * To work around it we hide the relationship of the pointer and the object
 * using this macro.
 *
 * Versions of the ppc64 compiler before 4.1 had a bug where use of
 * RELOC_HIDE could trash r30. The bug can be worked around by changing
 * the inline assembly constraint from =g to =r, in this particular
 * case either is valid.
 */
/*
 * Some barriers for x86:
 */
/**
 * lockless_dereference() - safely load a pointer for later dereference
 * @p: The pointer to load
 *
 * Similar to rcu_dereference(), but for situations where the pointed-to
 * object's lifetime is managed by something other than RCU.  That
 * "something other" might be reference counting or simple immortality.
 */
/* Special 64bit data type that is 8-byte aligned */
/*
 * Copyright (c) 2016-2018 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
/*
 * generic non-linear memory support:
 *
 * 1) we will not split memory into more chunks than will fit into the flags
 *    field of the struct page
 *
 * SECTION_SIZE_BITS		2^n: size of each section
 * MAX_PHYSADDR_BITS		2^n: max size of physical address space
 * MAX_PHYSMEM_BITS		2^n: how much memory we can have in that space
 *
 */
/* Cast *PAGE_MASK to a signed type so that it is sign-extended if
   virtual addresses are 32-bits but physical addresses are larger
   (ie, 32-bit PAE). */
/* See Documentation/x86/x86_64/mm.txt for description */
/*
 * Set __PAGE_OFFSET to the most negative possible address +
 * PGDIR_SIZE*16 (pgd slot 272).  The gap is to allow a space for a
 * hypervisor to fit.  Choosing 16 slots here is arbitrary, but it's
 * what Xen requires.
 */
/* __pa_symbol should be used for C visible symbols. */
/*
 * Copyright (c) 2016-2018 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
/*
 * The setup.h is used by both 16-bit setup kernel image
 * and the setup.c in 64-bit kernel.
 */
/*
 * Copyright (c) 2016-2018 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
/*
 * Copyright (c) 2016-2018 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
/*
 * Copyright (c) 2016-2018 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
/*
 * Copyright (c) 2016-2018 Wuklab, Purdue University. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */
/* Align . to a 8 byte boundary equals to maximum function alignment. */
/*
 * Exception table
 */
/**
 * PERCPU_INPUT - the percpu input sections
 * @cacheline: cacheline size
 *
 * The core percpu section names and core symbols which do not rely
 * directly upon load addresses.
 *
 * @cacheline is used to align subsections to avoid false cacheline
 * sharing between subsections for different purposes.
 */
/**
 * PERCPU_VADDR - define output section for percpu area
 * @cacheline: cacheline size
 * @vaddr: explicit base address (optional)
 * @phdr: destination PHDR (optional)
 *
 * Macro which expands to output section for percpu area.
 *
 * @cacheline is used to align subsections to avoid false cacheline
 * sharing between subsections for different purposes.
 *
 * If @vaddr is not blank, it specifies explicit base address and all
 * percpu symbols will be offset from the given address.  If blank,
 * @vaddr always equals @laddr + LOAD_OFFSET.
 *
 * @phdr defines the output PHDR to use if not blank.  Be warned that
 * output PHDR is sticky.  If @phdr is specified, the next output
 * section in the linker script will go there too.  @phdr should have
 * a leading colon.
 *
 * Note that this macros defines __per_cpu_load as an absolute symbol.
 * If there is no need to put the percpu section at a predetermined
 * address, use PERCPU_SECTION.
 */
/*
 * DWARF debug sections.
 * Symbols in the DWARF debugging sections are relative to
 * the beginning of the section so we begin them at 0.
 */
  /* Stabs debugging sections.  */
/* in case the preprocessor is a 32bit one */
OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64")
ENTRY(startup_64)
OUTPUT_ARCH(i386:x86-64)
PHDRS {
 text 1 FLAGS(5); /* R_E */
 data 1 FLAGS(6); /* RW_ */
 percpu 1 FLAGS(6); /* RW_ */
 init 1 FLAGS(7); /* RWE */
}
SECTIONS {
 . = 0xffffffff80000000 + 0x1000000;
 phys_startup_64 = ABSOLUTE(startup_64 - 0xffffffff80000000);
 .text : AT(ADDR(.text) - 0xffffffff80000000) {
  __text = .;
  /* Bootstrapping code */
  *(.head.text)
  . = ALIGN((1 << (6)));
  /* Whole story */
  __stext = .;
  *(.text*)
  . = ALIGN(8); __sched_text_start = .; *(.sched.text) __sched_text_end = .;
  . = ALIGN(8); __fixup_text_start = .; *(.fixup) __fixup_text_end = .;
  /* End of text section */
  __etext = .;
 } :text = 0x9090
 . = ALIGN(16); __ex_table : AT(ADDR(__ex_table) - 0xffffffff80000000) { __start___ex_table = .; *(__ex_table) __stop___ex_table = .; } :text = 0x9090
 . = ALIGN((1 << 12));
 .rodata : AT(ADDR(.rodata) - 0xffffffff80000000) {
  __srodata = .;
  *(.rodata*)
  __erodata = .;
 }
 . = ALIGN((1 << 12));
 .data : AT(ADDR(.data) - 0xffffffff80000000) {
  __sdata = .;
  /*
		 * the init task, which needs to be aligned
		 * with THREAD_SIZE (16KB in 64bit)
		 */
  . = ALIGN(((1 << 12) << 4));
  *(.data..init_task)
  . = ALIGN((1 << 12));
  *(.data..page_aligned)
  . = ALIGN((1 << (6)));
  *(.data..read_mostly)
  . = ALIGN((1 << (6)));
  *(.data..cacheline_aligned)
  /* Normal data */
  . = ALIGN((1 << (6)));
  *(.data)
  /* variables inside WARN_ONCE etc. */
  . = ALIGN((1 << (6)));
  *(.data..unlikely)
  __edata = .;
 } :data
 /* Init code and data - will be freed after init */
 . = ALIGN((1 << 12));
 .init.begin : AT(ADDR(.init.begin) - 0xffffffff80000000) {
  __sinit = .;
 }
 /*
	 * percpu offsets are zero-based on SMP.  PERCPU_VADDR() changes the
	 * output PHDR, so the next output section - .init.text - should
	 * start another segment - init.
	 */
 __per_cpu_load = .; .data..percpu 0 : AT(__per_cpu_load - 0xffffffff80000000) { __per_cpu_start = .; *(.data..percpu..first) . = ALIGN((1 << 12)); *(.data..percpu..page_aligned) . = ALIGN((1 << (6))); *(.data..percpu..read_mostly) . = ALIGN((1 << (6))); *(.data..percpu) . = ALIGN((1 << (6))); *(.data..percpu..shared_aligned) __per_cpu_end = .; } :percpu . = __per_cpu_load + SIZEOF(.data..percpu);
 ASSERT(SIZEOF(.data..percpu) < 0x1000000,
  "per-CPU data too large - increase CONFIG_PHYSICAL_START")
 . = ALIGN((1 << 12));
 .init.text : AT(ADDR(.init.text) - 0xffffffff80000000) {
  __sinittext = .;
  *(.init.text)
  __einittext = .;
 } :init
 . = ALIGN((1 << (6)));
 .init.data : AT(ADDR(.init.data) - 0xffffffff80000000) {
  __sinitdata = .;
  *(.init.data)
  __einitdata = .;
 }
 . = ALIGN((1 << (6)));
 .init.rodata : AT(ADDR(.init.rodata) - 0xffffffff80000000) {
  __sinitrodata = .;
  *(.init.rodata)
  __einitrodata = .;
 }
 /*
	 * Kernel parameter setup functions
	 * __setup(str, fn);
	 */
 . = ALIGN((1 << (6)));
 .init.setup : AT(ADDR(.init.setup) - 0xffffffff80000000) {
  __sinitsetup = .;
  *(.init.setup)
  __einitsetup = .;
 }
 . = ALIGN((1 << (6)));
 .init.trampoline : AT(ADDR(.init.trampoline) - 0xffffffff80000000) {
  __sinittrampoline = .;
  *(.init.trampoline)
  __einittrampoline = .;
 }
 . = ALIGN((1 << (6)));
 .x86_cpu_vendor.init : AT(ADDR(.x86_cpu_vendor.init) - 0xffffffff80000000) {
  __x86_cpu_vendor_start = .;
  *(.x86_cpu_vendor.init)
  __x86_cpu_vendor_end = .;
 }
 . = ALIGN((1 << (6)));
 .profile.point : AT(ADDR(.profile.point) - 0xffffffff80000000) {
  __sprofilepoint = .;
  *(.profile.point)
  __eprofilepoint = .;
 }
 /*
	 * struct alt_inst entries. From the header (alternative.h):
	 * "Alternative instructions for different CPU types or capabilities"
	 * Think locking instructions on spinlocks.
	 */
 . = ALIGN((1 << (6)));
 .altinstructions : AT(ADDR(.altinstructions) - 0xffffffff80000000) {
  __alt_instructions = .;
  *(.altinstructions)
  __alt_instructions_end = .;
 }
 /*
	 * And here are the replacement instructions. The linker sticks
	 * them as binary blobs. The .altinstructions has enough data to
	 * get the address and the length of them to patch the kernel safely.
	 */
 .altinstr_replacement : AT(ADDR(.altinstr_replacement) - 0xffffffff80000000) {
  __alt_replacement = .;
  *(.altinstr_replacement)
  __alt_replacement_end = .;
 }
 . = ALIGN((1 << (6)));
 .apicdrivers : AT(ADDR(.apicdrivers) - 0xffffffff80000000) {
  __apicdrivers = .;
  *(.apicdrivers)
  __apicdrivers_end = .;
 }
/* XXX: remove me later */
 . = ALIGN((1 << 12));
 .ramfs : AT(ADDR(.ramfs) - 0xffffffff80000000) {
  __ramfs_start = .;
  *(.ramfs)
  __ramfs_end = .;
 }
 __einit = .;
 /* Checked during boot-time */
 .signature : AT(ADDR(.signature) - 0xffffffff80000000) {
  kernel_signature = .;
  LONG(0x5a5aaa55)
 }
 /* Real-content ends here */
 /* BSS */
 . = ALIGN((1 << 12));
 .bss : AT(ADDR(.bss) - 0xffffffff80000000) {
  __bss_start = .;
  *(.bss..page_aligned)
  *(.bss*)
  . = ALIGN((1 << 12));
  __bss_end = .;
 }
 /*
	 * Boot-time early brk-like allocator
	 * Use RESERVE_BRK() to reserve this area at compile time
	 * Use extend_brk() to allocate brk at run time
	 */
 . = ALIGN((1 << 12));
 .brk : AT(ADDR(.brk) - 0xffffffff80000000) {
  __brk_start = .;
  . += 64 * 1024;
  *(.brk_reservation) /* areas brk users have reserved */
  __brk_limit = .;
 }
 . = ALIGN((1 << 12));
 __end = .;
}
/*
 * Per-cpu symbols which need to be offset from __per_cpu_load
 * for the boot processor.
 *
 * Used by head.S
 */
init_per_cpu__per_cpu_head_start = per_cpu_head_start + __per_cpu_load;
init_per_cpu__cpu_gdt_page = cpu_gdt_page + __per_cpu_load;
