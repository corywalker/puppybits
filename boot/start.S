
	// Entry point for stand-alone code.

	// Not sure exactly what u-boot passes in to a linux kernel,
	// looks like argc, archid, tag array

	// We just need to clear bss, (actually probably need to call static inits too)
	
_start:
	ldr	sp,=0x88000000

	push	{r0-r2}

	// don't get too fancy, probably not much bss
	ldr	r4,=__bss_start__
	ldr	r5,=__bss_end__
	mov	r6,#0
1:	str	r6,[r4],#4
	cmp	r4,r5
	blo	1b

	bl	exceptions_init
	bl	neon_init

	pop	{r0-r2}

	bl	main
	b	debugmon

