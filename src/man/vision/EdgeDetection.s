/* -*- mode: asm; indent-tabs-mode: nil -*- */
.intel_syntax noprefix

.globl _sobel_operator

.section .data

.equiv  imgWd, 320
.equiv  imgHt, 240
.equiv  xPitch, 2
.equiv  yPitch, imgWd * 2
.equiv  top, 0
.equiv  mid, yPitch
.equiv  bot, yPitch * 2
.equiv  nxt, yPitch * 3

.equiv  sqMag, 0
.equiv  xGrad, imgHt * yPitch
.equiv  yGrad, imgHt * yPitch * 2

        .struct 8
thresh: .skip 4
inImg:  .skip 4
outImg:

.section .text

        ## _sobel_operator(uint16_t thresh, uint16_t *yimg, int16_t  *outX, int16_t *outY, uint16_t *mag)
_sobel_operator:
        push    ebp
        mov     ebp, esp

        push    esi
        push    edi
        push    ebx

        sub     esp, 4

        ## Load arguments into registers

        ## We have to move destination registers to the ends of the next row
        ##
        ## | o | o | o | <- source comes from this row
        ## | o | Q | o | <- destination needs to write to this row,
        ## | o | o | o | plus one byte forward for 'Q' gradient value
        pinsrw  mm6, [ebp + thresh], 0b00
        mov     esi, [ebp + inImg]
        pshufw  mm6, mm6, 0b00000000

        pcmpeqb mm7, mm7
        pandn   mm7, mm7

        mov     edi, [ebp + outImg]
        add     edi, yPitch - 2 # Adjust destination pointer

        # Actually only does from top row through third to bottom
        mov     ebx, 238
yLoop:
        ## 4 pixels processed each iteration (320 per row / 4 = 80 iterations)
        mov     ecx, 80
xLoop:
        ## Load rows into registers to save memory accesses
	movq    mm0, [esi + top]
        movq    mm1, [esi + mid]
        movq    mm2, [esi + bot]

        ## Fetch next row into L1 cache and adjust source pointer
        add     esi, 8
        prefetch [esi + nxt]

        ## X GRADIENT calculation
	# mm3 = | - | - | z3 | z2| from previous iteration.
	# Each z is top + 2 * middle + bottom
        paddw   mm1, mm1
        paddw   mm1, mm0
        paddw   mm1, mm2
        punpckldq mm3, mm1
        psubw   mm3, mm1
        movntq [edi + xGrad], mm3

        ## Y GRADIENT calculation
	# mm4 = | z3 | z2 | - | - | from previous iteration.
	# Each z is top - bottom
	psubw	mm0, mm2		# mm0 = | z7 | z6 | z5 | z4 |
	pshufw	mm2, mm4, 0b01001110	# mm2 = |  - |  - | z3 | z2 |
	punpckldq mm2, mm0		# mm2 = | z5 | z4 | z3 | z2 |
	paddw	mm2, mm0		# mm2 = | z5 + z7 | z4 + z6 | z3 + z5 | z2 + z4 |
	movq	mm5, mm4		# mm5 = | z3 | z2 |  - |  - |
	psrlq	mm5, 48			# mm5 = |  0 |  0 |  0 | z3 |
	movq	mm4, mm0		# mm4 = | z7 | z6 |  - |  - | for next iteration
	psllq	mm0, 16			# mm0 = | z6 | z5 | z4 |  0 |
	por	mm0, mm5		# mm0 = | z6 | z5 | z4 | z3 |
	paddw	mm0, mm0		# mm0 = | 2*z6 | 2*z5 | 2*z4 | 2*z3 |
	paddw	mm0, mm2		# mm0 = | z5 + 2*z6 + z7 | z4 + 2*z5 + z6 | z3 + 2*z4 + z5 | z2 + 2*z3 + z4 |
	movntq	[edi + yGrad], mm0

        ## MAGNITUDE calculation
	psllw	mm3, 3			# x gradient becomes 16 bits signed
	psllw	mm0, 3			# y gradient becomes 16 bits signed
	pmulhw	mm3, mm3		# x squared, 14 bits unsigned
	pmulhw	mm0, mm0		# y squared, 14 bits unsigned
	paddw	mm0, mm3		# magnitude squared

        # subtract noise threshold (mm6), force to 0 if below threshold
	psubusw	mm0, mm6
	movntq	[edi + sqMag], mm0

        ## We're done with the destination pointer,
        ## so increment it to next line of pixels.
        add     edi, 8

        ## Set up x sums for next iteration
        # mm3 = | - | - | z7 | z6 |
        pshufw  mm3, mm1, 0b01001110

        prefetchw [edi + yPitch]

        ## xLoop finish

        dec     ecx
        jne     xLoop

        dec     ebx
        jne     yLoop

        ## Fix stack
        mov     esp, ebp
        sub     esp, 12

        pop     ebx
        pop     edi
        pop     esi

        pop     ebp

        emms

        ret
