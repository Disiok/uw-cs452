	.file	"helloworld.c"
	.section	.rodata
	.align	2
.LC0:
	.ascii	"Hello world.\012\015\000"
	.align	2
.LC1:
	.ascii	"Looping.\012\015\000"
	.align	2
.LC2:
	.ascii	"Read char:\000"
	.align	2
.LC3:
	.ascii	"\012\015\000"
	.align	2
.LC4:
	.ascii	"Breaking.\012\015\000"
	.text
	.align	2
	.global	main
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 12
	@ frame_needed = 1, uses_anonymous_args = 0
	mov	ip, sp
	stmfd	sp!, {sl, fp, ip, lr, pc}
	sub	fp, ip, #4
	sub	sp, sp, #12
	ldr	sl, .L7
.L6:
	add	sl, pc, sl
	str	r0, [fp, #-24]
	str	r1, [fp, #-28]
	mov	r0, #1
	mov	r1, #0
	bl	bwsetfifo(PLT)
	mov	r0, #1
	ldr	r3, .L7+4
	add	r3, sl, r3
	mov	r1, r3
	bl	bwprintf(PLT)
.L2:
	mov	r0, #1
	ldr	r3, .L7+8
	add	r3, sl, r3
	mov	r1, r3
	bl	bwprintf(PLT)
	mov	r0, #1
	bl	bwgetc(PLT)
	mov	r3, r0
	strb	r3, [fp, #-17]
	mov	r0, #1
	ldr	r3, .L7+12
	add	r3, sl, r3
	mov	r1, r3
	bl	bwprintf(PLT)
	ldrb	r3, [fp, #-17]	@ zero_extendqisi2
	mov	r0, #1
	mov	r1, r3
	bl	bwputx(PLT)
	mov	r0, #1
	ldr	r3, .L7+16
	add	r3, sl, r3
	mov	r1, r3
	bl	bwprintf(PLT)
	ldrb	r3, [fp, #-17]	@ zero_extendqisi2
	cmp	r3, #13
	bne	.L2
	mov	r0, #1
	ldr	r3, .L7+20
	add	r3, sl, r3
	mov	r1, r3
	bl	bwprintf(PLT)
	mov	r3, #0
	mov	r0, r3
	sub	sp, fp, #16
	ldmfd	sp, {sl, fp, sp, pc}
.L8:
	.align	2
.L7:
	.word	_GLOBAL_OFFSET_TABLE_-(.L6+8)
	.word	.LC0(GOTOFF)
	.word	.LC1(GOTOFF)
	.word	.LC2(GOTOFF)
	.word	.LC3(GOTOFF)
	.word	.LC4(GOTOFF)
	.size	main, .-main
	.ident	"GCC: (GNU) 4.0.2"
