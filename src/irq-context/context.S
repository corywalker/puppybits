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

	@ this assumes we're probably going to context switch most of the time,
	@ so always saves all user registers on entry
	.global	irq_entry
irq_entry:
	sub	lr,#4
	stm	sp,{ r0-r14 }^		@ save all user regs
	srsdb	#MODE_IRQ		@ save spsr and return pc

	cps	#MODE_SUPERVISOR
	push	{ r12, lr }		@ save supervisor lr and r12 to supervisor stack

	ldr	r5,=INTCPS_BASE		@ find active interrupt from INTCPS
	ldr	r0,[r5,#0x40]
	
	ldr	r2,=irq_vectors		@ execute vectored handler
	and	r0,r0,#0x7f
	mov	lr,pc
	ldr	pc, [r2, r0, lsl #2]

	mov	r1,#1			@ tell INTCPS we've handled int
	str	r1,[r5,#INTCPS_CONTROL]
	dsb

	pop	{ r12, lr }		@ last of state on supervisor stack

	@
	@ mmu code here?  or where we select new task?
	@

	cps	#MODE_IRQ

	ldm	sp,{r0-r14}^
	rfedb	sp			@ back to new or old task

	// schedule a new task to run
	
	// r0 = pointer to regs[0] in tcb
	.global	irq_new_task
irq_new_task:
	mrs	r1,cpsr
	cps	#MODE_IRQ
	mov	sp,r0
	msr	cpsr,r1
	bx	lr

	.global	irq_current_task
irq_current_task:	
	mrs	r1,cpsr
	cps	#MODE_IRQ
	mov	r0,sp
	msr	cpsr,r1	
	bx	lr


	.text
	.balign	4

	// initialise exception vectors again, this time for our local override ones
	.global	local_exceptions_init
local_exceptions_init:	
	// Default exception vectors at 0x4020ffc8-0x40210000 (end of internal omap ram)
	// But we just set VBAR to point to our vectors
	ldr	r0,=vectors
	mcr	p15, 0, r0, c12, c0, 0
	bx	lr

	// note that we mostly just use the supplied ones
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
v_irq:                   .word irq_entry
v_fiq:                   .word ex_fiq
