/* -*- mode: asm; indent-tabs-mode: nil -*- */
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

.globl _acquire_image_fast

.section .data

.equiv INIT_LOOP_COUNT, -320
.equiv OUT_IMG_WIDTH, 320
.equiv OUT_IMG_HEIGHT, 240
.equiv CHANNEL_IMG_BYTE_SIZE, OUT_IMG_WIDTH * OUT_IMG_HEIGHT * 2


        ## Output image layout
        .struct 0
yImg:   .skip OUT_IMG_WIDTH * OUT_IMG_HEIGHT * 2 # 16 bit Y values
uImg:  .skip OUT_IMG_WIDTH * OUT_IMG_HEIGHT * 2 # 16 bit U values
vImg:  .skip OUT_IMG_WIDTH * OUT_IMG_HEIGHT * 2 # 16 bit V values
colorImg:

## STACK STRUCTURE, esp offsets
        .struct 0
row_count:      .skip 4
rdtsc_output:   .skip 4
y_out_img:      .skip 4
color_out_img:  .skip 1280
                .skip 4         # make color_out_row qword aligned
color_stack_row_end:

.equiv end_of_stack, color_stack_row_end

        ## ColorParam structure
        .struct 0
yZero:   .skip 8
ySlope:  .skip 8
yLimit:  .skip 8

uvZero:  .skip 8
uvSlope: .skip 8
uvLimit: .skip 8
uvDim:   .skip 8

.section .text

## Inner loop of the color processing
.macro COLOR_LOOP phase
        ## Load the color address from the stack
        mov     eax, dword ptr[esp+ color_stack_row_end +ecx*4 + \phase * 4]

        ## Lookup color in table
        movzx   eax, byte ptr[ebx+eax]          # movzx may be faster than
                                                # just moving a byte to al
        mov     byte ptr[edi+ecx+\phase], al
                                # color image is just after y image in memory,
                                # Y values are 16 bits, color are bytes
.endm

        ## The inner loop of the image copy, averaging, and color segmentation
        ## Performs all the averaging, and calculates all the table indices, but
        ## leaves the table lookups to another loop.
.macro LOOP phase
        ## Prefetch the next 32 bytes of image (only useful when image is cacheable)
        ## Each "phase" loads 8 bytes, so we only need to prefetch once every 4 phases

        .ifeq (\phase)
        ## NOT USEFUL WHEN USING UNCACHEABLE CAMERA BUFFER
        ## prefetch [esi+ecx*4+64]

        ## Prefetch writes to the output images
        prefetchw [edi+ ecx*2 + 8 *((\phase-1)/2)]
        prefetchw [edi+ ecx*2 + 8 *((\phase-1)/2) + uImg]
        prefetchw [edi+ ecx*2 + 8 *((\phase-1)/2) + vImg]
        .endif

        # Fetch next 8 pixels from upper (0) source row, split into y and uv words
        # mm0:  | y30 | y20 | y10 | y00 |
        # mm1:  | v20 | u20 | v00 | u00 |
        movq    mm0, [esi+ecx*4 + (\phase * 8)]
        movq    mm1, mm0

        pand    mm0, mm7
        psrlw   mm1, 8

   #######
 #########
########## Y AVERAGING SECTION
 #########
   #######

        # Sum 2 y values (words, 9 bits used)
        # mm0: | xxx | y20 + y30 | xxx | y00 + y10 |
        pshufw  mm2, mm0, 0xB1
        paddw   mm0, mm2

        ## mm0: | 0000 | sum1 | 0000 | sum0 |
        pand    mm0, mm6

        .ifeq (\phase)
        .endif

########################## FIRST PHASES (0,2)
        .if (\phase == 0 || \phase == 2)
        ## Copy the Y values for later packing
        movq    mm3, mm0

        ## Store U values
        ## Zero out the top half of each doubleword, so
        ## mm4:  | 0000 | u20 | 0000 | u00 |
        movq    mm4, mm1
        pand    mm4, mm6

        ## Store V Values
        ## Move V values to bottom of each doubleword
        ## mm4:  | 0000 | v20 | 0000 | v00 |
        movq    mm5, mm1
        psrld   mm5, 16
        .endif

########################## SECOND PHASES (1,3)
        .if (\phase == 1 || \phase == 3)

        ## Pack values from first two phases together as 4 words in mm3
        ## mm3 after pack: | y3 | y2 | y1 | y0 |
        packssdw mm3, mm0

        ## First write out goes at the pointer, next goes 8 bytes later
        movntq [edi+ ecx*2 + 8 *((\phase-1)/2) + yImg], mm3

        ## Extract U values and write
        movq    mm2, mm1
        pand    mm2, mm6
        packssdw mm4, mm2

        ## ## mm2: |v20 | v20 | v00 | v00 |
        movq    mm2, mm1
        psrld   mm2, 16
        packssdw mm5, mm2

        movntq  [edi+ ecx*2 + 8 *((\phase-1)/2) + uImg], mm4
        movntq  [edi+ ecx*2 + 8 *((\phase-1)/2) + vImg], mm5

        .endif


   #######
 #########
########## COLOR SECTION
 #########
   #######

        # Convert two y sums in words 0 and 2 to two table indicies
        psubusw mm0, [edx + yZero]      # zero point
        pmulhw mm0, [edx + ySlope]                  # slope
        pminsw  mm0, [edx + yLimit]                 # limit to maximum value

        # Convert four u,v sums to four table indicies
        psubusw mm1, [edx + uvZero]                 # zero point
        pmulhw mm1, [edx + uvSlope]                 # slope
        psllw   mm1, 1
        pminsw  mm1, [edx + uvLimit]                # limit to maximum value

        # Calculate two table offsets from y, u, and v table indicies (dwords)
        # mm0: | table offset 1 | table offset 0 |
        pmaddwd mm1, [edx + uvDim]                  # combine u and v indicies
        paddd   mm0, mm1                            # add y index

        ## Write color address for 2 pixels to the stack, we'll look
        ## it up in the table later
        movq    [esp + color_stack_row_end + ecx*4 + \phase * 8], mm0
.endm

# acquire_image arguments ( byte* colorTable, ColorParams* params, byte* yuvImage, byte* outputImage)
_acquire_image_fast:

# Preserve the required registers: ebp, ebx, esi, edi
        push    ebp
        mov     ebp, esp

        push    ebx
        push    esi
        push    edi

        # Move stack pointer and push rowCount to it
        sub     esp, end_of_stack       # 4 bytes for rdtsc, 4 for rowCount
                                        # 320 * 4 bytes per color addresses

        # Ensure that the stack pointer is 8 byte aligned
        and     esp, 0xFFFFFFF8

        mov     dword ptr[esp + row_count], OUT_IMG_HEIGHT      # Row Count is only 240 bc it skips every other row.
        rdtsc
        mov     [esp + rdtsc_output],eax

        # Load arguments into registers
        mov     ebx, [ebp+8]    # Color table
        mov     edx, [ebp+12]   # Color params

        mov     esi, [ebp+16]   # Input YUV Image address
        add     esi, 640*2      # Move to end of source image row

        mov     edi, [ebp+20]   # Output image address
        add     edi, OUT_IMG_WIDTH*2    # Move to end of y row, 640 = 320 pixels * 2 bytes per Y value

        ## Put end of row pointers on stack
        mov     [esp + y_out_img], edi

        ## Set color image pointer forward entire y image, and uv image, then back half a y-image row
        mov     edi, [esp + y_out_img]
        add     edi, colorImg - OUT_IMG_WIDTH
        mov     [esp + color_out_img], edi

        # set mm7 to 0x00FF00FF00FF00FF for y pixel mask
        pcmpeqb mm7, mm7                        # set to all 1s
        psrlw   mm7, 8                          # set high 8 bits of each word to 0

        ## set mm6 to 0x0000FFFF0000FFFF for y packing mask
        pcmpeqb mm6, mm6
        psrldw  mm6, 16

# Start of outer (y) loop
yLoop:
        mov     ecx, INIT_LOOP_COUNT                       # x loop count
        mov     edi, [esp + y_out_img]

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

        mov     edi, [esp + color_out_img]

## Lookup and write color pixels to output image
## Loop from 0 to 320 in ecx
        mov     ecx, INIT_LOOP_COUNT
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

        add     ecx, 10
        jne     colorLoop



        # End of x loop. Decrement outer loop counter, update row pointers,
        # test for end of image
        add     esi, 640*2*2                    # next source rows

        ## Move pointers to end of next output image rows
        add     dword ptr[esp + y_out_img], OUT_IMG_WIDTH * 2
        add     dword ptr[esp + color_out_img], OUT_IMG_WIDTH

        dec     dword ptr[esp + row_count]
        jne     yLoop

        rdtsc
        sub     eax, [esp + rdtsc_output]

        # Restore esp to original position
        mov     esp, ebp

        # We still need to pop 3 bytes from the stack
        sub     esp, 12

        # Restore necessary _cdecl calling convention registers
        pop     edi
        pop     esi
        pop     ebx

        # Restore ebp
        pop     ebp

        emms

        ret
