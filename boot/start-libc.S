
	// Entry point for libc code and/or C++

	// Not sure exactly what u-boot passes in to a linux kernel,
	// looks like argc, archid, tag array

	// We just need to clear bss and call the static initialisers.
	
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

	bl	 __libc_init_array
	bl	exceptions_init

	pop	{r0-r2}
	
	b	main
