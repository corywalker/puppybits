
// FIXME: This has a major bug in it, sometimes it goes 32-bytes over X
//        This is why it isn't enabled yet
	
/// r0 = address
/// r1 = width
/// r2 = height
/// r3 = colour
/// [r4 = stride on stack ... use ip]
	.global	fill_rect_565
	.balign	8
fill_rect_565:
	ldr	ip, [sp]
	push	{ r5 - r8 }
	
	orr	r3,r3,r3, lsl #16
	add	r1,r1		// convert to shorts
	mov	r8,r2		// we need r2 for strd
	add	r7,r0,r1	// r7 is end address
	mov	r2,r3
	mov	r1,r1,lsr #5

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
2:	strd	r2,[r5]
	strd	r2,[r5, #8]
	strd	r2,[r5, #16]
	subs	r6,#1
	strd	r2,[r5, #24]
	add	r5,r5,#32
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

fmt:	.ascii	"addr: %08x\n"
	.byte	0
	
