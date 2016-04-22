/* -*- mode: asm; indent-tabs-mode: nil -*- */
.intel_syntax noprefix
#// *********************************
#// *                               *
#// *  Image Acquisition Front End  *
#// *                               *
#// *********************************

.globl _acquire_image

.section .data

 multiPhase  = 1 # enable two-phase processing (significant improvement)
 ntStore     = 0 # enable non-temporal stores, only for Y values in multiPhase mode (unclear improvement)
 prefetchSrc = 1 # enable prefetching (significant improvement)
 colorTable  = 0 # enable color table lookup

#// ***************************
#// *                         *
#// *  Structure Definitions  *
#// *                         *
#// ***************************

# Local variables on stack
    .struct 0  
# White parameters
whiteDark0:     .skip 16
whiteYCoeff:    .skip 16
whiteFuzzy:     .skip 16
whiteInvFuz:    .skip 16
# Green parameters struct
greenDark0:     .skip 16
greenYCoeff:    .skip 16
greenFuzzy:     .skip 16
greenInvFuz:    .skip 16
# Orange parameters
orangeDark0:    .skip 16
orangeYCoeff:   .skip 16
orangeFuzzy:    .skip 16
orangeInvFuz:   .skip 16

endOfColors:

orInvV:         .skip 16        # invert V values by XOR for orange
uvZero:         .skip 16        # UV zero value (256)
time:           .skip 4         # starting time from rdtsc
srcUpd:         .skip 4         # source image row update value in bytes
dPitch:         .skip 4         # width * height is destination image pitch
align:          .skip 4         # keep double quadword aligned
tableK:         .skip 16        # combine U and V table indicies with pmaddwd
tabIdx:         .skip 2560      # Table indicies stored during main processing loop. Room for up to 640
                # wide destination image. Use fixed size to make addressing easier.
                # These are processed in a separate loop because we don't have enough
                # GP registers.
localsStackEnd:

# Arguments accessed via ebp
    .struct 0
saves:      .skip 20    # 4 saved registers plus return address
source:     .skip 4     # address of source image
dstWd:      .skip 4     # destination image width, low 3 bits ignored and assumed 0
dstHt:      .skip 4     # destination image height
pitch:      .skip 4     # source image pitch in bytes
cParams:    .skip 4     # address of colors parameters
dest:       .skip 4     # address of destination images
table:      .skip 4     # address of color lookup table
argsStackEnd:

#// *****************
#// *               *
#// *  Entry Point  *
#// *               *
#// *****************

.section .text

_acquire_image:

# Preserve the required registers: ebp, ebx, esi, edi
    push    ebp
    push    ebx
    push    esi
    push    edi
    mov     ebp, esp

# Allocate local variables, ensure that the stack pointer is DQ aligned
    sub     esp, localsStackEnd 
    and     esp, 0x0FFFFFFF0

# Read and save starting time
    rdtsc
    mov     [esp + time], eax

# Copy colors to stack for access with esp and to insure DQ alignment
    mov esi, [ebp + cParams]
    lea edi, [esp + whiteDark0]
    mov ecx, endOfColors
    neg ecx
copy:  movdqu  xmm0, [esi + ecx + endOfColors]
    movdqa  [edi + ecx + endOfColors], xmm0 
    add ecx, 16
    jne copy

# Make the constant to invert orange V so fuzzy max can be used (0x1FF in V words)
    pcmpeqb xmm0, xmm0
    psrld   xmm0, 23
    pslld   xmm0, 16
    movdqa  [esp + orInvV], xmm0

# Make uvZero: 256 in all words
    pcmpeqb xmm0, xmm0
    psrlw   xmm0, 15
    psllw   xmm0, 8
    movdqa  [esp + uvZero], xmm0

# Make the color table index constant. 128*128 in U words, 128 in V words
.if (colorTable)
    pcmpeqb xmm0, xmm0          # all 1s
    psrld   xmm0, 31            # four dw of value 1
    movdqa  xmm1, xmm0      
    pslld   xmm0, 7            # four dw of value 0b00000000 00000000 01000000 00000000
    pslld   xmm1, 30            # four dw of value 0b00000000 01000000 00000000 00000000
    por xmm0, xmm1              # four dw of calue 0b00000000 01000000 01000000 00000000
                                                #  0b01000000 00000000 00000000 01000000
    movdqa  [esp + tableK], xmm0
.endif

# Make the source row update constant
    mov eax, [ebp + pitch]     # two rows
    shl eax, 1
    mov ebx, [ebp + dstWd]     # minus bytes moved by inner loop
    shl ebx, 2
    sub eax, ebx
    mov [esp + srcUpd], eax

#// ********************************
#// *                              *
#// *  Main Processing loop Macro  *
#// *                              *
#// ********************************

# eax   source row pitch
# ebx   destination image pitch
# ecx   inner loop counter, ascending from -width to 0
# edx   -> destination white image
# esi   -> source top row
# edi   -> destination Y image

# phase is 0 or 1
.macro xGroup phase
# fetch next 4 top row pixels, split into Y, UV
# xmm0 = | Y07 | Y06 | Y05 | Y04 | Y03 | Y02 | Y01 | Y00 |
# xmm1 = | V03 | U03 | V02 | U02 | V01 | U01 | V00 | U00 |
    movdqu  xmm0, [esi + 16*\phase]
    movdqa  xmm1, xmm0
    psllw   xmm0, 8
    psrlw   xmm0, 8
    psrlw   xmm1, 8

# fetch next 4 bottom row pixels, split into Y, UV
# xmm2 = | Y17 | Y16 | Y15 | Y14 | Y13 | Y12 | Y11 | Y10 |
# xmm3 = | V13 | U13 | V12 | U12 | V11 | U11 | V10 | U10 |
   movdqu  xmm2, [esi + eax + 16*\phase]

# here is a good place to prefetch the next two rows
.if (prefetchSrc == 1)
    prefetcht1 [esi + eax*2]
    add esi, eax
    prefetcht1 [esi + eax*2]
    sub esi, eax
.endif

# Update the source pointer in esi. We do it just after needing it for the last
# time in this loop iteration to cooperate with the CPU pipeline and OOO
# execution.
.if (multiPhase == 1)
    .if (\phase == 1)
        add esi, 32
    .endif
.else
    add esi, 16
.endif


# Now we can finish splitting up the bottom row pixels
    movdqa  xmm3, xmm2
    psllw   xmm2, 8
    psrlw   xmm2, 8
    psrlw   xmm3, 8

# sum 4 sets of 4 Y pixels. We will use the fact that the even words equal the odd words
# for later UV processing
# xmm0 = | Y06+Y07+Y16+Y17 | Y06+Y07+Y16+Y17 | Y04+Y05+Y14+Y15 | Y04+Y05+Y14+Y15 | Y02+Y01+Y12+Y11 | Y02+Y03+Y12+Y13 | Y00+Y01+Y10+Y11 | Y00+Y01+Y10+Y11 |
    paddw   xmm0, xmm2
    pshuflw xmm2, xmm0, 0b10110001
    pshufhw xmm2, xmm2, 0b10110001
    paddw   xmm0, xmm2

# Write 4 Y pixels.
.if (multiPhase == 1)
    .if (\phase == 0)
        movdqa  xmm4, xmm0          # multiphase phase 0, save for later
        psrld   xmm4, 16
    .else
        movdqa  xmm2, xmm0          # multiphase phase 1, combine with saved and write to destination
        psrld   xmm2, 16
        packssdw xmm4, xmm2
        .if (ntStore == 1)
            movntdq [edi + ecx*2], xmm4
        .else
            movdqu   [edi + ecx*2], xmm4
        .endif
    .endif

.else
    movdqa  xmm2, xmm0          # single phase, write to destination
    psrld   xmm2, 16
    packssdw xmm2, xmm2
    movq    QWORD PTR[edi + ecx*2], xmm2    # movntq is for MMX only
.endif

# sum 4 sets of 2 UV pixels
# xmm1 = | V03+V13 | U03+U13 | V02+V12 | U02+U12 | V01+V11 | U01+U11 | V00+V10 | U00+U10 |
    paddw   xmm1, xmm3

# white color
    movdqa  xmm2, xmm0                  # change in dark0 due to y
    pmulhw  xmm2, [esp + whiteYCoeff]
    paddsw  xmm2, [esp + whiteDark0]    # new dark0
    movdqa  xmm3, xmm1                  # recenter UV round 0 for absolute value
    psubw   xmm3, [esp + uvZero]
    pabsw   xmm3, xmm3                  # absolute value
    paddw   xmm3, [esp + uvZero]         # recenter around uvZero for fuzzy calcs
    psubusw xmm2, xmm3                  # max(t0 - |UV|, 0)
    pminsw  xmm2, [esp + whiteFuzzy]    # min(max(t0 - |UV|, 0), fuzzy)
    pmullw  xmm2, [esp + whiteInvFuz]   # min(max(t0 - |UV|, 0), fuzzy) * invFuz) >> 8
    pshuflw xmm3, xmm2, 0b10110001               # swap U and V for fuzzy AND (min)
    pshufhw xmm3, xmm3, 0b10110001
    psrld   xmm2, 24                    # this is the >> 8, but extra 16 for alignment
    psrld   xmm3, 24                    # this is the >> 8, but extra 16 for alignment
    pminsw  xmm2, xmm3                  # Fuzzy AND

.if (multiPhase == 1)
    .if (\phase == 0)
        movdqa  xmm5, xmm2      # multiphase phase 0, save 4 white pixels
    .else
        packssdw xmm5, xmm2     # multiphase phase 1, combine and write 8 white pixels
        packuswb xmm5, xmm5
        movq    QWORD PTR[edx], xmm5
    .endif
.else
    packssdw xmm2, xmm2     # single phase, write 4 white pixels
    packuswb xmm2, xmm2
    movd    DWORD PTR[edx], xmm2
.endif
 

# green color
    movdqa  xmm2, xmm0                  # change in dark0 due to y
    pmulhw  xmm2, [esp + greenYCoeff]
    paddsw  xmm2, [esp + greenDark0]    # new dark0
    psubusw xmm2, xmm1                  # max(t0 - |UV|, 0)
    pminsw  xmm2, [esp + greenFuzzy]    # min(max(t0 - |UV|, 0), fuzzy)
    pmullw  xmm2, [esp + greenInvFuz]   # min(max(t0 - |UV|, 0), fuzzy) * invFuz) >> 8
    pshuflw xmm3, xmm2, 0b10110001               # swap U and V for fuzzy AND (min)
    pshufhw xmm3, xmm3, 0b10110001
    psrld   xmm2, 24                    # this is the >> 8, but extra 16 for alignment
    psrld   xmm3, 24                    # this is the >> 8, but extra 16 for alignment
    pminsw  xmm2, xmm3                  # Fuzzy AND

.if (multiPhase == 1)
    .if (\phase == 0)
        movdqa  xmm6, xmm2      # multiphase phase 0, save 4 green pixels
    .else
        packssdw xmm6, xmm2     # multiphase phase 1, combine and write 8 green pixels
        packuswb xmm6, xmm6
        movq    QWORD PTR[edx + ebx], xmm6
    .endif
.else
    packssdw xmm2, xmm2     # single phase, write 4 green pixels
    packuswb xmm2, xmm2
    movd    DWORD PTR[edx + ebx], xmm2
.endif
 

# orange color
#     movdqa  xmm2, xmm0                  # change in dark0 due to y
#     pmulhw  xmm2, [esp + orangeYCoeff]
#     paddsw  xmm2, [esp + orangeDark0]   # new dark0
#     movdqa  xmm3, xmm1                  # invert V to make it max instead of min
#     pxor    xmm3, [esp + uvZero]
#     psubusw xmm2, xmm3                  # max(t0 - |UV|, 0)
#     pminsw  xmm2, [esp + orangeFuzzy]   # min(max(t0 - |UV|, 0), fuzzy)
#     pmullw  xmm2, [esp + orangeInvFuz]  # min(max(t0 - |UV|, 0), fuzzy) * invFuz) >> 8
#     pshuflw xmm3, xmm2, 0b10110001               # swap U and V for fuzzy AND (min)
#     pshufhw xmm3, xmm3, 0b10110001
#     psrld   xmm2, 24                    # this is the >> 8, but extra 16 for alignment
#     psrld   xmm3, 24                    # this is the >> 8, but extra 16 for alignment
#     pminsw  xmm2, xmm3                  # Fuzzy AND

# .if (multiPhase == 1)
#     .if (\phase == 0)
#         movdqa  xmm7, xmm2      # multiphase phase 0, save 4 orange pixels
#     .else
#         packssdw xmm7, xmm2     # multiphase phase 1, combine and write 8 orange pixels
#         packuswb xmm7, xmm7
#         movq    QWORD PTR[edx + ebx*2], xmm7
#         add edx, 8
#     .endif
# .else
#     packssdw xmm2, xmm2     # single phase, write 4 orange pixels
#     packuswb xmm2, xmm2
#     movd    DWORD PTR[edx + ebx*2], xmm2
#     add edx, 4
# .endif

# black color TODO: change parameter names to black from orange
    movdqa  xmm2, xmm0                  # change in dark0 due to y
    pmulhw  xmm2, [esp + orangeYCoeff]
    paddsw  xmm2, [esp + orangeDark0]    # new dark0
    movdqa  xmm3, xmm1                  # recenter UV round 0 for absolute value
    psubw   xmm3, [esp + uvZero]
    pabsw   xmm3, xmm3                  # absolute value
    paddw   xmm3, [esp + uvZero]         # recenter around uvZero for fuzzy calcs
    psubusw xmm2, xmm3                  # max(t0 - |UV|, 0)
    pminsw  xmm2, [esp + orangeFuzzy]    # min(max(t0 - |UV|, 0), fuzzy)
    pmullw  xmm2, [esp + orangeInvFuz]   # min(max(t0 - |UV|, 0), fuzzy) * invFuz) >> 8
    pshuflw xmm3, xmm2, 0b10110001               # swap U and V for fuzzy AND (min)
    pshufhw xmm3, xmm3, 0b10110001
    psrld   xmm2, 24                    # this is the >> 8, but extra 16 for alignment
    psrld   xmm3, 24                    # this is the >> 8, but extra 16 for alignment
    pminsw  xmm2, xmm3                  # Fuzzy AND

.if (multiPhase == 1)
    .if (\phase == 0)
        movdqa  xmm7, xmm2      # multiphase phase 0, save 4 white pixels
    .else
        packssdw xmm7, xmm2     # multiphase phase 1, combine and write 8 white pixels
        packuswb xmm7, xmm7
        movq    QWORD PTR[edx + ebx*2], xmm7
        add edx, 8
    .endif
.else
    packssdw xmm2, xmm2     # single phase, write 4 white pixels
    packuswb xmm2, xmm2
    movd    DWORD PTR[edx + ebx*2], xmm2
    add edx,4
.endif


# Compute and save color table indicies
.if (colorTable == 1)
    psrld   xmm0, 19            # high 7 bits of 4 Y values in dwords
    psrlw   xmm1, 2             # high 7 bits of 4 U and 4 V values in words
    pmaddwd xmm1, [esp + tableK] # combine U and U
    paddd   xmm0, xmm1          # and Y for 3D table index
    movdqa  [esp + (ecx*4) + localsStackEnd + (16 * \phase)], xmm0    # save on stack for later
.endif

.endm

#// ****************
#// *              *
#// *  Outer Loop  *
#// *              *
#// ****************

# eax   source row pitch
# ebx   destination image pitch
# ecx   inner loop counter, ascending from -width to 0
# edx   -> destination white image
# esi   -> source top row
# edi   -> destination Y image

    mov eax, [ebp + pitch]
    mov ebx, [ebp + dstWd]
    imul    ebx, [ebp + dstHt]
    mov edi, [ebp + dest]
    lea edx, [edi + ebx*2]
    mov esi, [ebp + source]

.if (colorTable)
    mov [esp + dPitch], ebx
.endif

# Outer loop
yLoop:
    mov ecx, [ebp + dstWd]
    lea edi, [edi + ecx*2]
    neg ecx

#// *********************
#// *                   *
#// *  Inner Main Loop  *
#// *                   *
#// *********************

# Inner loop
xLoop:
    xGroup 0
.if (multiPhase == 1)
    xGroup 1
    add ecx, 8
.else
    add ecx, 4
.endif
    jl  xLoop

# end of inner loop
    add esi, [esp + srcUpd]

#// ****************************
#// *                          *
#// *  Inner Color Table Loop  *
#// *                          *
#// ****************************
#
# eax   hold table index and byte fetched from table
# ebx   -> color table
# ecx   counter, ascending from -width to 0
# edx   -> end of current output row
# 
.macro cGroup phase
    mov     eax, [esp + localsStackEnd + ecx*4 + \phase*4]  # Load the color address from the stack
    movzx   eax, BYTE PTR[ebx + eax]        # Lookup color in table
    mov     BYTE PTR[edx + ecx + \phase], al     # write it out
.endm

.if (colorTable == 1)
    mov ecx, [ebp + dstWd]
    neg ecx
    lea eax, [ebx + ebx*2]
    add edx, eax
    mov ebx, [ebp + table]
cLoop:  cGroup  0
    cGroup  1
    cGroup  2
    cGroup  3
    add ecx, 4
    jl  cLoop

    mov ebx, [esp + dPitch]  # restore main loop's registers
    lea eax, [ebx + ebx*2]
    sub edx, eax
    mov eax, [ebp + pitch]
.endif

#// **********************************
#// *                                *
#// *  Enf of Outer Loop and Return  *
#// *                                *
#// **********************************

    dec dword ptr[ebp + dstHt]
    jg  yLoop

    rdtsc
    sub     eax, [esp + time]

    # Restore necessary _cdecl calling convention registers
    mov     esp, ebp
    pop     edi
    pop     esi
    pop     ebx
    pop     ebp

    ret

