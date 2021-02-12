	.arch armv8-a
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 2
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"expamd_new_ins.c"
	.text
	.align	2
	.global	expand_file
	.syntax unified
	.arm
	.fpu softvfp
	.type	expand_file, %function
expand_file:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	cmp	r2, #1
	ldrb	r3, [r0]	@ zero_extendqisi2
	bxle	lr
	add	r2, r0, r2
	add	r1, r1, #2
	add	r0, r0, #1
.L3:
	mulaw r3, r3
	strh	r3, [r1], #2	@ movhi
	ldrb	r3, [r0, #1]!	@ zero_extendqisi2
	cmp	r0, r2
	bne	.L3
	bx	lr
	.size	expand_file, .-expand_file
	.section	.text.startup,"ax",%progbits
	.align	2
	.global	main
	.syntax unified
	.arm
	.fpu softvfp
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	mov	r0, #0
	bx	lr
	.size	main, .-main
	.ident	"GCC: (GNU Tools for Arm Embedded Processors 9-2019-q4-major) 9.2.1 20191025 (release) [ARM/arm-9-branch revision 277599]"
