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

        .equiv R_SPAN, 256

        .data
        ## Calculated in Excel as ROUND( SIN(angle * PI/128) * 128/PI)
sinTable:
        .byte    -0, -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11
        .byte    -12, -13, -14, -15, -16, -17, -17, -18, -19, -20, -21, -22
        .byte    -23, -23, -24, -25, -26, -27, -27, -28, -29, -30, -30, -31
        .byte    -31, -32, -33, -33, -34, -34, -35, -35, -36, -36, -37, -37
        .byte    -38, -38, -38, -39, -39, -39, -40, -40, -40, -40, -40, -40
        .byte    -41, -41, -41, -41, -41, -41, -41, -41, -41, -40, -40, -40
        .byte    -40, -40, -40, -39, -39, -39, -38, -38, -38, -37, -37, -36
        .byte    -36, -35, -35, -34, -34, -33, -33, -32, -31, -31, -30, -30
        .byte    -29, -28, -27, -27, -26, -25, -24, -23, -23, -22, -21, -20
        .byte    -19, -18, -17, -17, -16, -15, -14, -13, -12, -11, -10, -9
        .byte    -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3
        .byte    4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15
        .byte    16, 17, 17, 18, 19, 20, 21, 22, 23, 23, 24, 25
        .byte    26, 27, 27, 28, 29, 30, 30, 31, 31, 32, 33, 33
        .byte    34, 34, 35, 35, 36, 36, 37, 37, 38, 38, 38, 39
        .byte    39, 39, 40, 40, 40, 40, 40, 40, 41, 41, 41, 41
        .byte    41, 41, 41, 41, 41, 40, 40, 40, 40, 40, 40, 39
        .byte    39, 39, 38, 38, 38, 37, 37, 36, 36, 35, 35, 34
        .byte    34, 33, 33, 32, 31, 31, 30, 30, 29, 28, 27, 27
        .byte    26, 25, 24, 23, 23, 22, 21, 20, 19, 18, 17, 17
        .byte    16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5
        .byte    4, 3, 2, 1, 0

        ## Calculated in Excel as ROUND( COS(angle * PI/128) * 128/PI)
cosTable:
        .byte    -41, -41, -41, -41, -41, -40, -40, -40, -40, -40, -40, -39
        .byte    -39, -39, -38, -38, -38, -37, -37, -36, -36, -35, -35, -34
        .byte    -34, -33, -33, -32, -31, -31, -30, -30, -29, -28, -27, -27
        .byte    -26, -25, -24, -23, -23, -22, -21, -20, -19, -18, -17, -17
        .byte    -16, -15, -14, -13, -12, -11, -10, -9, -8, -7, -6, -5
        .byte    -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7
        .byte    8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 17, 18
        .byte    19, 20, 21, 22, 23, 23, 24, 25, 26, 27, 27, 28
        .byte    29, 30, 30, 31, 31, 32, 33, 33, 34, 34, 35, 35
        .byte    36, 36, 37, 37, 38, 38, 38, 39, 39, 39, 40, 40
        .byte    40, 40, 40, 40, 41, 41, 41, 41, 41, 41, 41, 41
        .byte    41, 40, 40, 40, 40, 40, 40, 39, 39, 39, 38, 38
        .byte    38, 37, 37, 36, 36, 35, 35, 34, 34, 33, 33, 32
        .byte    31, 31, 30, 30, 29, 28, 27, 27, 26, 25, 24, 23
        .byte    23, 22, 21, 20, 19, 18, 17, 17, 16, 15, 14, 13
        .byte    12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1
        .byte    0, -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11
        .byte    -12, -13, -14, -15, -16, -17, -17, -18, -19, -20, -21, -22
        .byte    -23, -23, -24, -25, -26, -27, -27, -28, -29, -30, -30, -31
        .byte    -31, -32, -33, -33, -34, -34, -35, -35, -36, -36, -37, -37
        .byte    -38, -38, -38, -39, -39, -39, -40, -40, -40, -40, -40, -40
        .byte    -41, -41, -41, -41, -41

.section .text

.macro GET_R x, y, t
        ## r = x * cos(t) + y * sin(t)
        ## Possibly more code needed here to process multiplication results
        imul    \x, [cosTable + \t]
        imul    \y, [sinTable + \t]
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

