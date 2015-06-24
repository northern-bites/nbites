/* -*- mode: asm; indent-tabs-mode: nil -*- */
.intel_syntax noprefix
# ************************
# *                      *
# *  Smooth Hough Space  *
# *                      *
# ************************
#
# void houghSmooth(uint16_t* image, int dstWd, int dstHt, int pitch);
#
# In-place smooth Hough space using kernel:
#
#   1 2 1
#   2 4 2
#   1 2 1
#
# image, dstWd, and dstHt specify the result image, which will contain valid
# smoothed values. pitch must be a multiple of 16, but there are no other
# alignment requirements. Will read one row and column beyond result image
# on all sides, which rows and columns must contain valid values. May read
# more that one column beyond left and right sides, values there don't matter.
#
# Smoothed values replace original; the kernel is centered so there is no
# position shift of the result. Will overwrite one row above the result
# image with undefined values. May overwrite columns left and right with
# undefined values. The overwriting is OK because the active region of
# the Hough space is smaller than the allocated bins.

.globl _houghSmooth
.section .data


# *******************************
# *                             *
# *  Stack Frame and Arguments  *
# *                             *
# *******************************

   .struct 0    #
  rowBufs:   .skip 16   # This is unused
  local_stack_end:    #

   .struct 0
  saves:  .skip 20   # 4 saved registers (16 bytes) plus return address
  image:  .skip 4
  dstWd:  .skip 4
  dstHt:  .skip 4
  pitch:  .skip 4
  args_stack_end:

# *****************
# *               *
# *  Entry Point  *
# *               *
# *****************

.section .text

_houghSmooth:
# Preserve the required registers: ebp, ebx, esi, edi
  push    ebp
  push    ebx
  push    esi
  push    edi
  mov     ebp, esp    
# Make width and source address DQ aligned
  mov   esi, [ebp + image]
  mov   ecx, esi
  and   ecx, 0x0F
  sub   esi, ecx
  shr   ecx, 1
  add   ecx, [ebp + dstWd]
  add   ecx, 7
  and   ecx, 0x0FFFFFFF8
  mov   [ebp + dstWd], ecx

# Allocate local variables, ensure that the stack pointer is DQ aligned
  neg   ecx
  lea   esp, [esp + ecx*4]
  and esp, 0x0FFFFFFF0

# Top and mid row addresses (point to end of buffers)
  mov   eax, [ebp + dstWd]
  lea   edx, [esp + eax*2]
  lea   edi, [esp + eax*4]

# pitch
  mov   eax, [ebp + pitch]
  add   eax, eax
  neg   eax

# Prefill top
  add   esi, eax
fill:   movdqa   xmm0, [esi]
  movdqu   xmm1, [esi - 2]
  movdqu   xmm2, [esi + 2]
  add   esi, 16
  paddw   xmm0, xmm0
  paddw   xmm1, xmm2
  paddw   xmm0, xmm1
  movdqa   [edi + ecx*2], xmm0
  add   ecx, 8
  jl   fill

# **********************
# *                    *
# *  Outer/Inner Loop  *
# *                    *
# **********************
#
#  eax   -pitch
#  ebx
#  ecx   inner loop counter
#  edx   top buffer address
#  esi   source address
#  edi   mid buffer address
yLoop:    
  sub   esi, eax
  mov   ecx, [ebp + dstWd]
  neg   ecx 
  lea   esi, [esi + ecx*2]

xLoop:
# Next 121 smooth in x, bottom row
  movdqa   xmm0, [esi]
  movdqu   xmm1, [esi - 2]
  movdqu   xmm2, [esi + 2]
  paddw   xmm0, xmm0
  paddw   xmm1, xmm2
  paddw   xmm0, xmm1

  movdqa   xmm1, [edx + ecx * 2]
  movdqa   [edx + ecx * 2], xmm0
  paddw   xmm0, xmm1
  movdqa   xmm1, [edi + ecx * 2]
  paddw   xmm1, xmm1
  paddw   xmm0, xmm1

  movdqa   [esi + eax], xmm0
  add   esi, 16

# ******************
# *                *
# *  End of Loops  *
# *                *
# ******************

  add   ecx, 8
  jl   xLoop

# swap top and mid pointers
  xor   edx, edi
  xor   edi, edx
  xor   edx, edi

  dec   DWORD PTR[ebp + dstHt]
  jge   yLoop

# **********
# *        *
# *  Done  *
# *        *
# **********

# Restore necessary _cdecl calling convention registers
  mov     esp, ebp
  pop     edi
  pop     esi
  pop     ebx
  pop     ebp

  ret
