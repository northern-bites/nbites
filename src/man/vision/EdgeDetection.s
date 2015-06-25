/* -*- mode: asm; indent-tabs-mode: nil -*- */
.intel_syntax noprefix

        ## Expose these routines to the linker
.globl _sobel_operator
.globl _find_edge_peaks

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
bound_sobel_param:  .skip 4
thresh: .skip 4
inImg:  .skip 4
outImg:

# Tables and structs for ***Edge Peak Loop***
#
        ## Parameter struct
                        .struct 8
bound_peak_param:       .skip 4
gradients_param:        .skip 4
angles_param:           .skip 4
field_edges_param:      .skip 4

        ## Stack layout
                .struct 0
row_count:      .skip 4
angles_ptr:     .skip 4
num_peaks:      .skip 4
bound_val:      .skip 4
end_of_peak_stack:

        .equiv neighborOffset, 12*4
        .equiv quadrantOffset, 12
        .equiv octantOffset, 12

        .data
#
# Octant code in edx:
#       \ -1  -2|2   3  /
#       -3\     |     / 1
#           \   |   /
#       -4    \ | /     0
#       -----------------
#      -12    / | \    -8
#           /   |   \
#      -11/     |     \-7
#       / -9 -10|-6  -5 \
                                        # edx (octant code)
neighborTable:
        .int            - xPitch        # -12
        .int    -yPitch - xPitch        # -11
        .int    -yPitch                 # -10
        .int    -yPitch - xPitch        #  -9
        .int              xPitch        #  -8
        .int    -yPitch + xPitch        #  -7
        .int    -yPitch                 #  -6
        .int    -yPitch + xPitch        #  -5
        .int            - xPitch        #  -4
        .int     yPitch - xPitch        #  -3
        .int     yPitch                 #  -2
        .int     yPitch - xPitch        #  -1

        .int              xPitch        #   0
        .int     yPitch + xPitch        #   1
        .int     yPitch                 #   2
        .int     yPitch + xPitch        #   3

quadrantTable:
        .byte   0x80                    # -12
        .byte   0x80                    # -11
        .byte   0xC0                    # -10
        .byte   0xC0                    #  -9
        .byte   0x00                    #  -8
        .byte   0x00                    #  -7
        .byte   0xC0                    #  -6
        .byte   0xC0                    #  -5
        .byte   0x80                    #  -4
        .byte   0x80                    #  -3
        .byte   0x40                    #  -2
        .byte   0x40                    #  -1
        .byte   0x00                    #   0
        .byte   0x00                    #   1
        .byte   0x40                    #   2
        .byte   0x40                    #   3

octantTable:
        .byte    0                      # -12
        .byte    0                      # -11
        .byte   -1                      # -10
        .byte   -1                      #  -9
        .byte   -1                      #  -8
        .byte   -1                      #  -7
        .byte    0                      #  -6
        .byte    0                      #  -5
        .byte   -1                      #  -4
        .byte   -1                      #  -3
        .byte    0                      #  -2
        .byte    0                      #  -1
        .byte    0                      #   0
        .byte    0                      #   1
        .byte   -1                      #   2
        .byte   -1                      #   3

## The reciprocal table is a 256-word table that for an index i gives
## 1/x as a U16.16, i.e. an unsigned integer with 16 bits to the
## right of the binary point. More accurately, it gives
##
##       min((int)round(65536.0 / (i + 0.5)), 0xFFFF)
##
## The idea is that an index i is obtained from the high 8 bits of a
## 12-bit number by truncating (i.e. just a right shift 4), so
## that index i represents the range [i .. i+1), which is i+0.5
## on average. The min operation prevents 16-bit overflow, which
## can only happen for i = 0, and which will never occur in
## practice because if the largest gradient component is that
## small, the gradient magnitude won't be above any reasonable
## noise threshold. But even if it happens, clamping to 0xFFFF is
## reasonable. The integer version of the above floating point
## expression that is actually used below gets i+0.5 by using one
## bit to the right of the binary point, so it steps i from 1 to
## 0x1FF. This means that the 65536.0 also has to be
## doubled. It's doubled again to get 0x40000 because we want 17
## bits of reciprocal so that we can then round off the result to
## the final 16 bits we want.
recipTable:
        .word   0xFFFF          # i = 0.5 will overflow, so just use the max value
        index = 3               # start at i = 1.5
        .rept   255             # step i from 1.5 to 255.5
          .word (0x40000 / index + 1) >> 1
          index = index + 2
        .endr

# 129-byte arctangent table covering the range 0 - 45 degrees and containing
# 5 bits of binary angle
atanTable:
        .byte    0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  3,  4,  4,  4,  5
        .byte    5,  5,  6,  6,  6,  7,  7,  7,  8,  8,  8,  8,  9,  9,  9, 10
        .byte   10, 10, 11, 11, 11, 11, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14
        .byte   15, 15, 15, 15, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 19
        .byte   19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 23
        .byte   23, 23, 23, 23, 24, 24, 24, 24, 25, 25, 25, 25, 25, 26, 26, 26
        .byte   26, 26, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 29, 29, 29, 29
        .byte   29, 29, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 32, 32, 32
        .byte   32, 32          # Table extended by 1 (to 130 bytes) to compensate for
                                # rounding error in reciprocal calculation.

.section .text

        ## _sobel_operator(int bound, uint8_t thresh, uint16_t *in, int16_t* out);
_sobel_operator:
        push    ebp
        mov     ebp, esp

        push    esi
        push    edi
        push    ebx

        sub     esp, 8

        ## Load arguments into registers

        ## Load bound on starting row
        mov     eax, [ebp + bound_sobel_param]

        ## We have to move destination registers to the ends of the next row
        ##
        ## | o | o | o | <- source comes from this row
        ## | o | Q | o | <- destination needs to write to this row,
        ## | o | o | o | plus one byte forward for 'Q' gradient value
        ## Load threshold into register
        pinsrw  mm6, [ebp + thresh], 0b00
        pshufw  mm6, mm6, 0b00000000
        pmullw  mm6, mm6
        psrlw   mm6, 10


        imul    eax, yPitch
        mov     esi, [ebp + inImg]
        add     esi, eax        # Move forward to bound-th row

        pcmpeqb mm7, mm7
        pandn   mm7, mm7

        mov     edi, [ebp + outImg]
        add     edi, yPitch     # Adjust destination pointer to second row
        add     edi, eax        # Move dest pointer forward by 'bound' rows

        # Actually only does from top row through third to bottom
        mov     ebx, 238
        # We start at the "bound" row, so do fewer
        sub     ebx, dword ptr[ebp+bound_sobel_param]

sobel_yLoop:
        ## 4 pixels processed each iteration (320 per row / 4 = 80 iterations)
        mov     ecx, 80
sobel_xLoop:
        ## Load rows into registers to save memory accesses
        movq    mm0, [esi + top]
        movq    mm1, [esi + mid]
        movq    mm2, [esi + bot]

        ## Fetch next row into L1 cache and adjust source pointer
        prefetch [esi + nxt]
        add     esi, 8

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
        psubw   mm0, mm2                # mm0 = | z7 | z6 | z5 | z4 |
        pshufw  mm2, mm4, 0b01001110    # mm2 = |  - |  - | z3 | z2 |
        punpckldq mm2, mm0              # mm2 = | z5 | z4 | z3 | z2 |
        paddw   mm2, mm0                # mm2 = | z5 + z7 | z4 + z6 | z3 + z5 | z2 + z4 |
        movq    mm5, mm4                # mm5 = | z3 | z2 |  - |  - |
        psrlq   mm5, 48                 # mm5 = |  0 |  0 |  0 | z3 |
        movq    mm4, mm0                # mm4 = | z7 | z6 |  - |  - | for next iteration
        psllq   mm0, 16                 # mm0 = | z6 | z5 | z4 |  0 |
        por     mm0, mm5                # mm0 = | z6 | z5 | z4 | z3 |
        paddw   mm0, mm0                # mm0 = | 2*z6 | 2*z5 | 2*z4 | 2*z3 |
        paddw   mm0, mm2                # mm0 = | z5 + 2*z6 + z7 | z4 + 2*z5 + z6 | z3 + 2*z4 + z5 | z2 + 2*z3 + z4 |
        movntq  [edi + yGrad], mm0

        ## MAGNITUDE calculation
        psllw   mm3, 3                  # x gradient becomes 16 bits signed
        psllw   mm0, 3                  # y gradient becomes 16 bits signed
        pmulhw  mm3, mm3                # x squared, 14 bits unsigned
        pmulhw  mm0, mm0                # y squared, 14 bits unsigned
        paddw   mm0, mm3                # magnitude squared

        # subtract noise threshold (mm6), force to 0 if below threshold
        psubusw mm0, mm6

        ## The first time we write out an MMX register, the first two
	## values are garbage. They are initialized with garbage data
	## so they are wrong.
        movntq  [edi + sqMag], mm0

        ## We're done with the destination pointer,
        ## so increment it to next line of pixels.
        add     edi, 8

        ## Set up x sums for next iteration
        # mm3 = | - | - | z7 | z6 |
        pshufw  mm3, mm1, 0b01001110

        dec     ecx
        jne     sobel_xLoop

        dec     ebx
        jne     sobel_yLoop

        ## Fix stack
        mov     esp, ebp
        sub     esp, 12

        pop     ebx
        pop     edi
        pop     esi

        pop     ebp

        emms

        ret

################################################
        ## Edge peak detection loop ##
################################################

.section        .text

        ## _find_edge_peaks(int bound, int16_t *gradients, uint16_t *output)
        ##
        ## General Registers:
        ##      eax     y gradient, then max(xGrad, yGrad), then binary angle
        ##      ebx     x gradient for a time, then min(xGrad, yGrad)
        ##      edx     ends up with octant bits
        ##      ecx     x index counter
        ##      edi     gradient values pointer
        ##      esi
        ##      ebp     offset to gradient direction neighbors for peak test
        ##
        ## Stack:
        ##      [esp + row_count]       y index counter
        ##      [esp + angles_ptr]      pointer to output (angles) array
        ##      [esp + num_peaks]       number of peaks found

_find_edge_peaks:
        ## Load output address
        push    ebp

        push    esi
        push    edi
        push    ebx

        mov     ecx, dword ptr[esp + bound_peak_param + 12]
        imul    ebx, ecx, yPitch # move forward by 'bound' rows

        mov     edi, dword ptr[esp + gradients_param + 12]
        add     edi, ebx

        ## Move foward to first usable gradient point
        ## This is 2 rows down (since row 0 has no gradient values and
        ## row 1 has no gradient values above it, so it can't be peak)
        ##
        ## 6 bytes in because first 2 bytes are garbage, second two bytes are
        ## "gradient" over pixel 0 (aka garbage), and bytes 5,6 are the
        ## gradient over pixel 1. We need a pixel to our left, so we
	## start by looking at the gradient across pixel 2.
        add     edi, 6 + yPitch
        mov     eax, dword ptr[esp + angles_param + 12]

        sub     esp, end_of_peak_stack
        mov     dword ptr[esp + bound_val], ecx
        mov     dword ptr[esp + angles_ptr], eax

        mov     dword ptr[esp + num_peaks], 0

        ## 240 rows, 238 gradient magnitudes, 236 possible peak rows
        mov     ebx, 237
        sub     ebx, dword ptr[esp + bound_val]
        mov     dword ptr[esp + row_count], ebx

        cld                             # search increasing addresses

peaks_yLoop:

        mov     ecx, 316                # 320 pixels, 318 gradient magnitudes,
                                        # 316 possible peak positions
peaks_xLoop:
        xor     eax, eax                # looking for non-zero
        repe scasw                      # search for non-zero squared magnitude,
                                        # or until end of row
        jz      peaks_xLoop_end         # end of row or non-zero value found?

############################################
### Functionally equivalent, but slower loop, here for explanation of
###      what is going on in 'repe scasw'
############################################
## above:  add     edi, 2
##         dec     ecx
##         jz      peaks_xLoop_end

##         cmp     eax, [edi-2]
##         je      above

	##
        ##
        ## CODE BELOW:
        ##     The commented out code below checks the field horizon to see if the
        ##     point it is checking is above the field horizon.
        ##
        ##

        ## ## Calculate address in field_edges table
        ## ## ecx goes 316 -> 0 and is one past index of the non-zero pixel
        ## ## plus it begins at index 2: thus we add 317
        ## ## so we want &field_edges - 4*ecx + 317*4 (this is a table of 32bit ints)
        ## imul    esi, ecx, -4
        ## add     esi, dword ptr[esp + field_edges_param + end_of_peak_stack + 12]
        ## mov     esi, dword ptr [esi + 317*4]

	## ## row count goes 237->2, and topEdge is from 0->239
        ## ## transform esi to be the same as the row_count
        ## imul    esi, -1
        ## add     esi, 239

        ## If the topEdge has a larger value (i.e. is below the current point
        ##    when you look at the image) then keep looping. We've
	## flipped all the values, however, so we compare looking for lower values
        ## now
        ## cmp     esi, dword ptr[esp + row_count]
        ## jb      peaks_xLoop     # below, not above!

        # Found next magnitude above noise threshold. Here
        #       edi     -> one word past the non-zero
        #                  (i.e. >= noise threshold) squared magnitude.
        #       ecx     count remaining in current row
        #               (x coordinate is offset from this value)
        prefetch [edi - xPitch]
        prefetch [edi - xPitch - yPitch]
        prefetch [edi - xPitch + yPitch]

        # Fetch y gradient, compute absolute value, save sign bit in edx
        movsx   eax, word ptr [edi - xPitch + yGrad]
        cdq
        xor     eax, edx
        sub     eax, edx

        # Fetch x gradient, compute absolute value, shift sign bit into edx
        movsx   ebx, word ptr [edi - xPitch + xGrad]
        mov     esi, ebx                # using cdq would require saving
                                        # eax and edx somewhere, so this
        sar     esi, 16                 # seems 1 tick faster
        xor     ebx, esi
        sub     ebx, esi
        add     edx, edx                # shift in sign bit
        add     edx, esi

        # eax = max(xGrad, yGrad) ;  ebx = min(xGrad, yGrad);
        # shift octant bit into edx
        cmp     ebx, eax
        cmova   esi, eax
        cmova   eax, ebx
        cmova   ebx, esi
        adc     edx, edx                # shift in octant bit

        # One more bit of gradient direction into edx
        add     ebx, ebx
        cmp     eax, ebx
        adc     edx, edx

        # Lookup offset of neighbors
        mov     ebp, dword ptr[neighborTable + edx*4 + neighborOffset]

        # Peak?
        movzx   esi, word ptr[edi + ebp - xPitch]

        # It's an asymmetric peak test
        # (neighbor-1 < peak < otherNeighbor) equiv (neighbor <= peak < otherNeighbor)
        # since we're using integers
        dec     esi
        neg     ebp             # Find opposite neighbor offset

        ## INTERLACED instructions (watch out, could get confusing,
        ##                                      don't hurt yourself)

        ## Yes, we have a new edge point. Compute gradient direction. x
        ## component is in eax, 12 bits unsigned. y component is in ebx,
        ## and has been multiplied by 2, so it's 13 bits
        ## unsigned. Tangent of angle, 0 - 45 deg, is y/2x. High 3 bits
        ## of binary angle are looked up from octant code in edx.
        shr     eax, 4

        ## Do peak comparisons
        cmp     si, word ptr[edi + ebp - xPitch]
        cmovb   si, word ptr[edi + ebp - xPitch]
        cmp     si, word ptr[edi - xPitch]
        ja      peaks_xLoop     # If curpix.mag > max(neighbor1.mag, neighbor2.mag)

        # lookup reciprocal, U16.16
        movzx   eax, word ptr [recipTable + eax*2]

        imul    eax, ebx                # y/x, U32.21
        shr     eax, 14                 # y/x, U32.7 (129-element table)

        # lookup arc tangent
        movzx   eax, byte ptr [atanTable + eax]

        # negate arc tangent for appropriate octants
        xor     al, byte ptr[octantTable + edx + octantOffset]
        sub     al, byte ptr[octantTable + edx + octantOffset]

        # get 8-bit binary angle
        add     al, byte ptr[quadrantTable + edx + quadrantOffset]

        # Write binary angle (in a, also eax),
        # x coord (from ecx), y coord (on stack)

        ## edx, esi are no longer needed so we use them as temp registers
        ## Load in pointer to angles array
        mov     esi, [esp + angles_ptr]

        ## Write out 16 bits of binary (actually only 8 bit) angle
        mov     word ptr[esi], ax

        ## Write out x coordinate
        ## 316 >= ecx >= 0, should write out 2 --> 318, ecx is one
        ## past the last non-zero
        ## for instance, on first pixel in image,
        ## ecx will be at 315 (not 316 still, since it gets decremented once
        ## in the 'repe scasw' instruction)

        ## Calculate the coordinate relative to the image center, also
        mov     edx, 317 - imgWd/2
        sub     edx, ecx
        mov     word ptr[esi + 2], dx

        ## Load y coordinate and write it out
        ## Calculate the coordinate relative to the image center
        mov     edx, 238 - imgHt/2
        sub     edx, dword ptr[esp + row_count]

        ## 236 >= row_count >= 0, must write out starting from 2 --> 238
        mov     word ptr[esi + 4], dx

        ## Move angles ptr forward to the next location in the array
        add     dword ptr[esp + angles_ptr], 6

        inc     dword ptr[esp + num_peaks]

        jmp     peaks_xLoop

peaks_xLoop_end:
        add     edi, 8          # Advance to beginning of next row
        dec     dword ptr[esp + row_count]
        jne     peaks_yLoop

        ## Load in pointer to angles array
        mov     esi, [esp + angles_ptr]

        ## Put zeros in the x,y, angle spots for the element after the
        ## last in the list of edge peaks. Signals end of list to user.
        mov     word ptr[esi], 0
        mov     word ptr[esi+2], 0 - imgWd/2 # Relative to image center
        mov     word ptr[esi+4], 0 - imgHt/2

        mov     eax, [esp + num_peaks]

        ## Clean up and return
        add     esp, end_of_peak_stack

        pop    ebx
        pop    edi
        pop    esi

        pop    ebp

        emms

        ret
