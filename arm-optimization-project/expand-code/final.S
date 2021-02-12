	.arch armv6
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 1
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"expand_pipe_no_lut.c"
	.text
	.align	2
	.global	expand_file
	.syntax unified
	.arm
	.fpu vfp
	.type	expand_file, %function
expand_file:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r9, r10, lr}
	cmp	r2, #3
	movle	lr, #0
	ble	.L2
	ldrb	r6, [r0]	@ zero_extendqisi2
	eor	r6, r6, #127
	lsl	r3, r6, #1
	and	r3, r3, #30
	ldrb	r10, [r0, #1]	@ zero_extendqisi2
	eor	r10, r10, #127
	orr	r3, r3, #33
	lsr	r4, r6, #4
	and	r4, r4, #7
	lsl	r3, r3, r4
	and	r6, r6, #128
	lsl	r6, r6, #8
	sub	r3, r3, #33
	uxth	r4, r3
	lsr	r9, r10, #4
	and	r9, r9, #7
	lsl	r3, r10, #1
	and	r3, r3, #30
	and	r10, r10, #128
	orr	r3, r3, #33
	ldrb	ip, [r0, #2]	@ zero_extendqisi2
	eor	ip, ip, #127
	ldrb	r5, [r0, #3]	@ zero_extendqisi2
	cmp	r2, #4
	ble	.L8
	add	lr, r0, #4
	add	r8, r0, r2
	mov	r7, r1

	push { r0, r1, r2 }			@ steal extra registers not used in the loop
	mov	r1, #33					@ load constant for loops so ca use barrel shifter
	mov	r2, #7
	mov	r0, #-33
.L4:
	orr	r4, r6, r4, lsl #2		@ stage 5
	strh	r4, [r7], #2		@ movhi
				
	lsl	r6, r10, #8				@ stage 4
	add	r4, r0, r3, lsl r9 		@ condensed shift and sub into add with -33
	
	and	r3, ip, #0x0000000F		@stage 3
	and	r9, r2, ip, lsr #4 		@ condensed to use barrel shifter
	and	r10, ip, #128
	orr	r3, r1, r3, lsl #1 		@ condensed to use barrel shifter

	eor	ip, r5, #127			@ stage 2

	ldrb	r5, [lr], #1		@ stage 1 @ zero_extendqisi2
	cmp	lr, r8
	bne	.L4

	pop { r0, r1, r2 }

	sub	lr, r2, #-2147483644
	add	r1, r1, lr, lsl #1
	mov	lr, r2
.L3:
	orr	r4, r6, r4, lsl #2
	strh	r4, [r1]	@ movhi
	lsl	r3, r3, r9
	lsl	r6, ip, #1
	and	r6, r6, #30
	eor	r5, r5, #127
	sub	r3, r3, #33
	uxth	r3, r3
	lsl	r10, r10, #8
	orr	r3, r10, r3, lsl #2
	strh	r3, [r1, #2]	@ movhi
	orr	r6, r6, #33
	lsr	r3, ip, #4
	and	r3, r3, #7
	lsl	r6, r6, r3
	lsl	r3, r5, #1
	and	r4, r3, #30
	sub	r3, r6, #33
	uxth	r6, r3
	and	r3, ip, #128
	lsl	r3, r3, #8
	orr	r3, r3, r6, lsl #2
	strh	r3, [r1, #4]	@ movhi
	orr	r3, r4, #33
	lsr	ip, r5, #4
	and	ip, ip, #7
	lsl	r3, r3, ip
	sub	r3, r3, #33
	uxth	ip, r3
	bic	r3, r5, #127
	uxtb	r3, r3
	lsl	r3, r3, #8
	orr	r3, r3, ip, lsl #2
	strh	r3, [r1, #6]	@ movhi
	add	r1, r1, #8
.L2:
	cmp	r2, lr
	pople	{r4, r5, r6, r7, r8, r9, r10, pc}
	add	lr, r0, lr
	add	r2, r0, r2
.L6:
	ldrb	r3, [lr], #1	@ zero_extendqisi2
	eor	r3, r3, #127
	lsl	ip, r3, #1
	and	ip, ip, #30
	orr	ip, ip, #33
	lsr	r0, r3, #4
	and	r0, r0, #7
	lsl	ip, ip, r0
	sub	ip, ip, #33
	uxth	ip, ip
	and	r3, r3, #128
	lsl	r3, r3, #8
	orr	r3, r3, ip, lsl #2
	strh	r3, [r1], #2	@ movhi
	cmp	lr, r2
	bne	.L6
	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
.L8:
	mov	lr, #4
	b	.L3
	.size	expand_file, .-expand_file
	.align	2
	.global	store_little_4
	.syntax unified
	.arm
	.fpu vfp
	.type	store_little_4, %function
store_little_4:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	sub	sp, sp, #8
	mov	r4, r1
	mov	r3, r0
	mov	r2, #1
	mov	r1, #4
	add	r0, sp, r1
	bl	fread
	ldrb	r2, [sp, #5]	@ zero_extendqisi2
	ldrb	r3, [sp, #6]	@ zero_extendqisi2
	lsl	r3, r3, #16
	orr	r3, r3, r2, lsl #8
	ldrb	r2, [sp, #4]	@ zero_extendqisi2
	orr	r3, r3, r2
	ldrb	r2, [sp, #7]	@ zero_extendqisi2
	orr	r3, r3, r2, lsl #24
	str	r3, [r4]
	add	sp, sp, #8
	@ sp needed
	pop	{r4, pc}
	.size	store_little_4, .-store_little_4
	.align	2
	.global	write_little_4
	.syntax unified
	.arm
	.fpu vfp
	.type	write_little_4, %function
write_little_4:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	str	lr, [sp, #-4]!
	sub	sp, sp, #12
	strb	r1, [sp, #4]
	lsr	r3, r1, #8
	strb	r3, [sp, #5]
	lsr	r3, r1, #16
	strb	r3, [sp, #6]
	lsr	r1, r1, #24
	strb	r1, [sp, #7]
	mov	r3, r0
	mov	r2, #1
	mov	r1, #4
	add	r0, sp, r1
	bl	fwrite
	add	sp, sp, #12
	@ sp needed
	ldr	pc, [sp], #4
	.size	write_little_4, .-write_little_4
	.align	2
	.global	store_little_2
	.syntax unified
	.arm
	.fpu vfp
	.type	store_little_2, %function
store_little_2:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	sub	sp, sp, #8
	mov	r4, r1
	mov	r3, r0
	mov	r2, #1
	mov	r1, #2
	add	r0, sp, #4
	bl	fread
	ldrb	r2, [sp, #5]	@ zero_extendqisi2
	ldrb	r3, [sp, #4]	@ zero_extendqisi2
	orr	r3, r3, r2, lsl #8
	strh	r3, [r4]	@ movhi
	add	sp, sp, #8
	@ sp needed
	pop	{r4, pc}
	.size	store_little_2, .-store_little_2
	.align	2
	.global	write_little_2
	.syntax unified
	.arm
	.fpu vfp
	.type	write_little_2, %function
write_little_2:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 0, uses_anonymous_args = 0
	str	lr, [sp, #-4]!
	sub	sp, sp, #12
	strb	r1, [sp, #4]
	lsr	r1, r1, #8
	strb	r1, [sp, #5]
	mov	r3, r0
	mov	r2, #1
	mov	r1, #2
	add	r0, sp, #4
	bl	fwrite
	add	sp, sp, #12
	@ sp needed
	ldr	pc, [sp], #4
	.size	write_little_2, .-write_little_2
	.align	2
	.global	read_wav_header
	.syntax unified
	.arm
	.fpu vfp
	.type	read_wav_header, %function
read_wav_header:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	mov	r4, r0
	mov	r5, r1
	mov	r3, r0
	mov	r2, #1
	mov	r1, #4
	mov	r0, r5
	bl	fread
	add	r1, r5, #4
	mov	r0, r4
	bl	store_little_4
	mov	r3, r4
	mov	r2, #1
	mov	r1, #4
	add	r0, r5, #8
	bl	fread
	mov	r3, r4
	mov	r2, #1
	mov	r1, #4
	add	r0, r5, #12
	bl	fread
	add	r1, r5, #16
	mov	r0, r4
	bl	store_little_4
	add	r1, r5, #20
	mov	r0, r4
	bl	store_little_2
	add	r1, r5, #22
	mov	r0, r4
	bl	store_little_2
	add	r1, r5, #24
	mov	r0, r4
	bl	store_little_4
	add	r1, r5, #28
	mov	r0, r4
	bl	store_little_4
	add	r1, r5, #32
	mov	r0, r4
	bl	store_little_2
	add	r1, r5, #34
	mov	r0, r4
	bl	store_little_2
	mov	r3, r4
	mov	r2, #1
	mov	r1, #4
	add	r0, r5, #36
	bl	fread
	add	r1, r5, #40
	mov	r0, r4
	bl	store_little_4
	pop	{r4, r5, r6, pc}
	.size	read_wav_header, .-read_wav_header
	.global	__aeabi_uidiv
	.global	__aeabi_idiv
	.align	2
	.global	read_wav
	.syntax unified
	.arm
	.fpu vfp
	.type	read_wav, %function
read_wav:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r9, r10, lr}
	mov	r5, r1
	mov	r8, r2
	ldr	r1, .L34
	bl	fopen
	subs	r6, r0, #0
	beq	.L31
	mov	r1, r5
	mov	r0, r6
	bl	read_wav_header
	ldrh	r3, [r5, #20]
	cmp	r3, #1
	bne	.L32
	ldrh	r3, [r5, #22]
	ldrh	r4, [r5, #34]
	mul	r4, r4, r3
	ldr	r0, [r5, #40]
	mov	r1, r4
	lsl	r0, r0, #3
	bl	__aeabi_uidiv
	mov	r9, r0
	mov	r7, r0
	add	r3, r4, #7
	cmp	r4, #0
	movlt	r4, r3
	asr	r4, r4, #3
	mov	r2, r0
	asr	r3, r0, #31
	strd	r2, [r8]
	mov	r2, r4
	asr	r3, r4, #31
	strd	r2, [r8, #8]
	ldrh	r8, [r5, #22]
	mov	r1, r8
	mov	r0, r4
	bl	__aeabi_idiv
	mul	r0, r8, r0
	cmp	r4, r0
	bne	.L33
	mov	r1, r4
	mov	r0, r9
	bl	calloc
	str	r0, [r5, #44]
	cmp	r9, #0
	blt	.L27
	mov	r4, #0
.L28:
	ldr	r1, [r5, #44]
	add	r1, r1, r4, lsl #1
	mov	r0, r6
	bl	store_little_2
	add	r4, r4, #1
	cmp	r7, r4
	bge	.L28
.L27:
	mov	r0, r6
	bl	fclose
	mov	r0, #0
	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
.L31:
	ldr	r3, .L34+4
	ldr	r3, [r3]
	mov	r2, #29
	mov	r1, #1
	ldr	r0, .L34+8
	bl	fwrite
	mvn	r0, #0
	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
.L32:
	ldr	r3, .L34+4
	ldr	r3, [r3]
	mov	r2, #39
	mov	r1, #1
	ldr	r0, .L34+12
	bl	fwrite
	mvn	r0, #0
	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
.L33:
	ldr	r3, .L34+4
	ldr	r3, [r3]
	mov	r2, #20
	mov	r1, #1
	ldr	r0, .L34+16
	bl	fwrite
	mvn	r0, #0
	pop	{r4, r5, r6, r7, r8, r9, r10, pc}
.L35:
	.align	2
.L34:
	.word	.LC0
	.word	stderr
	.word	.LC1
	.word	.LC2
	.word	.LC3
	.size	read_wav, .-read_wav
	.align	2
	.global	write_header
	.syntax unified
	.arm
	.fpu vfp
	.type	write_header, %function
write_header:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, lr}
	mov	r4, r0
	mov	r5, r1
	mov	r3, r0
	mov	r2, #1
	mov	r1, #4
	mov	r0, r5
	bl	fwrite
	ldr	r1, [r5, #4]
	mov	r0, r4
	bl	write_little_4
	mov	r3, r4
	mov	r2, #1
	mov	r1, #4
	add	r0, r5, #8
	bl	fwrite
	mov	r3, r4
	mov	r2, #1
	mov	r1, #4
	add	r0, r5, #12
	bl	fwrite
	ldr	r1, [r5, #16]
	mov	r0, r4
	bl	write_little_4
	ldrh	r1, [r5, #20]
	mov	r0, r4
	bl	write_little_2
	ldrh	r1, [r5, #22]
	mov	r0, r4
	bl	write_little_2
	ldr	r1, [r5, #24]
	mov	r0, r4
	bl	write_little_4
	ldr	r1, [r5, #28]
	mov	r0, r4
	bl	write_little_4
	ldrh	r1, [r5, #32]
	mov	r0, r4
	bl	write_little_2
	ldrh	r1, [r5, #34]
	mov	r0, r4
	bl	write_little_2
	mov	r3, r4
	mov	r2, #1
	mov	r1, #4
	add	r0, r5, #36
	bl	fwrite
	ldr	r1, [r5, #40]
	mov	r0, r4
	bl	write_little_4
	pop	{r4, r5, r6, pc}
	.size	write_header, .-write_header
	.align	2
	.global	write_wav
	.syntax unified
	.arm
	.fpu vfp
	.type	write_wav, %function
write_wav:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, lr}
	mov	r6, r1
	mov	r7, r2
	ldr	r1, .L46
	ldr	r0, .L46+4
	bl	fopen
	subs	r5, r0, #0
	beq	.L45
	mov	r1, r6
	mov	r0, r5
	bl	write_header
	cmp	r7, #0
	blt	.L41
	mov	r4, #0
.L42:
	ldr	r2, [r6, #44]
	lsl	r3, r4, #1
	ldrh	r1, [r2, r3]
	mov	r0, r5
	bl	write_little_2
	add	r4, r4, #1
	cmp	r7, r4
	bge	.L42
.L41:
	mov	r0, r5
	bl	fclose
	mov	r0, #0
	pop	{r4, r5, r6, r7, r8, pc}
.L45:
	ldr	r3, .L46+8
	ldr	r3, [r3]
	mov	r2, #46
	mov	r1, #1
	ldr	r0, .L46+12
	bl	fwrite
	mvn	r0, #0
	pop	{r4, r5, r6, r7, r8, pc}
.L47:
	.align	2
.L46:
	.word	.LC4
	.word	.LC5
	.word	stderr
	.word	.LC6
	.size	write_wav, .-write_wav
	.align	2
	.global	test
	.syntax unified
	.arm
	.fpu vfp
	.type	test, %function
test:
	@ args = 0, pretend = 0, frame = 64
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, r5, r6, r7, r8, r9, r10, fp, lr}
	sub	sp, sp, #68
	mov	r5, r0
	ldr	r1, .L61
	ldr	r0, .L61+4
	bl	fopen
	subs	r8, r0, #0
	beq	.L58
	mov	r3, r8
	mov	r2, #1
	mov	r1, #16
	add	r0, sp, #48
	bl	fread
	mov	r3, r8
	mov	r2, #1
	mov	r1, #48
	mov	r0, sp
	bl	fread
	ldr	r4, [sp, #48]
	mov	r1, #1
	mov	r0, r4
	bl	calloc
	mov	r7, r0
	mov	r1, #2
	mov	r0, r4
	bl	calloc
	mov	r9, r0
	mov	r1, #2
	mov	r0, r4
	bl	calloc
	cmp	r7, #0
	cmpne	r9, #0
	moveq	r3, #1
	movne	r3, #0
	mov	r10, r0
	cmp	r0, #0
	movne	r0, r3
	orreq	r0, r3, #1
	cmp	r0, #0
	bne	.L59
	mov	r3, r8
	mov	r2, r4
	mov	r1, #1
	mov	r0, r7
	bl	fread
	ldr	r2, [sp, #48]
	mov	r1, r9
	mov	r0, r7
	blx	r5
	ldr	r1, .L61
	ldr	r0, .L61+8
	bl	fopen
	subs	fp, r0, #0
	beq	.L60
	mov	r3, fp
	ldr	r2, [sp, #48]
	mov	r1, #2
	mov	r0, r10
	bl	fread
	ldrd	r2, [sp, #48]
	orrs	r3, r2, r3
	beq	.L53
	sub	r6, r10, #2
	sub	r5, r9, #2
	mov	r4, #0
	b	.L55
.L58:
	ldr	r0, .L61+12
	bl	puts
	mvn	r0, #0
	b	.L48
.L59:
	ldr	r0, .L61+16
	bl	puts
	mvn	r0, #0
	b	.L48
.L60:
	ldr	r0, .L61+20
	bl	puts
	mvn	r0, #0
	b	.L48
.L54:
	add	r4, r4, #1
	asr	r1, r4, #31
	ldrd	r2, [sp, #48]
	cmp	r1, r3
	cmpeq	r4, r2
	bcs	.L53
.L55:
	ldrh	r1, [r6, #2]!
	ldrh	r2, [r5, #2]!
	cmp	r1, r2
	beq	.L54
	ldrb	r3, [r7, r4]	@ zero_extendqisi2
	ldr	r0, .L61+24
	bl	printf
	b	.L54
.L53:
	mov	r0, r7
	bl	free
	mov	r0, r9
	bl	free
	mov	r0, r10
	bl	free
	mov	r0, r8
	bl	fclose
	mov	r0, fp
	bl	fclose
	mov	r0, #0
.L48:
	add	sp, sp, #68
	@ sp needed
	pop	{r4, r5, r6, r7, r8, r9, r10, fp, pc}
.L62:
	.align	2
.L61:
	.word	.LC0
	.word	.LC7
	.word	.LC10
	.word	.LC8
	.word	.LC9
	.word	.LC11
	.word	.LC12
	.size	test, .-test
	.align	2
	.global	main
	.syntax unified
	.arm
	.fpu vfp
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
	ldr	r0, .L65
	bl	test
	mov	r0, #0
	pop	{r4, pc}
.L66:
	.align	2
.L65:
	.word	expand_file
	.size	main, .-main
	.section	.rodata.str1.4,"aMS",%progbits,1
	.align	2
.LC0:
	.ascii	"r\000"
	.space	2
.LC1:
	.ascii	"Unable to open provided file\012\000"
	.space	2
.LC2:
	.ascii	"Only support pulse code modulated type\012\000"
.LC3:
	.ascii	"Incorrect file size\012\000"
	.space	3
.LC4:
	.ascii	"w+\000"
	.space	1
.LC5:
	.ascii	"mu_expand_output.wav\000"
	.space	3
.LC6:
	.ascii	"Unable to open/create file with provided name\012\000"
	.space	1
.LC7:
	.ascii	"compressed.txt\000"
	.space	1
.LC8:
	.ascii	"Could not open compressed.txt, please create one\000"
	.space	3
.LC9:
	.ascii	"Error allocating memory\000"
.LC10:
	.ascii	"./expanded.txt\000"
	.space	1
.LC11:
	.ascii	"Could not open expanded.txt, please create one\000"
	.space	1
.LC12:
	.ascii	"%X %X %X\012\000"
	.ident	"GCC: (Raspbian 6.3.0-18+rpi1+deb9u1) 6.3.0 20170516"
	.section	.note.GNU-stack,"",%progbits
