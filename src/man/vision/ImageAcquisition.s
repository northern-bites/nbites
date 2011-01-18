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

.macro COPY_SHIFT_R dest, source

	movq	\dest, \source
	psrld	\dest, 2
.endm

## Inner loop of the color processing
.macro COLOR_LOOP phase

	## Load the color address from the stack
	mov	eax, dword ptr[esp+1280 + 8 +ecx*4 + \phase * 4]

	## Lookup color in table
        movzx   eax, byte ptr[ebx+eax]          # movzx may be faster than
	                                        # just moving a byte to al
        mov     word ptr[edi+ecx*2+\phase*2+(320*240*2)], ax
				# color image is just after y image in memory,
				# so displacement is (320*240*2)
				# since Y and color values 16 bits each
.endm

	## The inner loop of the image copy, averaging, and color segmentation
	## Performs all the averaging, and calculates all the table indices, but
	## leaves the table lookups to another loop.
.macro LOOP phase
        # Fetch next 8 pixels from upper (0) source row, split into y and uv words
        # mm0:  | y30 | y20 | y10 | y00 |
        # mm1:  | v20 | u20 | v00 | u00 |
	movq    mm0, [esi+ecx*4 + (\phase * 8)]
        movq    mm1, mm0
        pand    mm0, mm7
        psrlw  	mm1, 8

        # Fetch next 8 pixels from lower (1) source row, split into y and uv words
        # mm2:  | y31 | y21 | y11 | y01 |
        # mm3:  | v21 | u21 | v01 | u01 |
        movq    mm2, [esi+ecx*4+ (\phase * 8)+(640*2)]	# row = 640 pixels * 2 bytes per pixel
        movq    mm3, mm2
        pand    mm2, mm7
        psrlw 	mm3, 8

	##
	##
	## Y AVERAGING SECTION
	##
	##

        # Sum 2x2 y values (words, 10 bits used)
        # mm0: | xxx | y20 + y30 + y21 + y31 | xxx | y00 + y10 + y01 + y11 |
        paddw   mm0, mm2
        pshufw  mm2, mm0, 0xB1
        paddw   mm0, mm2

	## mm0: | 0000 | sum1 | 0000 | sum0 |
	pand	mm0, mm6

	## COPY_SHIFT_R used to copy into various MMX registers and
	## divide the Y sum by 4 to get the average.
	## COPY_SHIFT_R avoids copying into mm2 register unless necessary, so
	## phase 0 and 2 just skip mm2 register copy.
	.ifeq (\phase)
	COPY_SHIFT_R mm4, mm0
	.endif


	.ifeq (\phase - 1)
	COPY_SHIFT_R mm2, mm0

	## Pack values from first two phases together as 4 words in mm4
	## mm4 after pack: | y3 | y2 | y1 | y0 |
	packssdw mm4, mm2
	movntq [edi+ecx*2], mm4
	.endif

	.ifeq (\phase - 2)
	COPY_SHIFT_R mm5, mm0
	.endif

	## Last phase, pack phase 2 & 3 into an array, then pack all 8
	## bytes together and write them out
	.ifeq (\phase - 3)
	COPY_SHIFT_R mm2, mm0

	## mm2 before: | 0 | y7 | 0 | y6|
	## mm5 after:  | y7 | y6 | y5 | y4 | all 16 bit words
	packssdw mm5, mm2
	movntq [edi+ecx*2+8], mm5
	.endif

	##
	##
	## UV-COLOR SECTION
	##
	##

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

	## Write color address for 2 pixels to the stack, we'll look
	## it up in the table later
	movq	[esp + 1280 + 8 + ecx*4 + \phase * 8], mm0
.endm

# acquire_image arguments ( byte* colorTable, ColorParams* params, byte* yuvImage, byte* outputImage)
_acquire_image:

# Preserve the required registers: ebp, ebx, esi, edi
	push	ebp
	mov	ebp, esp

	push	ebx
	push	esi
	push	edi

	# Move stack pointer and push rowCount to it
	sub 	esp, 1280 + 8 	# 4 bytes for rdtsc, 4 for rowCount
				# 320 * 4 bytes per color addresses

	# Ensure that the stack pointer is 8 byte aligned
	and	esp, 0xFFFFFFF8

	mov 	dword ptr[esp], 240	# Row Count is only 240 bc it skips every other row.
	rdtsc
	mov	[esp+4],eax

	# Load arguments into registers
	mov	ebx, [ebp+8]	# Color table *
	mov	edx, [ebp+12]	# Color params *

	mov	esi, [ebp+16]	# Input YUV Image address
	add	esi, 640*2	# Move to end of source image row

	mov	edi, [ebp+20]	# Output (color segmented) image address
	add	edi, 320*2	# Move to end of y row, 640 = 320 pixels * 2 bytes per Y value

        # set mm7 to 0x00FF00FF00FF00FF for y pixel mask
        pcmpeqb	mm7, mm7                        # set to all 1s
        psrlw	mm7, 8                          # set high 8 bits of each word to 0

	## set mm6 to 0x0000FFFF0000FFFF for y packing mask
	pcmpeqb	mm6, mm6
	psrldw 	mm6, 16

# Start of outer (y) loop
yLoop:
        mov     ecx, -320                       # x loop count

# Start of inner (x) loop
#
# Processes 8 pixels total in each xLoop.
# Y Values get written to memory, color values are written to the stack then processed.
xLoop:
	## Calculate the offset for the pixel accesses
	# ecx * 2 bytes/pixel * 2 pixels (-320 <= ecx< 0)
	# Only perform once since ecx only changes at the end of the loop
	LOOP 0
	LOOP 1
	LOOP 2
	LOOP 3

        # Update loop/offset counter, test for end of row
        add     ecx, 8
        jne     xLoop



## Lookup and write color pixels to output image
## Loop from 0 to 320 in ecx
	mov	ecx, -320
colorLoop:
	## Load pixel color address
	COLOR_LOOP 0
	COLOR_LOOP 1
	COLOR_LOOP 2
	COLOR_LOOP 3
	COLOR_LOOP 4
	COLOR_LOOP 5
	COLOR_LOOP 6
	COLOR_LOOP 7
	COLOR_LOOP 8
	COLOR_LOOP 9

	add	ecx, 10
	jne	colorLoop

        # End of x loop. Decrement outer loop counter, update row pointers,
        # test for end of image
        add     esi, 640*2*2                    # next source rows
        add     edi, 320*2                      # next output rows
        dec     dword ptr[esp]
        jne     yLoop

	rdtsc
	sub	eax, [esp+4]

	# Restore esp to original position
	mov 	esp, ebp

	# We still need to pop 3 bytes from the stack
	sub 	esp, 12

	# Restore necessary _cdecl calling convention registers
	pop	edi
	pop	esi
	pop	ebx

	# Restore ebp
	pop 	ebp

	emms

	ret
