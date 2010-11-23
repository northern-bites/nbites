.intel_syntax noprefix
# *********************************
# *                               *
# *  Image Acquisition Front-End  *
# *                               *
# *********************************
#
#  General registers
#       eax     working register
#       ebx     address of color conversion table
#       ecx     loop counter and offset into source and destination rows
#                 for (ecx = -320; ecx < 0; ecx += 2)
#       edx     address of struct ColorParams, defined below
#       esi     address of end of current yuv source image row
#       edi     address of end of current output image row in the y value section
#
#  The mapping from y pixel values to y table indicies is:
#
#            y index
#               |             _____________
#       (yn-1) -|            /
#               |           /
#               |          /
#               |         /
#            0 -| _______/
#                -----------------------------  y
#                 |      |    |            |
#                 0     y0   y1           255
#
# The mapping for u and v is similar. The values yn, un, and vn give the table size
# in each of the three dimensions. The total table size is yn * un * vn. These values
# need not be powers of 2. The slopes of the active segment of the maps are given by
#
#       yk = (yn << 14) / (y1 - y0)
#       uk = (un << 15) / (u1 - u0)
#       vk = (vn << 15) / (v1 - v0)
#

#  Parameters structure:
#
#    struct ColorParams
#    {                     // words:      3         2         1         0
#      Int64  yZero;              // | y0 << 2 | y0 << 2 | y0 << 2 | y0 << 2 |
#      Int64  ySlope;             // |    0    |   yk    |    0    |   yk    |
#      Int64  yLimit;             // |  yn-1   |  yn-1   |  yn-1   |  yn-1   |

#      Int64  uvZero;             // | v0 << 1 | u0 << 1 | v0 << 1 | u0 << 1 |
#      Int64  uvSlope;            // |   vk    |   uk    |   vk    |   uk    |
#      Int64  uvLimit;            // |  vn-1   |  un-1   |  vn-1   |  un-1   |
#      Int64  uvDim;              // | un * yn |   yn    | un * yn |   yn    |
#    };

.globl _acquire_image


# acquire_image arguments ( byte* colorTable, ColorParams* params, byte* yuvImage, byte* outputImage)
_acquire_image:

# Preserve the required registers: ebp, ebx, esi, edi
	push	ebp
	mov	ebp, esp

	push	ebx
	push	esi
	push	edi

	# Move stack pointer and push rowCount to it
	sub 	esp, 4
	mov 	dword ptr[esp], 240	# Row Count is only 240 bc it skips every other row.

	# Load arguments into registers
	mov	ebx, [ebp+8]	# Color table *
	mov	edx, [ebp+12]	# Color params *

	mov	esi, [ebp+16]	# Input YUV Image address
	add	esi, 640*2	# Move to end of source image row

	mov	edi, [ebp+20]	# Output (color segmented) image address
	add	edi, 320	# Move to end of y row

        # set mm7 to 0x00FF00FF00FF00FF for y pixel mask
        pcmpeqb	mm7, mm7                        # set to all 1s
        psrlw	mm7, 8                          # set high 8 bits of each word to 0

# Start of outer (y) loop
yLoop:
        mov     ecx, -320                       # x loop count

# Start of inner (x) loop
#
# Source image pixels in memory, processed by each iteration:
#
#   y00  u00  y10  v00  y20  u20  y30  v20
#   y01  u01  y11  v01  y21  u21  y31  v21

        # Fetch next 8 pixels from upper (0) source row, split into y and uv words
        # mm0:  | y30 | y20 | y10 | y00 |
        # mm1:  | v20 | u20 | v00 | u00 |
xLoop:  movq    mm0, [esi+ecx*4]	# ecx * 2 bytes/pixel * 2 pixels (-320 <= ecx< 0)
        movq    mm1, mm0
        pand    mm0, mm7
        psrlw  	mm1, 8

        # Fetch next 8 pixels from lower (1) source row, split into y and uv words
        # mm2:  | y31 | y21 | y11 | y01 |
        # mm3:  | v21 | u21 | v01 | u01 |
        movq    mm2, [esi+ecx*4+(640*2)]	# row = 640 pixels * 2 bytes per pixel
        movq    mm3, mm2
        pand    mm2, mm7
        psrlw 	mm3, 8

        # Sum 2x2 y values (words, 10 bits used)
        # mm0: | xxx | y20 + y30 + y21 + y31 | xxx | y00 + y10 + y01 + y11 |
        paddw   mm0, mm2
        pshufw  mm2, mm0, 0xB1
        paddw   mm0, mm2

        # Divide y sums by 4 to get 8 bits, write 2 y pixels to output y image
        movq    mm2, mm0                        # make a copy of the y sums
        psrld  mm2, 2                          # unsigned divide by 4
        pextrw  eax, mm2, 0                     # get and write low pixel
        mov     [edi+ecx], al
        pextrw  eax, mm2, 2                     # get and write high pixel
        mov     [edi+ecx+1], al

        # Sum 1x2 uv values (words, 9 bits used)
        # mm1: | v20 + v21 | u20 + u21 | v00 + v01 | u00 + u01 |
        paddw   mm1, mm3

        # Convert two y sums in words 0 and 2 to two table indicies
        psubusw mm0, [edx]                      # zero point
        pmulhw mm0, [edx+8]                    # slope
        pminsw  mm0, [edx+16]                   # limit to maximum value

        # Convert four u,v sums to four table indicies
        psubusw mm1, [edx+24]                   # zero point
        pmulhw mm1, [edx+32]                   # slope
        pminsw  mm1, [edx+40]                   # limit to maximum value

        # Calculate two table offsets from y, u, and v table indicies (dwords)
        # mm0: | table offset 1 | table offset 0 |
        pmaddwd mm1, [edx+48]                   # combine u and v indicies
        paddd   mm0, mm1                        # add y index

        # Lookup color pixel 0 in table, write to output image
        pextrw  eax, mm0, 0
        movzx   eax, byte ptr[ebx+eax]          # movzx may be faster than
	                                        # just moving a byte to al
        mov     [edi+ecx+(320*240)], al         # color image is just after y image in memory,
                                                # so displacement is (320*240)

        # Lookup color pixel 1 in table, write to output image
        pextrw  eax, mm0, 2
        movzx   eax, byte ptr[ebx+eax]
        mov     [edi+ecx+(320*240 + 1)], al

        # Update loop/offset counter, test for end of row
        add     ecx, 2
        jne     xLoop

        # End of x loop. Decrement outer loop counter, update row pointers,
        # test for end of image
        add     esi, 640*2*2                    # next source rows
        add     edi, 320                        # next output rows
        dec     dword ptr[esp]
        jne     yLoop

	# Restore esp to on top of rowCount
	add	esp, 4

	# Restore necessary _cdecl calling convention registers
	pop	edi
	pop	esi
	pop	ebx

	# Restore ebp
	pop 	ebp

	ret
