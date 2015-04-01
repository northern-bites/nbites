/* -*- mode: asm; indent-tabs-mode: nil -*- */
.intel_syntax noprefix
# **************************************
# *                                    *
# *  Image Acquisition Pre-processing  *
# *                                    *
# **************************************
#
#  Takes as input a YUV image and its dimentions and row pitch, outputs
#  - 16 bit Y image
#  - 8 bit white image
#  - 8 bit orange image
#  - 8 bit green image
#
#  General registers
#       eax     working register
#       ebx     row pitch
#       ecx     loop counter and offset into source and destination rows
#                 for (ecx = -cols; ecx < 0; ecx += 2)
#       edx     address of end of current White output image row
#       esi     address of end of current yuv source image row
#       edi     address of end of current Y output image row

.globl _acquire_image

.section .data


## STACK STRUCTURE, esp offsets
        .struct 0
row_count:      .skip 4        
col_count:	.skip 4         
row_pitch:	.skip 4
out_row_count:  .skip 4         # out_row_count = row_count/2 
rdtsc_output:   .skip 4
y_out_img:      .skip 4
output_size:    .skip 4
stack_end:

## ARGUMENT OFFSETS, ebp offsets
        .struct 0
saved_ebp:      .skip 4
ret_address:    .skip 4
int_rows:       .skip 4
int_cols:       .skip 4
int_rowPitch:   .skip 4
ptr_yuvImage:   .skip 4
ptr_output:     .skip 4

.section .text

   #
   ##
###### 
####### INNER LOOP
######
   ##       Executes averaging and image copying for 8 pixels.
   #        Called four times per inner loop. 
#
#
#    
#
#
.macro LOOP phase

####### IF PHASE 0, prefetch next 32 bytes of source image and writes to output (to be tested)
# TODO: PREFETCHING
 #       .ifeq (\phase)
  #      prefetch [eax+64] 
   #     prefetchw [edi + ecx*2]
    #    prefetchw [edx+ ecx*2]
     #   .endif
   
####### Fetch next 16 bytes from source row and split into Y and UV words
      #  Starting format: |vA3,yA7|uA3,yA6|vA3,yA5|uA2,yA4|vA1,yA3|uA1,yA2|vA0,yA1|uA0,yA0|
        movdqu  xmm0, [eax + (\phase * 16)]   #eax == esi + ecx*4.                      # should be movdqa? i.e. add alignment checking?
        movdqu  xmm1, xmm0            # xmm0: xmm1: |vA3,yA7|uA3,yA6|vA3,yA5|uA2,yA4||vA1,yA3|uA1,yA2|vA0,yA1|uA0,yA0|
        pand    xmm0, xmm7                  # xmm0: |    yA7|    yA6|    yA5|    yA4||    yA3|    yA2|    yA1|    yA0|  
        psrlw   xmm1, 8                     # xmm1: |    vA3|    uA3|    vA2|    uA2||    vA1|    uA1|    vA0|    uA0|
       
      #  Fetch from from the next row down. 
	    movdqu	xmm2, [eax + ebx*4 + (\phase * 16)] #eax == esi + ecx*4. ebx == row pitch.
        movdqu  xmm3, xmm2            # xmm2: xmm3: |vB3,yB7|uB3,yB6|vB3,yB5|uB2,yB4||vB1,yB3|uB1,yB2|vB0,yB1|uB0,yB0|
        pand    xmm2, xmm7                  # xmm2: |    yB7|    yB6|    yB5|    yB4||    yB3|    yB2|    yB1|    yB0|  
        psrlw   xmm3, 8                     # xmm3: |    vB3|    uB3|    vB2|    uB2||    vB1|    uB1|    vB0|    uB0|
		
      #  First and second Y sums
        paddw   xmm0, xmm2                  # xmm0: | A7+B7 | A6+B6 | A5+B5 | A4+B4 || A3+B3 | A2+B2 | A1+B1 | A0+B0 |     (all Ys)
        pshufhw xmm2, xmm0, 0b01001110      # xmm2: | A6+B6 | A7+B7 | A4+B4 | A5+B5 || A3+B3 | A2+B2 | A1+B1 | A0+B0 |     (all Ys)
        pshufhw xmm2, xmm2, 0b01001110      # xmm2: | A6+B6 | A7+B7 | A4+B4 | A5+B5 || A2+B2 | A3+B3 | A0+B0 | A1+B1 |     (all Ys)
        paddw   xmm0, xmm2                  # xmm2: |A67+B67|A67+B67|A45+B45|A45+B45||A23+B23|A23+B23|A01+B01|A01+B01|     (all Ys)
        psrld   xmm0, 16                    # xmm2: |   |A6+A7+B6+B7|   |A4+A5+B4+B5||   |A2+A3+B2+B3|   |A0+B0+A1+B1|     (all Ys)

      # U and V average
        paddw   xmm1, xmm3                  # xmm1: |vA3+vB3|uA3+uB3|vA2+vB2|uA2+uB2||vA1+vB1|uA1+uB1|vA0+vB0|uA0+uB0|
        psrlw   xmm1, 1                     # xmm1: |   v3  |   u3  |    v2 |   u2  ||   v1  |   u1  |   v0  |   u0  |
       
####### IF PHASE 0: Store
        .if (\phase == 0)
        movdqu  xmm4, xmm0                  # xmm4: |   |A6+A7+B6+B7|   |A4+A5+B4+B5||   |A2+A3+B2+B3|   |A0+B0+A1+B1|     (all Ys)
        packsswb    xmm5, xmm1              # xmm5: | v3 u3 | v2 u2 | v1 u1 | v0 u0 ||       |       |       |       |
        .endif

####### IF PHASE 1: Pack and Write Y, Pack UV values into xmm5
        .if (\phase == 1)
        packssdw xmm4, xmm0                 # xmm0: |A&B 6&7|A&B 4&5|A&B 3&2|A&B 0&1||C&D 6&7|C&D 4&5|C&D 2&3|C&D 0&1|     (all Ys)
        movdqu [edi + ecx*2 + (\phase-1)*8], xmm4      ## Non-temporal instruction causing seg fault???
        
        packsswb    xmm1, xmm1              # xmm1: | v7 u7 | v6 u6 | v5 u5 | v4 u4 || v7 u7 | v6 u6 | v5 u5 | v4 u4 |
        psrldq      xmm1, 64                # xmm1: |       |       |       |       || v7 u7 | v6 u6 | v5 u5 | v4 u4 |
        pand        xmm5, xmm1              # xmm1: | v3 u3 | v2 u2 | v1 u1 | v0 u0 || v7 u7 | v6 u6 | v5 u5 | v4 u4 |
        .endif

####### IF PHASE 2: Store
        .if (\phase == 2)
        movdqu      xmm4, xmm0                  # xmm4: |   |A6+A7+B6+B7|   |A4+A5+B4+B5||   |A2+A3+B2+B3|   |A0+B0+A1+B1|     (all Ys)
        packsswb    xmm6, xmm1              # xmm6: | vB uB | vA uA | v9 u9 | v8 u8 ||       |       |       |       |
        .endif

####### IF PHASE 3: Pack and Wirte Y, Pack UV values in xmm6 then separate V and U vals into four registers
        .if (\phase == 3)
        packssdw xmm4, xmm0                 # xmm4: |A&B 6&7|A&B 4&5|A&B 3&2|A&B 0&1||C&D 6&7|C&D 4&5|C&D 2&3|C&D 0&1|     (all Ys)
        movdqu [edi + ecx*2 + (\phase-1)*8], xmm4      ## Non-temporal instruction causing seg fault???

        # Pack and unpack for U and V registers
        packsswb    xmm1, xmm1              # xmm1: | vF uF | vE uE | vD uD | vC uC || vF uF | vE uE | vD uD | vC uC |
        psrldq      xmm1, 64                # xmm1: |       |       |       |       || vF uF | vE uE | vD uD | vC uC |      
        por        xmm6, xmm1              # xmm6: | vB uB | vA uA | v9 u9 | v8 u8 || vF uF | vE uE | vD uD | vC uC |

        movdqu      xmm0, xmm6

        movdqu      xmm3, xmm5              
        psrlw       xmm5, 8                 # xmm5: |   v3  |   v2  |   v1  |   v0  ||   v7  |   v6  |   v5  |   v4  |
        pand        xmm3, xmm7              # xmm3: |   u3  |   u2  |   u1  |   u0  ||   u7  |   u6  |   u5  |   u4  |
  
        movdqu      xmm4, xmm6
        psrlw       xmm6, 8                 # xmm6: |   vB  |   vA  |   v9  |   v8  ||   vF  |   vE  |   vD  |   vC  |  
        pand        xmm4, xmm7              # xmm4: |   uB  |   uA  |   u9  |   u8  ||   uF  |   uE  |   uD  |   uC  |


        # xmm0 to be t0 replicated 8 times
        # xmm1 to be w replicated 8 times
        # xmm2 to be w1 replicated 8 times

        # TODO: White, orange, and green calcs using registers xmm0-6

        # for now, just writes white and black images to test addressing
        #  addressing works but isn't the fastest
       # pcmpeqd xmm0, xmm0
        movdqu  [edx + ecx], xmm0

        psrlw   xmm0, 16
        sub     edx, [esp + output_size]
        movdqu  [edx + ecx], xmm0

        pcmpeqd xmm0, xmm0
        sub     edx, [esp + output_size]
        movdqu  [edx + ecx], xmm0

        # reset edx back to white image
        add     edx, [esp + output_size]
        add     edx, [esp + output_size]          # can't lea beacuse output size is kept on stack. thoughts?

        .endif
.endm

   #
   ##
######
####### _acquire_image (int row_count, 
######		        int col_count,
   ##			    int row_pitch,			
   #			    byte* yuvImage,
#			        byte* outputImage)
_acquire_image:

# Preserve required registers: epb, ebx, esi, edi
        push    ebp
        mov     ebp, esp    #ebp now contains address of top of stack
        push    ebx
        push    esi
        push    edi

        # Move stack pointer to allocate space for parameters
        sub     esp, stack_end
        and     esp, 0xFFFFFFF8         #clear low-order three bits to ensure sp is qw aligned

        # Preserve stamp counter
        rdtsc
        mov [esp + rdtsc_output], eax

# Read and manage arguments  
        # Load arguments into registers and stack 
        mov     eax, [ebp + int_rows]
        mov     [esp + row_count], eax          # set rows

        mov     [esp + out_row_count], eax      # set row count

        mov     eax, [ebp + int_cols]
        mov     [esp + col_count], eax

        mov     esi, [ebp + ptr_yuvImage]       # set esi to start of yuv input image
        lea     esi, [esi + eax * 4]            # move esi to end of first row

        mov     edi, [ebp + ptr_output]
        lea     edi, [edi + eax * 2]

        mov     ebx, [ebp + int_rowPitch]
        mov     [esp + row_pitch], ebx   
        
        # Put end-of-output-row pointer on stack
        mov     [esp + y_out_img], edi

        # Set output UV image to one output image more than the current edi
        imul    eax, [esp + row_count]
        lea     edx, [edi + eax * 2]
        sub     edx, [esp + col_count]
        imul    eax, -1
        mov    [esp + output_size], eax
        
       
# Set pixel masks  # TODO: masks
        pcmpeqd xmm7, xmm7        # set to all 1s
        psrlw   xmm7, 8           # set high 8 bits of each word to 0 

## Start of outer loop (y loop)
yLoop:
        mov     ecx, [esp + col_count]   # ecx = -col_count
        imul    ecx, -1

## Start of inner loop (x loop)

# Processes 8 pixels (bytes) in two rows (16 total) in each LOOP call (64 per xLoop).
# Y values, white, orange, and green values are written to memory
xLoop:
        lea     eax, [esi+ecx*4]

        LOOP 0
        LOOP 1
        LOOP 2
        LOOP 3
        
        # Update loop/offset counter, test for end of row
        add     ecx, 16
        jne     xLoop
        
# End of xLoop. Decrement outer loop counter, update row pointers, test for end of image
        lea     esi, [esi + ebx * 8]                        # point to next row of source image
        lea     edi, [edi + ebx * 2]
        lea     edx, [edx + ebx]
        
        # Update outloop counter        
        dec     dword ptr[esp + out_row_count]        
        jne     yLoop

# Finish
        rdtsc
        sub     eax, [esp + rdtsc_output]

        mov     esp, ebp
        sub     esp, 12         # pop edi, ebp, esi, and ebx from stack
        pop     edi
        pop     esi
        pop     ebx
        pop     ebp

        emms

        ret
