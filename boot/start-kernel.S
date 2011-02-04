	@ Copyright (C) 2010 Michael Zucchi
	@
	@ This program is free software: you can redistribute it and/or modify
	@ it under the terms of the GNU General Public License as published by
	@ the Free Software Foundation, either version 3 of the License, or
	@ (at your option) any later version.
	@
	@ This program is distributed in the hope that it will be useful,
	@ but WITHOUT ANY WARRANTY; without even the implied warranty of
	@ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	@ GNU General Public License for more details.
	@
	@ You should have received a copy of the GNU General Public License
	@ along with this program.  If not, see <http://www.gnu.org/licenses/>.

	@@ start-kernel.S
	@@
	@@ This is a somewhat more advanced startup routine that is
	@@ used to relocate the image from the load address to
	@@ a working virtual address via the MMU.
	@@
	@@ It must be used with the link-kernel.lds linker script.
	@@

	@ This is the only 'known' location the code must know about.
	@ Needed to generate the right linker arithmetic for absolute
	@ targets.

	.set	LOADADDR, 0x80008000

	// virtual memory map
	// +--------+---------
	// |fffff000| stack fence (unmapped)
	// |        | kernel stack
	//  ...
	// |        |	
	// |        | ?? system i/o remaps? ?? shared libraries?
	//  ...
	// |        |
	// |        | kernel data
	// |f0000000| kernel text / start-kernel.S  (address set by linker script - can be anything)
	// +--------+---------
	// |        |
	// |        |
	// |        | 
	// |80000000| ?? system i/o remaps?  ?? shared libaries?     ^^ above here is globally mapped (TTBR1)
	// +--------+---------
	// |        | `agile' memory
	// |        | app stack(s)
	// |        |
	// |        |
	// |40000000|
	// +--------+---------
	// |        | app heap
	// |        | app bss
	// |        | app data
	// |00000000| app text
	// +--------+---------                                        ^^ per-process user memory (TTBR0)

	// physical memory map
	// +--------+---------
	//  ...
	// |        |
	// |        | kernel bss
	// |        | kernel data
	// |80009000| kernel text
	// |80008000| start-kernel.S
	// |80000000| kernel stack/workspace
	// +--------+---------
	// |...     |

	.section	.init, "x"

//#define DEBUG 1
#define uart 0x49020000
#define gpio5 0x49056000
	
#define LEDOFF 0x49056090
#define LEDON 0x49056094
	
#define LED0 0x00200000
#define LED1 0x00400000
	
#define SCTLR_TE (1<<30)
#define SCTLR_AFE (1<<29)
#define SCTLR_TRE (1<<28)
#define SCTLR_ICACHE (1<<12)	/* I */
#define SCTLR_PREDICT (1<<11)	/* Z */
#define SCTLR_DCACHE (1<<2)	/* C */
#define SCTLR_ALIGN (1<<1)	/* A */
#define SCTLR_MMUEN (1<<0)	/* M */

#define CP15_SCTLR c1, c0, 0
#define CP15_TTBR0 c2, c0, 0
#define CP15_TTBR1 c2, c0, 1
#define CP15_TTBCR c2, c0, 2
#define CP15_DACR c3, c0, 0

#define CP15_TLBIALL c8, c7, 0

#ifdef DEBUG
	.macro	OUTX, reg
	push	{ r0 }
	mov	r0,\reg
	bl	outx
	pop	{ r0 }
	.endm
	.macro	OUTC, val
	push	{ r0 }
	mov	r0,\val
	bl	outc
	pop	{ r0 }
	.endm
#else
	.macro	OUTX, reg
	.endm
	.macro	OUTC, val
	.endm
#endif
	
	@
	@ Initial entry point
	@
	
_start:
	adr	sp,_start
	adr	r12,_start	       	@ this will be physical load address
	push	{ r0 - r3 }

	@
	@ Clear bss
	@

	ldr	r1,bss_offset
	ldr	r2,bss_offset+4
	add	r1,r1,r12
	add	r2,r2,r12
	mov	r0,#0
1:	str	r0,[r1],#4
	cmp	r1,r2
	blo	1b

	@
	@ Init page table using table
	@

	ldr	r11,ttb_offset
	add	r11,r12		       	@ physical address of kernel_ttb
	add	r10,r11,#16384	       	@ same for kernel_pages

	adr	r9,ttb_map
	mov	r8,#ttb_size
1:	ldm	r9!, { r4, r5, r6, r7 }	@ virtual dest, start offset, virtual end, flags
	add	r5,r12			@ physical address

2:	mov	r3,r4,lsr #20
	ldr	r2,[r11, r3, lsl #2]
	cmp	r2,#0

	@
	@ Allocate new L2 page
	@

	moveq	r2,r10
	addeq	r10,#1024
	orreq	r2,#1
	streq	r2,[r11, r3, lsl #2]

	@
	@ Write PTE
	@
	
	bic	r2,#0xff			@ r2 = physical address of l2 page
	mov	r1,r4,lsr #12
	and	r1,#0xff
	orr	r0,r5,r7
	str	r0,[r2, r1, lsl #2]

	OUTX	r4
	OUTC	#' '
	OUTX	r5
	OUTC	#' '
	OUTX	r1
	OUTC	#'\n'
	
	@
	@ Loop for all pages, note the cmp.eq <- addresses must be aligned
	@

	add	r4,#4096
	add	r5,#4096
	cmp	r4,r6
	bne	2b

	subs	r8,#1
	bne	1b

	@
	@ Turn on MMU
	@

	@ MMU already off, Initialise registers
	mov	r0,#0
	mov	r1,#1

	mcr	p15, 0, r0, CP15_TLBIALL	@ clear tlb cache
	mcr	p15, 0, r1, CP15_TTBCR		@ Top 2G uses TTBR1
	mcr	p15, 0, r11, CP15_TTBR0
	mcr	p15, 0, r11, CP15_TTBR1
	mcr	p15, 0, r0, CP15_TLBIALL	@ clear tlb cache
	sub	r0,#1
	mcr	p15, 0, r0, CP15_DACR		@ all domains permits

	@ Whilst we still have the correct stack, restore arguments.
	pop	{ r0 - r3 }

	@ Finally turn on MMU (and caches and branch prediction).
	ldr	r8,=(SCTLR_ICACHE | SCTLR_DCACHE | SCTLR_PREDICT | SCTLR_MMUEN)
	mcr	p15, 0, r8, CP15_SCTLR
	
	@
	@ Jump to virtual address code, and start up
	@

	ldr	pc, =vstart

	@
	@ This is now running at target address
	@
	
vstart: ldr	sp,=-4096			@ init stack
@	bl	__libc_init_array		@ static intialisers
	mov	r8,#(0xf<<20)			@ enable NEON coprocessor access (still off though)
	mcr	p15, 0, r8, c1, c0, 2
@	bl	exceptions_init			@ ????
	b	main

#ifdef DEBUG
outc:
	push	{ r1, r2 }
	ldr	r2,=uart
1:	ldr	r1,[r2,#0x14]
	tst	r1,#0x20
	beq	1b
	str	r0,[r2]
	pop	{ r1, r2 }
	bx	lr

tohex:	.ascii	"0123456789ABCDEF"

outx:	push	{ r0-r6, lr }
2:	adr	r6,tohex
	mov	r5,#32
	mov	r4,r0
1:	sub	r5,r5,#4
	
	cmp	r4,#0
	moveq	r0,#32
	lsrne	r0,r4,r5
	andne	r0,#0x0f
	ldrneb	r0,[r6,r0]
	cmpeq	r5,#0
	moveq	r0,#'.'

	bl	outc
	cmp	r5,#0
	bne	1b
	pop	{ r0-r6, pc }
#endif
	
bss_offset:
	.word	__bss_start__ - _start
	.word	__bss_end__ - _start
ttb_offset:
	.word	kernel_ttb - _start
	// describes initial memory map
	// format is:
	// word: target virtual address
	// word: relative offset from start of range to _start (can be anywhere in memory)
	// word: end virtual address
	// word: ttl2 flags
	// All addresses must align to pages!
	@.set	CODE, 0x00000398|2
	@.set	DATA, 0x00000154|2
	@.set	NDEV, 0x00000090|2
	.set	CODE, 0x000003b8|2
	.set	DATA, 0x00000174|2
	.set	NDEV, 0x00000014|2
ttb_map:
	.word	LOADADDR, 0, LOADADDR + start_sizeof, CODE		@ this page, so mmu can be enabled
	.word	0 - 32768 - 4096, 0x80000000 - LOADADDR, 0-4096, DATA	@ system stack, 32K, 4K from end of memory
	.word	__executable_start, 0, __data_start__, CODE		@ kernel text at virt address
	.word	__data_start__, __data_start__-_start, __bss_end__,DATA	@ kernel data
	.word	UART3, 0x49020000 - LOADADDR, UART3+4096, NDEV		@ do serial port too, for debug stuff
	.word	GPIO5, 0x49056000 - LOADADDR, GPIO5+4096, NDEV		@ i/o of gpio, for debug too (LEDs!)
	.set	ttb_size, (. - ttb_map) / 16

	@ Make sure we include constants in initial page range
	
	.ltorg

	@ All code that might execute at load address included within this range

	.set	start_sizeof, ((. - _start)+4095) & 0xfffff000

	.bss
	.balign		16384
	.global	kernel_ttb, kernel_pages, UART3, GPIO5
kernel_ttb:
	.skip	16384
kernel_pages:
	.skip	1024*64
UART3:	.skip	4096
GPIO5:	.skip	4096
