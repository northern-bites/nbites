/* -*- mode: asm; indent-tabs-mode: nil -*- */
.intel_syntax noprefix

.globl _mark_edges

.section .data

        .struct 8
num_peaks:      .skip 4
angle_spread:   .skip 4
peaks:          .skip 4
hough_space:

        .struct 0
angle_offset:   .skip 2
x_offset:       .skip 2
y_offset:

sinTable:

cosTable:

        .equiv R_SPAN, 256

.section .text

.macro GET_R x, y, t
        ## x * cos(t) + y * sin(t)
        imul    \x, [sinTable + t]
        imul    \y, [cosTable + t]

        add     \x, \y

        ## Set \y to all zeros for later use
        mov     \y, R_SPAN

        ## Put the R value in the Hough Space coordinates
        add     \x, R_SPAN_OVER_2

        ## Ensure 0 <= R < R_SPAN
        cmp     \x, \y
        cmovae  \x, \y

        xor     \y,\y

        cmp     \x, \y
        cmovb   \x, \y
.endm

        ## _mark_edges(int numPeaks, int angleSpread,
        ##             uint16_t *peaks, int *houghSpace)
_mark_edges:
        push    ebp
        mov     ebp, esp

        push    esi
        push    edi
        push    ebx

peakLoop:
        mov     edi, dword ptr[ebp + peaks]

        ## Get gradient angle lower bound
        mov     esi, [edi + angle_offset]

        ## esi = t0 - angleSpread
        sub     esi, [ebp + angle_spread]

        ## Get radius at gradient lower bound
        mov     ebx, [edi + x_offset]
        mov     eax, [edi + y_offset]

        ## put r0 in ebx
        GET_R   ebx, eax, esi

        ## Iterate from lower gradient angle to upper
gradientLoop:
        ## edx = x
        ## eax = y
        mov     edx, [edi + x_offset]
        mov     eax, [edi + y_offset]

        ## Get radius at current angle + 1
        mov     ecx, esi
        add     ecx, 1

        ## ebx = r0, edx = r1
        GET_R   edx, eax, ecx

        ## Calculate hough space address
        mov     eax, esi
        imul    eax, R_SPAN * 4 # 4 bytes per hough bin
        add     eax, [ebp + hough_space]

        ## edi = min(r0,r1), ecx = max(r0, r1)
        cmp     edx, ebx

        ## If edx > ebx
        cmova   ecx, edx
        cmova   edi, ebx

        ## If edx <= ebx
        cmovbe  ecx, ebx
        cmovbe  edi, edx

        ## ecx = maxR - minR
        ## rDiff counter
        sub     ecx, edi

        ## Iterate from minR to maxR
        ## for ( ; rDiff < 0; rDiff++)
radiiLoop:
        inc     dword ptr[eax + edi * 4] # ti * R_SPAN + ri

        ## radiiLoop
        inc     edi             # Increment ri
        dec     ecx             # Increment rDiff counter
        jge     radiiLoop       # rDiff >= 0, means minR < ri <= maxR

        ## r0 = r1
        mov     ebx, edx

        ## gradientLoop
        inc     esi             # Increment ti
        dec     dword ptr[ebp + angle_spread]
        jne     gradientLoop

        ## 6 bytes per x,y,angle triple
        add     dword ptr[ebp + peaks], 6

        dec     [ebp + num_peaks]
        jne     peakLoop

        pop     ebx
        pop     edi
        pop     esi

        mov     esp, ebp
        pop     ebp

        ret