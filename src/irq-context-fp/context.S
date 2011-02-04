	// Processor modes
	.set	MODE_USER, 0x10
	.set	MODE_FIQ, 0x11
	.set	MODE_IRQ, 0x12
	.set	MODE_SUPERVISOR, 0x13
	.set	MODE_ABORT, 0x17
	.set	MODE_UNDEFINED, 0x1b
	.set	MODE_SYSTEM, 0x1f

	// my gas doesn't have vmsr/vmrs so synthesise
	.macro	vmsrFPEXC	r
	mcr	p10,7,\r,c8,c0,0
	.endm
	.macro	vmrsFPEXC	r
	mrc	p10,7,\r,c8,c0,0
	.endm
	.macro	vmsrFPSCR	r
	mcr	p10,7,\r,c1,c0,0
	.endm
	.macro	vmrsFPSCR	r
	mrc	p10,7,\r,c1,c0,0
	.endm
	
#define INTCPS_BASE 0x48200000
#define INTCPS_SIR_IRQ 0x40
#define INTCPS_SIR_FIQ 0x44
#define INTCPS_CONTROL 0x48
#define INTCPS_MIR_CLEAR0 0x88

// offset of 'tcb pointer' to floating point regs - past all saved user regs
#define TCB_FPOFFSET (4*15)

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
ex_context_exit:	
	pop	{ r12, lr }		@ last of state on supervisor stack

	@
	@ mmu code here?  or where we select new task?
	@

	cps	#MODE_IRQ
	ldm	sp,{r0-r14}^
	rfedb	sp			@ back to new or old task

	// schedule a new task to run
	
	// disable/enable fpu as appropriate if we are no longer `owner' of registers.
	
	// r0 = pointer to regs[0] in tcb
	.global	irq_new_task
irq_new_task:
	// disable fpu if switching to a task who doesn't own the registers
	ldr	r3,=fptask
	ldr	r2,[r3]
	cmp	r2,r0
	movne	r1,#0
	moveq	r1,#0x40000000
	vmsrFPEXC	r1
	// set sp_irq to new task
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

	// check for fpu instruction, and act accordingly

	// encodings for vfp instructions
	// 1 1111001x        -        -        - advanced simd data processing
	// 2 xxxx1110        - xxxx101x xxx0xxxx vfp data processing
	// 3 xxxx110x        - xxxx101x xxxxxxxx ext reg l/s
	// 4 11110100 xxx0xxxx        -        - asimd struct l/s
	// 2 xxxx1110        - xxxx101x xxx1xxxx vfp<>arm transfer
	// 5 xxxx1100 010xxxxx xxxx101x        -

	@ data for fpu instruction checks
	@ if inst & mask == value -> vfp instruction
	@ must be within 255 bytes of code that uses it
	.balign	64
vfp_checks:	@ mask    ,value
	.word	0x0f000e00,0x0e000a00	@ xxxx1110        - xxxx101x -
	.word	0x0e000e00,0x0c000a00	@ xxxx110x        - xxxx101x -
	.word	0xff100000,0xf4000000	@ 11110100 xxx0xxxx        - -
	.word	0x0fe00e00,0x0c400a00	@ xxxx1100 010xxxxx xxxx101x -

	@
	@ On entry irq disabled
	@ ... so store the state on the irq stack - i.e. the TCB.
	@
	@ This is used to lazy-context-switch the fpu registers, so
	@ check that first.
	@
	@ If it's simply a code error, save the whole process context
	@ and retire the process.
	@

	@ TODO: What if this is invoked from system code?
	@ probably doesn't matter - will still suspend callee task, though stack is lost

uni_entry:	
	sub	lr,#4			@ save partial context to callee tcb via irq stack
	srsdb	#MODE_IRQ
	cps	#MODE_IRQ
	stm	sp,{ r0-r3 }
	mov	r3,sp			@ save for later r3 == &tcb.regs[0]
	cps	#MODE_SUPERVISOR

	ldr	r2,[r3,#-8]		@ load offending instruction
	ldr	r2,[r2]

	@
	@ taken branches incur 13 cycle penalty by default (if not predicted)
	@  so this is probably faster in the general case
	@
	
	and	r0,r2,#0xfe000000	@ check 1111001x xxxxxxxx xxxxxxxx xxxxxxxx
	cmp	r0,#0xf2000000
	ldrned	r0,r1,vfp_checks
	andne	r0,r0,r2
	cmpne	r0,r1
	ldrned	r0,r1,vfp_checks+8
	andne	r0,r0,r2
	cmpne	r0,r1
	ldrned	r0,r1,vfp_checks+16
	andne	r0,r0,r2
	cmpne	r0,r1
	ldrned	r0,r1,vfp_checks+24
	andne	r0,r0,r2
	cmpne	r0,r1
	bne	1f

	@
	@ Had FPU instruction - enable FPU and swap context
	@

	mov	r0,#0x40000000		@ fp context switch - enable fpu first
	vmsrFPEXC	r0
	
	ldr	r2,=fptask
	ldr	r1,[r2]
	cmp	r1,#0
	
	vmrsFPSCR	r0		@ save state from last task to use fpu, if one did
	addne	r1,#TCB_FPOFFSET
	strne	r0,[r1],#4
	vstmne	r1!,{d0-d15}
	vstmne	r1,{d16-d31}

	str	r3,[r2]			@ set current task as new owner

	add	r1,r3,#TCB_FPOFFSET	@ restore state of current task
	ldr	r0,[r1],#4
	vmsrFPSCR	r0
	vldm	r1!,{d0-d15}
	vldm	r1,{d16-d31}

	cps	#MODE_IRQ
	ldm	sp,{ r0-r3 }
	rfedb	sp

	@
	@ Unknown instruction - call back to C to handle it
	@
	@ TODO: just use asm, trivial with struct offsets
	@

1:	add	r3,r3,#4*4		@ save remaining context
	stm	r3,{r4-r14}^

	@
	@ Note: Uses common code in irq_entry to finish off
	@

	push	{ r12, lr }
	adr	lr,ex_context_exit
	b	uni_handler
	
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

v_undefined_instruction: .word uni_entry
v_software_interrupt:    .word ex_software_interrupt
v_prefetch_abort:        .word ex_prefetch_abort
v_data_abort:            .word ex_data_abort
v_not_used:              .word ex_not_used
v_irq:                   .word irq_entry
v_fiq:                   .word ex_fiq
