/* -*- mode: asm; indent-tabs-mode: nil -*- */
.intel_syntax noprefix

.globl	_copy_image

.macro LOAD phase
	movq 	mm\phase, [eax+ecx+(8*\phase)]
.endm

.macro WRITE phase
	movntq	[edx+ecx+(8*\phase)], mm\phase
.endm


	## Arguments:
	## (uint_8 * image, uint_8 * output)
_copy_image:

	push	ebp
	mov 	ebp, esp

	sub	esp, 4

	rdtsc
	mov	[esp], eax

	## Load input image address into eax
	mov	eax, [ebp+8]
	add	eax, 640*480*2	# Move it to the end of the input image

	## Load output image address into edx
	mov	edx, [ebp+12]
	add	edx, 640*480*2	# Move pointer to end of output image

	## Loop counter
	mov 	ecx, -640*480*2
loop:
	prefetchnta [eax+ecx+640]
	LOAD 	0
	LOAD 	1
	LOAD 	2
	LOAD 	3
	LOAD 	4
	LOAD 	5
	LOAD 	6
	LOAD 	7

	WRITE	0
	WRITE	1
	WRITE	2
	WRITE	3
	WRITE	4
	WRITE	5
	WRITE	6
	WRITE	7

	add	ecx, 64
	jne	loop

	rdtsc
	sub	eax, [esp]

	mov	esp, ebp

	pop	ebp

	emms

	ret
