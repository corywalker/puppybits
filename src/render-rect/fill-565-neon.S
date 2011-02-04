
// very simple neon rect fill
// actually we just use neon for the aligned block fill, and do the rest using ARM

// THESE ALL HAVE A BUG
//  Certain X sizes over-shoot, too lazy to fix right now
	
/// r0 = address
/// r1 = width
/// r2 = height
/// r3 = colour
/// [r4 = stride on stack ... use ip]
	.global	fill_rect_565_neon_test
	.balign	8
fill_rect_565_neon_test:
	ldr	ip, [sp]
	push	{ r5 - r8 }

	vdup.16	q0,r3

	orr	r3,r3,r3, lsl #16

	add	r1,r1		// convert to shorts
	mov	r8,r2		// we need r2 for strd (not in this code but too lazy to fix)
	add	r7,r0,r1	// r7 is end address
	mov	r1,r1,lsr #5

	mov	r2,r3
	
	mov	r5,r0
1:	mov	r6,r1
	add	r0,ip		// add stride

	// align start pointer to 8 bytes
	tst	r5,#2
	strneh	r2,[r5],#2
	tst	r5,#4
	strne	r2,[r5],#4

	// probably should handle small sizes separately
	cmp	r6,#0
	beq	3f
	
	// write out 32-byte chunks
2:	subs	r6,#1
	vst1.64	{ d0, d1 }, [r5, #64]! // ARM syntax is `r5 @ 64'
	vst1.64	{ d0, d1 }, [r5, #64]!
	bgt	2b

	// check for end extra bytes
3:	sub	r6,r7,r5
	tst	r6,#16
	strned	r2,[r5],#8
	strned	r2,[r5],#8
	tst	r6,#8
	strned	r2,[r5],#8
	tst	r6,#4
	strne	r2,[r5],#4
	tst	r6,#2
	strneh	r2,[r5]	
	
	subs	r8,#1
	mov	r5,r0
	bgt	1b

	pop	{ r5 - r8 }
	bx	lr

/// very minor change, this writes out all 32 bytes in 1 instruction

/// r0 = address
/// r1 = width
/// r2 = height
/// r3 = colour
/// [r4 = stride on stack ... use ip]
	.global	fill_rect_565_neon_test2
	.balign	8
fill_rect_565_neon_test2:
	ldr	ip, [sp]
	push	{ r5 - r8 }

	vdup.16	q0,r3
	vdup.16	q1,r3

	orr	r3,r3,r3, lsl #16

	add	r1,r1		// convert to shorts
	mov	r8,r2		// we need r2 for strd (not in this code but too lazy to fix)
	add	r7,r0,r1	// r7 is end address
	mov	r1,r1,lsr #5

	mov	r2,r3

	mov	r5,r0
1:	mov	r6,r1
	add	r0,ip		// add stride

	// align start pointer to 8 bytes
	tst	r5,#2
	strneh	r2,[r5],#2
	tst	r5,#4
	strne	r2,[r5],#4

	// probably should handle small sizes separately
	cmp	r6,#0
	beq	3f
	
	// write out 32-byte chunks
2:	subs	r6,#1
	vst1.64	{ d0, d1, d2, d3 }, [r5, :64]! // ARM syntax is `r5 @ 64'
	bgt	2b

	// check for end extra bytes
3:	sub	r6,r7,r5
	tst	r6,#16
	strned	r2,[r5],#8
	strned	r2,[r5],#8
	tst	r6,#8
	strned	r2,[r5],#8
	tst	r6,#4
	strne	r2,[r5],#4
	tst	r6,#2
	strneh	r2,[r5]	
	
	subs	r8,#1
	mov	r5,r0
	bgt	1b

	pop	{ r5 - r8 }
	bx	lr

	// align to 128 bits (16 bytes) instead
	.global	fill_rect_565_neon_test3
	.balign	8
fill_rect_565_neon_test3:
	ldr	ip, [sp]
	push	{ r5 - r8 }

	orr	r3,r3,r3, lsl #16

	vdup.16	q0,r3
	vdup.16	q1,r3

	add	r1,r1		// convert to shorts
	mov	r8,r2		// we need r2 for strd
	add	r7,r0,r1	// r7 is end address
	mov	r1,r1,lsr #6

	mov	r2,r3

	mov	r5,r0
1:	mov	r6,r1
	add	r0,ip		// add stride

	// align start pointer to 16 bytes
	tst	r5,#2
	strneh	r2,[r5],#2
	tst	r5,#4
	strne	r2,[r5],#4
	tst	r5,#8
	strned	r2,[r5],#8

	// probably should handle small sizes separately
	cmp	r6,#0
	beq	3f
	
	// write out 32-byte chunks
2:	vst1.64	{ d0, d1, d2, d3 }, [r5, :128]! // ARM syntax is `r5 @ 64'
	subs	r6,#1
	vst1.64	{ d0, d1, d2, d3 }, [r5, :128]!
	bgt	2b

	// check for end extra bytes
3:	sub	r6,r7,r5
	tst	r6,#32
	strned	r2,[r5],#8
	strned	r2,[r5],#8
	strned	r2,[r5],#8
	strned	r2,[r5],#8
	tst	r6,#16
	strned	r2,[r5],#8
	strned	r2,[r5],#8
	tst	r6,#8
	strned	r2,[r5],#8
	tst	r6,#4
	strne	r2,[r5],#4
	tst	r6,#2
	strneh	r2,[r5]	
	
	subs	r8,#1
	mov	r5,r0
	bgt	1b

	pop	{ r5 - r8 }
	bx	lr


	// using vmstr instead
	.global	fill_rect_565_neon_test4
	.balign	8
fill_rect_565_neon_test4:
	ldr	ip, [sp]
	push	{ r5 - r8 }

	orr	r3,r3,r3, lsl #16

	vdup.16	q0,r3
	vdup.16	q1,r3
	vdup.16	q2,r3
	vdup.16	q3,r3

	add	r1,r1		// convert to shorts
	mov	r8,r2		// we need r2 for strd
	add	r7,r0,r1	// r7 is end address
	mov	r1,r1,lsr #6

	mov	r2,r3

	mov	r5,r0
1:	mov	r6,r1
	add	r0,ip		// add stride

	// align start pointer to 16 bytes
	tst	r5,#2
	strneh	r2,[r5],#2
	tst	r5,#4
	strne	r2,[r5],#4
	tst	r5,#8
	strned	r2,[r5],#8

	// probably should handle small sizes separately
	cmp	r6,#0
	beq	3f
	
	// write out 64-byte chunks
2:	subs	r6,#1
	vstm	r5!, { d0 - d7 }
	bgt	2b

	// check for end extra bytes
3:	sub	r6,r7,r5
	tst	r6,#32
	strned	r2,[r5],#8
	strned	r2,[r5],#8
	strned	r2,[r5],#8
	strned	r2,[r5],#8
	tst	r6,#16
	strned	r2,[r5],#8
	strned	r2,[r5],#8
	tst	r6,#8
	strned	r2,[r5],#8
	tst	r6,#4
	strne	r2,[r5],#4
	tst	r6,#2
	strneh	r2,[r5]	
	
	subs	r8,#1
	mov	r5,r0
	bgt	1b

	pop	{ r5 - r8 }
	bx	lr

// from : http://groups.google.com/group/beagleboard/browse_thread/thread/12c7bd415fbc0993/456fd8eda1edde8e?lnk=gst&q=memory+bandwidth
// r0 = addr
// r1 = va
// r2 = len
// supposedly achieves: write ASM A+N      590044870 B/s
// I get about 648MB/s
	.global memset16_armneon
memset16_armneon:
        push            {r4-r11}
        mov             r3,  r0
        vdup.16         q0,  r1
        vmov            q1,  q0
        orr             r4,  r1, r1, lsl #8
        orr             r4,  r4, r4, lsl #16
        mov             r5,  r4
        mov             r6,  r4
        mov             r7,  r4
        mov             r8,  r4
        mov             r9,  r4
        mov             r10, r4
        mov             r11, r4
        add             r12, r3,  r2, lsr #2
1:      subs            r2,  r2, #128
        pld             [r3, #64]
        stm             r3!, {r4-r11}
        vst1.64         {d0-d3},   [r12,:128]!
        vst1.64         {d0-d3},   [r12,:128]!
        vst1.64         {d0-d3},   [r12,:128]!
        bgt             1b
        pop             {r4-r11}
        bx              lr 
