/*  exceptions.S: Assembly exception entry points and init.

    Copyright (C) 2010 Michael Zucchi

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
	// Stacks: FIQ uses it's own stack, but all
	// other exceptions use the supervisor stack.
	
	.set	FIQ_STACK, 0x80008000
	
	// Processor modes
	.set	MODE_USER, 0x10
	.set	MODE_FIQ, 0x11
	.set	MODE_IRQ, 0x12
	.set	MODE_SUPERVISOR, 0x13
	.set	MODE_ABORT, 0x17
	.set	MODE_UNDEFINED, 0x1b
	.set	MODE_SYSTEM, 0x1f

#define INTCPS_BASE 0x48200000
#define INTCPS_SIR_IRQ 0x40
#define INTCPS_SIR_FIQ 0x44
#define INTCPS_CONTROL 0x48
#define INTCPS_MIR_CLEAR0 0x88

	.text
	.balign	4

	// initialise exception vectors
	.global	exceptions_init
exceptions_init:	
	// Setup the FIQ stack by jumping to that mode
	// The rest use supervisor stack (we're standing in it now)
	ldr	r0,=FIQ_STACK
	mrs	r3, cpsr
	cps	#MODE_FIQ
	mov	sp,r0
	msr	cpsr, r3

	// Default exception vectors at 0x4020ffc8-0x40210000 (end of internal omap ram)
	// But we just set VBAR to point to our vectors
	ldr	r0,=vectors
	mcr	p15, 0, r0, c12, c0, 0

	// Enable IRQ/FIQ
	// cpsie if

	bx	lr

	.balign	32
vectors:
	mov	pc, #0
        ldr     pc, v_undefined_instruction
        ldr     pc, v_software_interrupt
        ldr     pc, v_prefetch_abort
        ldr     pc, v_data_abort
        ldr     pc, v_not_used
        ldr     pc, v_irq
        ldr     pc, v_fiq

v_undefined_instruction: .word ex_undefined_instruction
v_software_interrupt:    .word ex_software_interrupt
v_prefetch_abort:        .word ex_prefetch_abort
v_data_abort:            .word ex_data_abort
v_not_used:              .word ex_not_used
v_irq:                   .word ex_irq
v_fiq:                   .word ex_fiq

	// Fatal exceptions dump all registers and stop

	.macro	exception_fail	type
	srsdb	#MODE_SUPERVISOR! // stores r4/lr and sr
	cps	#MODE_SUPERVISOR

	// This just stores all (user) registers so C can dump them out
	sub	sp,sp,#15*4
	stmdb	sp, { r0-r14 }^

	// If we came from non-user mode, also set the SP
	ldr	r1,[sp, #16*4]
	ands	r1,#15
	addne	r1,sp,#17*4
	strne	r1,[sp, #13*4]

	mov	r0,#\type
	mov	r1,sp

	bl	exception_fatal_dump
1:	b	1b
	.endm

	// Starts in undefined mode, irq disabled
	.global ex_undefined_instruction
ex_undefined_instruction:
	exception_fail	1

	// Abort mode, irq disabled
	.global	ex_prefetch_abort
ex_prefetch_abort:
	exception_fail	3
	// Abort mode, irq disabled
	.global ex_data_abort
ex_data_abort:
	exception_fail	4
	// Who knows, future expansion
	.global ex_not_used
ex_not_used:
	exception_fail	5

	// System call entry, run with interrupts enabled
	.global	ex_software_interrupt
ex_software_interrupt:
	// FIXME: appears to corrupt running system, so something's wrong here
	push	{ r0-r3, r12, lr }
	cpsie	i

	bl	exception_swi

	pop	{ r0-r3, r12, pc }^

	// Jump to supervisor mode and process for nested interrupts
	// See OMAP TRM S10.5.3 MPU INTC Preemptive Processing Sequence
	.global ex_irq
ex_irq:
	// 1. save critical registers
	sub	lr,lr,#4
	srsdb	#MODE_SUPERVISOR!
	cps	#MODE_SUPERVISOR
	push	{ r0-r3, r12, lr }

	ldr	r3,=INTCPS_BASE

	// 2,3 save and set priority threshold (not done)

	// 4. find interrupt source
	ldr	r0,[r3,#0x40]

	// 5. allow new interrupts
	mov	r1,#1
	str	r1,[r3,#INTCPS_CONTROL]

	// 6. data sync barrier for reg writes before enable irq
	dsb				 // not sure what options it should use
	
	// 7. enable irq
//	cpsie	i
	// 8. jump to handler
	ldr	r2,=irq_vectors
	and	r0,r0,#0x7f
	ldr	lr,=ex_irq_done
	ldr	pc, [r2, r0, lsl #2]
	
ex_irq_done:
	// 1. disable irq
//	cpsid	i
	// 2. restore threshold level (not done)

	// 3. restore critical registers
	pop	{ r0-r3, r12, lr }
	rfeia	sp!

	// Since FIQ's are by definition fast, don't allow nesting, other interrupts or use supervisor mode
	.global	ex_fiq
ex_fiq:
	push	{ r0-r3, lr }
	bl	exception_fiq
	pop	{ r0-r3, lr }^
	subs	pc,lr,#4

	.data
	.balign	4
	.global	irq_vectors
irq_vectors:
	.word	exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq
	.word	exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq
	.word	exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq
	.word	exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq

	.word	exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq
	.word	exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq
	.word	exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq
	.word	exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq

	.word	exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq
	.word	exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq
	.word	exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq
	.word	exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq, exception_irq
