/* -*- mode: asm; indent-tabs-mode: nil -*- */
.intel_syntax noprefix

.globl _mark_edges

.section .data

        ## Parameter layout
        .struct 8
num_peaks:      .skip 4
angle_spread:   .skip 4
peaks:          .skip 4
hough_space:

        ## Peak list layout
        .struct 0
angle_offset:   .skip 2
x_offset:       .skip 2
y_offset:

        ## Stack layout
        .struct 0
spread: .skip 4
end_of_stack:

sinTable:

cosTable:

        .equiv R_SPAN, 256

.section .text

.macro GET_R x, y, t
        ## r = x * cos(t) + y * sin(t)
        ## Possibly more code needed here to process multiplication results
        imul    \x, [cosTable + t]
        imul    \y, [sinTable + t]
        add     \x, \y

        ## Set \y to zero
        xor     \y,\y

        ## Put the R value in the Hough Space coordinates
        add     \x, R_SPAN/2

        ## Ensure R >= 0
        ## If \x + R_SPAN/2 < 0 (less than, than set it to zero)
        cmovl   \x, \y

        ## Ensure R < R_SPAN
        and     \x, R_SPAN-1
.endm

        ## _mark_edges(int numPeaks, int angleSpread,
        ##             uint16_t *peaks, int *houghSpace)
_mark_edges:
        push    ebp
        mov     ebp, esp

        push    esi
        push    edi
        push    ebx

        sub     esp, end_of_stack

peakLoop:
        ## Load pointer to peak list
        mov     edi, dword ptr[ebp + peaks]

        ## Load angleSpread parameter
        mov     eax, [ebp + angle_spread]

        ## Get gradient angle lower bound
        mov     esi, [edi + angle_offset]

        ## esi = t0 - angleSpread
        sub     esi, eax

        ## Double spread param and store it on the stack as a counter
        shl     eax, 1
        mov     dword ptr[esp + spread], eax

        ## Load peak's x and y values
        mov     ebx, [edi + x_offset]
        mov     eax, [edi + y_offset]

        ## put r0, radius at t-angleSpread in ebx
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
        add     eax, dword ptr[ebp + hough_space]

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
        inc     dword ptr[eax + edi * 4] # ti * R_SPAN + ri * (4 bytes per bin)

        ## radiiLoop
        inc     edi             # Increment ri
        dec     ecx             # Increment rDiff counter
        jge     radiiLoop       # rDiff >= 0, means minR < ri <= maxR

        ## r0 = r1
        mov     ebx, edx

        ## gradientLoop
        inc     esi             # Increment ti
        dec     dword ptr[esp + spread]
        jne     gradientLoop

        ## 6 bytes per x,y,angle triple
        add     dword ptr[ebp + peaks], 6

        dec     dword ptr[ebp + num_peaks]
        jne     peakLoop

        pop     ebx
        pop     edi
        pop     esi

        mov     esp, ebp
        pop     ebp

        ret

