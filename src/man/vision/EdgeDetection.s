.intel_syntax noprefix

.globl _sobel_operator

.section .data

.section .text

.macro Y_GRAD phase, reg
	## Load the lower row
	## mm\phase: | y13 | y12 | y11 | y10 |
	movq	mm\phase, [esi + ecx + 320 * 2 * 2 + (\phase * 4)] # Two rows after top row
	movq	mm\reg, mm\phase

	## Compute the difference between the rows
	## mm\reg: | y3 | y2 | y1 | y0 | diffs
	psubsw	mm\reg, qword ptr[esi + ecx + (\phase * 4)]

	## mm2 after pshufw: | y0 | y3 | y2 | y1 |
	pshufw	mm2, mm\reg, 0b00111001

	## mm2 after addition:
	## 	 | y0 + y3 | y3 + y2 | y2 + y1 | y0 + y1 |
	paddw	mm2, mm\reg

	## mm\reg after 2nd pshufw:
	## 	| y0 + y1 | y0 + y3 | y3 + y2 | y2 + y1 |
	pshufw	mm\reg, mm2, 0b00111001

	## mm\reg:	| xxx | xxx | y1 + 2*y2 +  y3 | y2 + 2*y1 + y0 |
	paddw	mm\reg, mm2
.endm

.macro X_GRAD phase, reg
	## bottom row in mm\phase
	## Add middle to top, twice
	paddw	mm\phase, [esi + ecx + 320*2 + (\phase * 4)]
	paddw	mm\phase, [esi + ecx + 320*2 + (\phase * 4)]

	## Add top to accumulator
	paddw	mm\phase, [esi + ecx + (\phase * 4)]

	.ifeq (\phase -1)
	movq	mm\reg, mm0
	punpckldq mm0, mm1
	punpckhdq mm\reg, mm1
	psubsw	mm\reg, mm0
	.endif
.endm

	## _sobel_operator(uint16_t *yimg, int16_t  *outX, int16_t *outY, uint16_t *mag)
_sobel_operator:
	push 	ebp
	mov	ebp, esp

	push 	esi
	push 	edi
	push	ebx

	sub 	esp, 4

	## Load arguments into registers

	## We have to move destination registers to the ends of the next row
	##
	## | o | o | o | <- source comes from this row
	## | o | Q | o | <- destination needs to write to this row, plus one byte forward for 'Q' gradient value
	## | o | o | o |
	mov	esi, [ebp+8]
	add	esi, 320 * 2	# Move pointer to end of row

	mov	ebx, [ebp+12]
	add	ebx, 320 * 2 * 2 + 2

	mov	edi, [ebp+16]
	add	edi, 320 * 2 * 2 + 2

	mov	eax, [ebp+20]
	add	eax, 320 * 2 * 2 + 2

	mov	dword ptr[esp], 238 # Actually only does from top row through third to bottom
yLoop:

	## Two pixels processed each time
	mov	ecx, -640
xLoop:
	Y_GRAD 0, 3
	X_GRAD 0, 4
	Y_GRAD 1, 5
	X_GRAD 1, 6

	punpckldq mm3, mm5	# Combine the two Y_GRAD buffers

	##
	##
	## MAGNITUDE CALCULATION
	##
	##
magnitude:
	movq	mm0, mm6	# mm6 = abs(x gradient), sign bit in mm0
	psraw	mm0, 15
	pxor	mm6, mm0
	psubw	mm6, mm0

	movq	mm1, mm3	# mm3 = abs(y gradient), sign bit in mm1
	psraw	mm1, 15
	pxor	mm3, mm1
	psubw	mm3, mm1

	paddw	mm1, mm1	# combine x and y signs ; range 0 to -3
	paddw	mm0, mm1

	movq	mm1, mm6	# Compare x and y gradient magnitudes
	pcmpgtw mm1, mm3
	paddw	mm0, mm0	# combine x >= y bit with sign bits to make octant
	paddw	mm0, mm1
	packsswb mm0, mm7	# write octants (bytes); range 0 to -7
	## movd	[octant], mm0

	psraw	mm6, 2		# make x and y gradient magnitudes fit in 8 bits
	psraw	mm3, 2
	movq	mm0, mm6	# mm0 gets larger magnitude
	pmaxsw	mm0, mm3
	movq	mm1, mm6	# mm1 gets smaller magnitude
	pminsw	mm1, mm3
	packsswb mm0, mm1	# write magnitudes (tangent of angle within octant)
	## movq	[tangent], mm0

	pmullw	mm6, mm6	# compute and write squared magnitude
	pmullw	mm3, mm3
	pavgw	mm6, mm3	# average is used to avoid 16-bit overflow
	movntq	[eax + ecx], mm6

	## Prefetch row after bottom
	prefetch [esi + ecx + 320*2*3]
	prefetchw [eax+ecx+320*2]

	## xLoop finish
	add	ecx, 8
	jne	xLoop

	## Move source pointers
	add	esi, 320 * 2	# next source row (2 bytes * 320 pixels)

	# next dest rows
	add	ebx, 320 * 2
	add	edi, 320 * 2
	add	eax, 320 * 2

	dec	dword ptr[esp]
	jne 	yLoop

	## Fix stack
	mov	esp, ebp
	sub	esp, 12

	pop	ebx
	pop	edi
	pop	esi

	pop	ebp

	emms

	ret
