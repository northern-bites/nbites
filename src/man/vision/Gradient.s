/* -*- mode: asm; indent-tabs-mode: nil -*- */
.intel_syntax noprefix
# **************
# *            *
# *  Gradient  *
# *            *
# **************
#
# int gradient(short* sourceImage, int width, int height, int pitch,
#          short* gradientImage, int gradientThreshold);
#   
# Compute gradient of source image, 10-bit pixels (i.e. sum of four Y pixels) in
# 16-bit words. Pitch is in bytes, not pixels.
#
# Produce a gradient image with 16-bit pixels, where the high byte of each pixel
# is gradient magnitude and the low byte is gradient direction (binary angle).
# All pixels where gradient magnitude is less than the specified threshold are
# set to 0 (all 16 bits).
#
# Source image and gradientImage must be word aligned (multiple of 2). Pitch
# must be a multiple of 16 (bytes). There are no other alignment requirements.
#
# The row pitch of the gradient image in pixels is
#
#   (((sourceImage >> 1) + width + 7) & ~7) - ((sourceImage >> 1) & ~7)
#
# You must allocate exactly that much space for each output row. Valid output
# data in each row starts at column ((sourceImage >> 1) & 7) + 2. Each output
# row contains width - 2 valid pixels. There are height - 2 output rows in
# each image.
#
# Best practice is for gradientImage to be DQ aligned (multiple of 16), and
# edgeImage Q aligned (multiple of 8), but neither is required. The output
# image pitch requirement allows all source accesses to be DQ aligned,
# regardless of the specified address and width. This supports running on
# arbitrary windows of images.

.globl _gradient
.section .data

# ***************
# *             *
# *  Constants  *
# *             *
# ***************

    .align  16
cosSin: .word 32758, 32758, 32758, 32758
    .word 32758, 32758, 32758, 32758
    .word 804, 804, 804, 804
    .word 804, 804, 804, 804
    .word 32729, 32729, 32729, 32729
    .word 32729, 32729, 32729, 32729
    .word 1608, 1608, 1608, 1608
    .word 1608, 1608, 1608, 1608
    .word 32610, 32610, 32610, 32610
    .word 32610, 32610, 32610, 32610
    .word 3212, 3212, 3212, 3212
    .word 3212, 3212, 3212, 3212
    .word 32138, 32138, 32138, 32138
    .word 32138, 32138, 32138, 32138
    .word 6393, 6393, 6393, 6393
    .word 6393, 6393, 6393, 6393
    .word 30274, 30274, 30274, 30274
    .word 30274, 30274, 30274, 30274
    .word 12540, 12540, 12540, 12540
    .word 12540, 12540, 12540, 12540
tanK:   .word 82, 82, 82, 82
    .word 82, 82, 82, 82
inv6:   .word 0x003F, 0x003F, 0x003F, 0x003F
    .word 0x003F, 0x003F, 0x003F, 0x003F
rndMag: .word 0x0040, 0x0040, 0x0040, 0x0040
    .word 0x0040, 0x0040, 0x0040, 0x0040
rndDir: .word 1, 1, 1, 1, 1, 1, 1, 1
mask8:  .word 0x00FF, 0x00FF, 0x00FF, 0x00FF
    .word 0x00FF, 0x00FF, 0x00FF, 0x00FF

# *******************************
# *                             *
# *  Stack Frame and Arguments  *
# *                             *
# *******************************

    .struct 0
ySmooth:    .skip 32
yDiff:      .skip 32
time:       .skip 4           # starting time from rdtsc
locals_stack_end:

# struct
    .struct 0
saves:      .skip 20      # 4 saved registers plus return address
image:      .skip 4
srcWd:      .skip 4   
srcHt:      .skip 4   
pitch:      .skip 4   
gradDst:    .skip 4   
gradThr:    .skip 4   
args_stack_end:

# *****************
# *               *
# *  Entry Point  *
# *               *
# *****************

.section .text
_gradient:
# Preserve the required registers: ebp, ebx, esi, edi
    push    ebp
    push    ebx
    push    esi
    push    edi
    mov     ebp, esp

# Allocate local variables, ensure that the stack pointer is DQ aligned
    sub     esp, locals_stack_end 
    and     esp, 0x0FFFFFFF0

# Read and save starting time
    rdtsc
    mov     [esp + time], eax

# Fetch and extend threshold
    mov eax,    [ebp + gradThr]
    movd        xmm5, eax
    pshuflw     xmm5, xmm5, 0
    punpcklwd   xmm5, xmm5

# Process two fewer output rows
    mov eax, 2
    sub [ebp + srcHt], eax
    jle quit

# Set up general registers
#  eax  source row pitch
#  ebx  source row pitch * 3 for prefetch
#  ecx  inner loop counter
#  edx
#  esi  source address
#  edi  gradient image end of row address
    mov eax, [ebp + pitch]
    lea ebx, [eax + eax*2]

    mov esi, [ebp + image]
    mov ecx, esi
    and esi, 0x0FFFFFFF0
    shr ecx, 1
    mov edx, ecx
    add ecx, [ebp + srcWd]
    add ecx, 7
    and ecx, 0x0FFFFFFF8
    and edx, 0x0FFFFFFF8
    sub ecx, edx
    mov [ebp + srcWd], ecx

    mov edi, [ebp + gradDst]
    lea edi, [edi + ecx*2]

# **********************
# *                    *
# *  Outer/Inner Loop  *
# *                    *
# **********************

    neg ecx
yLoop:
xLoop:
# Fetch next set of 8 pixels from top, middle, and bottom rows
    movdqa  xmm0, [esi]
    movdqa  xmm1, [esi + eax]
    movdqa  xmm2, [esi + eax*2]

# Prefetch next row
    prefetcht1 [esi + ebx]
    add esi, 16

# Smoothing part of x gradient
    paddw   xmm1, xmm1              # 2 * middle
    paddw   xmm1, xmm0              # + top
    paddw   xmm1, xmm2              # + bottom
    movdqa  [esp + ySmooth + 16], xmm1  # save for later
    
# Differencing part of y gradient
    psubw   xmm2, xmm0              # bottom - top
    movdqa  [esp + yDiff + 16], xmm2

# Differencing part of x gradient. Use unaligned move to line up words for subtract
    movdqa  xmm0, xmm1
    movdqu  xmm3, [esp + ySmooth + 12]  # 8 words offset by -2
    psubw   xmm0, xmm3
    movdqa  [esp + ySmooth], xmm1       # save smoothed words for next iteration

# Smoothing part of y gradient. Use unaligned moves to line up words for smoothing
    movdqu  xmm1, [esp + yDiff + 14]        # 8 words offset by -1
    paddw   xmm1, xmm1
    paddw   xmm1, xmm2
    movdqu  xmm3, [esp + yDiff + 12]        # 8 words offset by -2
    paddw   xmm1, xmm3
    movdqa  [esp + yDiff], xmm2     # save differenced words for next iteration
    
# Now we have gradient: xmm0 = gx, xmm1 = gy. The source image is the sum of 4 8-bit
# Y values, and Sobel gradient adds another factor of 4 plus sign, so we have 13-bit
# signed gradient components.

# Octant conversion. We'll fold the gradient components into the first octant
# (0 - 45 deg), so that gx >= 0, gy >= 0 and gx >= gy. Keep track of the
# foldings in xmm1 so we know which octant. We'll form a 9-bit binary
# angle in xmm1 so we can round off to 8 bits later. The binary angle
# is formed by XOR to keep track of the effects of folding.

# Half-plane
    pabsw   xmm3, xmm1      # xmm3 = |gy|
    psraw   xmm1, 15        # xmm1 gets half-plane bits
    psrlw   xmm1, 7
    pxor    xmm1, XMMWORD PTR inv6  # bookkeeping

# Quadrant
    pabsw   xmm2, xmm0      # xmm2 = |gx|
    psraw   xmm0, 15
    psrlw   xmm0, 8
    pxor    xmm1, xmm0      # xmm1 gets quadrant bits

# Octant
    movdqa  xmm0, xmm2      # xmm0 = |gx|
    pmaxsw  xmm0, xmm3      # xmm0 = max(|gx|, |gy|)
    pminsw  xmm2, xmm3      # xmm2 = min(|gx|, |gy|)
    pcmpeqw xmm3, xmm0
    psrlw   xmm3, 9
    pxor    xmm1, xmm3      # xmm1 gets octant bits

# The next 5 bits of direction are computed using this arcTan macro, which
# computes 1 bit for each invokation. It works by rotating the (gx,gy)
# vector by the angle correspondsing to the binary angle bit being
# sought, and seeing if the result is a positive or negative angle
# (i.e. is gy >= or < zero). If gy is negative, another folding occurs.
# When the gradient vector has been rotated through all of the angles,
# gy will be nearly 0 and gx will be the magnitude of the original vector.
# So we have got the full cartesian to polar conversion.
#   xmm0    gx, updated
#   xmm1    binary angle bits
#   xmm2    gy, updated
#   xmm3    temp
#   xmm4    temp
.macro ArcTan1  bit
    movdqa  xmm3, xmm0          # copy gx, gy
    movdqa  xmm4, xmm2
    movdqa  xmm6, XMMWORD PTR cosSin + 32*\bit     
    movdqa  xmm7, XMMWORD PTR cosSin + 32*\bit + 16
    pmulhrsw xmm0, xmm6     # rotate gx, gy
    pmulhrsw xmm2, xmm7
    pmulhrsw xmm3, xmm7
    pmulhrsw xmm4, xmm6
    paddw   xmm0, xmm2
    psubw   xmm4, xmm3
    pabsw   xmm2, xmm4          # |gy|
    psraw   xmm4, 15            # fold in sign of gy
    psrlw   xmm4, 14 - \bit
    pxor    xmm1, xmm4
.endm

# (gx,gy) are 12 bits unsigned now. Shift left 3 to use 15 bits for best
# numerical accuracy.
    psllw   xmm0, 3
    psllw   xmm2, 3

# Complete the arctangent and get magnitude for free
    ArcTan1 4
    ArcTan1 3
    ArcTan1 2
    ArcTan1 1
    ArcTan1 0

# The 9th bit of grdient direction is obtained by comparing gy/gx to the
# tangent of the angle corresponding to that final bit. The equivalent
# comparison uses the sign of gy * (1/tan) - gx, where 1/tan ~ 82.
    pmullw  xmm2, tanK
    psubw   xmm2, xmm0
    psrlw   xmm2, 15
    pxor    xmm1, xmm2

# Round off magnitude (remaining gx) and convert to 8 bits
    paddw   xmm0, rndMag
    psrlw   xmm0, 7

# Round off direction and convert to 8 bits
    paddw   xmm1, rndDir
    psrlw   xmm1, 1
    pand    xmm1, XMMWORD PTR mask8

# Compute the gradient pixels
    movdqa      xmm2, xmm0          # copy magnitude
    pcmpgtw     xmm2, xmm5          # compare to threshold
    pand        xmm0, xmm2          # zero mag and dir bytes for mag below threshold
    pand        xmm1, xmm2
    packuswb    xmm0, xmm0          # pack mag and dir bytes
    packuswb    xmm1, xmm1
    punpcklbw   xmm1, xmm0
    movdqu      [edi + ecx*2], xmm1     # write to gradient image

# ******************
# *                *
# *  End of Loops  *
# *                *
# ******************

    add ecx, 8
    jl  xLoop

    mov ecx, [ebp + srcWd]
    lea edi, [edi + ecx*2]

    neg ecx
    lea esi, [esi + ecx*2]
    add esi, eax

    dec DWORD PTR[ebp + srcHt]
    jg  yLoop

# **********
# *        *
# *  Done  *
# *        *
# **********

quit:
    rdtsc
    sub     eax, [esp + time]

# Restore necessary _cdecl calling convention registers
    mov     esp, ebp
    pop     edi
    pop     esi
    pop     ebx
    pop     ebp

    ret
