;// *********************************
;// *                               *
;// *  Image Acquisition Front End  *
;// *                               *
;// *********************************
;
; Microsoft MASM syntax

        public __acquire_image
        .model flat
        .data

multiPhase	= 1	; enable two-phase processing (significant improvement)
ntStore		= 1	; enable non-temporal stores, only for Y values in multiPhase mode (unclear improvement)
prefetchSrc	= 1	; enable prefetching (significant improvement)
colorTable	= 1	; enable color table lookup

;// ***************************
;// *                         *
;// *  Structure Definitions  *
;// *                         *
;// ***************************

; Color parameters; must match C++ structs. See C++ for more info.
Color	STRUCT
dark0	XMMWORD ?
yCoeff	XMMWORD ?
fuzzy	XMMWORD ?
invFuz	XMMWORD ?
Color	ENDS

Colors	STRUCT
white	Color {}
green	Color {}
orange	Color {}
Colors	ENDS

; Local variables on stack
Locals	STRUCT
cParams	Colors {}		; copy of color parameters, DQ aligned, saves a general register
orInvV	XMMWORD ?		; invert V values by XOR for orange
uvZero	XMMWORD ?		; UV zero value (256)
time	DWORD ?			; starting time from rdtsc
srcUpd	DWORD ?			; source image row update value in bytes
  IF	colorTable
dPitch	DWORD ?			; width * height is destination image pitch
	ALIGN	16
tableK	XMMWORD ?		; combine U and V table indicies with pmaddwd
tabIdx	DWORD 640 dup (?)	; Table indicies stored during main processing loop. Room for up to 640
				; wide destination image. Use fixed size to make addressing easier.
				; These are processed in a separate loop because we don't have enough
				; GP registers.
  ENDIF
Locals	ENDS

; Arguments accessed via ebp
Args	STRUCT
saves	DWORD 5 dup (?)		; 4 saved registers plus return address
source	DWORD ?			; address of source image
dstWd	DWORD ?			; destination image width, low 3 bits ignored and assumed 0
dstHt	DWORD ?			; destination image height
pitch	DWORD ?			; source image pitch in bytes
cParams	DWORD ?			; address of colors parameters
dest	DWORD ?			; address of destination images
  IF	colorTable
table	DWORD ?			; address of color lookup table
  ENDIF
Args	ENDS

;// *****************
;// *               *
;// *  Entry Point  *
;// *               *
;// *****************

	.CODE

__acquire_image:
; Preserve the required registers: ebp, ebx, esi, edi
        push    ebp
        push    ebx
        push    esi
        push    edi
        mov     ebp, esp

; Allocate local variables, ensure that the stack pointer is DQ aligned
        sub     esp, SIZEOF Locals 
        and     esp, 0FFFFFFF0h

; Read and save starting time
        rdtsc
        mov     [esp + Locals.time], eax

; Copy colors to stack for access with esp and to insure DQ alignment
	mov	esi, [ebp + Args.cParams]
	lea	edi, [esp + Locals.cParams]
	mov	ecx, -SIZEOF Colors
copy:	movdqu	xmm0, [esi + ecx + SIZEOF Colors]
	movdqa	[edi + ecx + SIZEOF Colors], xmm0
	add	ecx, 16
	jne	copy

; Make the constant to invert orange V so fuzzy max can be used (0x1FF in V words)
	pcmpeqb	xmm0, xmm0
	psrld	xmm0, 23
	pslld	xmm0, 16
	movdqa	[esp + Locals.orInvV], xmm0

; Make uvZero: 256 in all words
	pcmpeqb	xmm0, xmm0
	psrlw	xmm0, 15
	psllw	xmm0, 8
	movdqa	[esp + Locals.uvZero], xmm0

; Make the color table index constant. 128*128 in U words, 128 in V words
  IF	colorTable
	pcmpeqb	xmm0, xmm0
	psrld	xmm0, 31
	movdqa	xmm1, xmm0
	pslld	xmm0, 14
	pslld	xmm1, 23
	por	xmm0, xmm1
	movdqa	[esp + Locals.tableK], xmm0
  ENDIF

; Make the source row update constant
	mov	eax, [ebp + Args.pitch]		; two rows
	shl	eax, 1
	mov	ebx, [ebp + Args.dstWd]		; minus bytes moved by inner loop
	shl	ebx, 2
	sub	eax, ebx
	mov	[esp + Locals.srcUpd], eax

;// ********************************
;// *                              *
;// *  Main Processing loop Macro  *
;// *                              *
;// ********************************

; eax	source row pitch
; ebx	destination image pitch
; ecx	inner loop counter, ascending from -width to 0
; edx	-> destination white image
; esi	-> source top row
; edi	-> destination Y image

; phase is 0 or 1
xGroup	MACRO	phase
; fetch next 4 top row pixels, split into Y, UV
; xmm0 = | Y07 | Y06 | Y05 | Y04 | Y03 | Y02 | Y01 | Y00 |
; xmm1 = | V03 | U03 | V02 | U02 | V01 | U01 | V00 | U00 |
	movdqu	xmm0, [esi + 16*phase]
	movdqa	xmm1, xmm0
	psllw	xmm0, 8
	psrlw	xmm0, 8
	psrlw	xmm1, 8

; fetch next 4 bottom row pixels, split into Y, UV
; xmm2 = | Y17 | Y16 | Y15 | Y14 | Y13 | Y12 | Y11 | Y10 |
; xmm3 = | V13 | U13 | V12 | U12 | V11 | U11 | V10 | U10 |
	movdqu	xmm2, [esi + eax + 16*phase]

; here is a good place to prefetch the next two rows
  IF	prefetchSrc
	prefetcht1 [esi + eax*2]
	add	esi, eax
	prefetcht1 [esi + eax*2]
	sub	esi, eax
  ENDIF

; Update the source pointer in esi. We do it just after needing it for the last
; time in this loop iteration to cooperate with the CPU pipeline and OOO
; execution. 
  IF	multiPhase
    IF	phase eq 1
	add	esi, 32
    ENDIF
  ELSE
	add	esi, 16
  ENDIF

; Now we can finish splitting up the bottom row pixels
	movdqa	xmm3, xmm2
	psllw	xmm2, 8
	psrlw	xmm2, 8
	psrlw	xmm3, 8

; sum 4 sets of 4 Y pixels. We will use the fact that the even words equal the odd words
; for later UV processing
; xmm0 = | Y06+Y07+Y16+Y17 | Y06+Y07+Y16+Y17 | Y04+Y05+Y14+Y15 | Y04+Y05+Y14+Y15 | Y02+Y01+Y12+Y11 | Y02+Y03+Y12+Y13 | Y00+Y01+Y10+Y11 | Y00+Y01+Y10+Y11 |
	paddw	xmm0, xmm2
	pshuflw	xmm2, xmm0, 10110001b
	pshufhw	xmm2, xmm2, 10110001b
	paddw	xmm0, xmm2

; Write 4 Y pixels.
  IF	multiPhase
    IF	phase eq 0
	movdqa	xmm4, xmm0			; multiphase phase 0, save for later
	psrld	xmm4, 16
    ELSE
	movdqa	xmm2, xmm0			; multiphase phase 1, combine with saved and write to destination
	psrld	xmm2, 16
	packssdw xmm4, xmm2
      IF ntStore
	movntdq	XMMWORD PTR[edi + ecx*2], xmm4
      ELSE
	movdq	XMMWORD PTR[edi + ecx*2], xmm4
      ENDIF
    ENDIF

  ELSE
	movdqa	xmm2, xmm0			; single phase, write to destination
	psrld	xmm2, 16
	packssdw xmm2, xmm2
	movq	QWORD PTR[edi + ecx*2], xmm2	; movntq is for MMX only
  ENDIF

; sum 4 sets of 2 UV pixels
; xmm1 = | V03+V13 | U03+U13 | V02+V12 | U02+U12 | V01+V11 | U01+U11 | V00+V10 | U00+U10 |
	paddw	xmm1, xmm3

; white color
	movdqa	xmm2, xmm0					; change in dark0 due to y
	pmulhw	xmm2, [esp + Locals.cParams.white.yCoeff]
	paddsw	xmm2, [esp + Locals.cParams.white.dark0]	; new dark0
	movdqa	xmm3, xmm1					; recenter UV round 0 for absolute value
	psubw	xmm3, [esp + Locals.uvZero]
	pabsw	xmm3, xmm3					; absolute value
	paddw	xmm3, [esp + Locals.uvZero]			; recenter around uvZero for fuzzy calcs
	psubusw	xmm2, xmm3					; max(t0 - |UV|, 0)
	pminsw	xmm2, [esp + Locals.cParams.white.fuzzy]	; min(max(t0 - |UV|, 0), fuzzy)
	pmullw	xmm2, [esp + Locals.cParams.white.invFuz]	; min(max(t0 - |UV|, 0), fuzzy) * invFuz) >> 8
	pshuflw	xmm3, xmm2, 10110001b				; swap U and V for fuzzy AND (min)
	pshufhw	xmm3, xmm3, 10110001b
	psrld	xmm2, 24					; this is the >> 8, but extra 16 for alignment
	psrld	xmm3, 24					; this is the >> 8, but extra 16 for alignment
	pminsw	xmm2, xmm3					; Fuzzy AND

  IF	multiPhase
    IF	phase eq 0
	movdqa	xmm5, xmm2		; multiphase phase 0, save 4 white pixels
    ELSE
	packssdw xmm5, xmm2		; multiphase phase 1, combine and write 8 white pixels
	packuswb xmm5, xmm5
	movq	QWORD PTR[edx], xmm5
    ENDIF
  ELSE
	packssdw xmm2, xmm2		; single phase, write 4 white pixels
	packuswb xmm2, xmm2
	movd	DWORD PTR[edx], xmm2
  ENDIF

; green color
	movdqa	xmm2, xmm0					; change in dark0 due to y
	pmulhw	xmm2, [esp + Locals.cParams.green.yCoeff]
	paddsw	xmm2, [esp + Locals.cParams.green.dark0]	; new dark0
	psubusw	xmm2, xmm1					; max(t0 - |UV|, 0)
	pminsw	xmm2, [esp + Locals.cParams.green.fuzzy]	; min(max(t0 - |UV|, 0), fuzzy)
	pmullw	xmm2, [esp + Locals.cParams.green.invFuz]	; min(max(t0 - |UV|, 0), fuzzy) * invFuz) >> 8
	pshuflw	xmm3, xmm2, 10110001b				; swap U and V for fuzzy AND (min)
	pshufhw	xmm3, xmm3, 10110001b
	psrld	xmm2, 24					; this is the >> 8, but extra 16 for alignment
	psrld	xmm3, 24					; this is the >> 8, but extra 16 for alignment
	pminsw	xmm2, xmm3					; Fuzzy AND

  IF	multiPhase
    IF	phase eq 0
	movdqa	xmm6, xmm2		; multiphase phase 0, save 4 green pixels
    ELSE
	packssdw xmm6, xmm2		; multiphase phase 1, combine and write 8 green pixels
	packuswb xmm6, xmm6
	movq	QWORD PTR[edx + ebx], xmm6
    ENDIF
  ELSE
	packssdw xmm2, xmm2		; single phase, write 4 green pixels
	packuswb xmm2, xmm2
	movd	DWORD PTR[edx + ebx], xmm2
  ENDIF

; orange color
	movdqa	xmm2, xmm0					; change in dark0 due to y
	pmulhw	xmm2, [esp + Locals.cParams.orange.yCoeff]
	paddsw	xmm2, [esp + Locals.cParams.orange.dark0]	; new dark0
	movdqa	xmm3, xmm1					; invert V to make it max instead of min
	pxor	xmm3, [esp + Locals.orInvV]
	psubusw	xmm2, xmm3					; max(t0 - |UV|, 0)
	pminsw	xmm2, [esp + Locals.cParams.orange.fuzzy]	; min(max(t0 - |UV|, 0), fuzzy)
	pmullw	xmm2, [esp + Locals.cParams.orange.invFuz]	; min(max(t0 - |UV|, 0), fuzzy) * invFuz) >> 8
	pshuflw	xmm3, xmm2, 10110001b				; swap U and V for fuzzy AND (min)
	pshufhw	xmm3, xmm3, 10110001b
	psrld	xmm2, 24					; this is the >> 8, but extra 16 for alignment
	psrld	xmm3, 24					; this is the >> 8, but extra 16 for alignment
	pminsw	xmm2, xmm3					; Fuzzy AND

  IF	multiPhase
    IF	phase eq 0
	movdqa	xmm7, xmm2		; multiphase phase 0, save 4 orange pixels
    ELSE
	packssdw xmm7, xmm2		; multiphase phase 1, combine and write 8 orange pixels
	packuswb xmm7, xmm7
	movq	QWORD PTR[edx + ebx*2], xmm7
	add	edx, 8
    ENDIF
  ELSE
	packssdw xmm2, xmm2		; single phase, write 4 orange pixels
	packuswb xmm2, xmm2
	movd	DWORD PTR[edx + ebx*2], xmm2
	add	edx, 4
  ENDIF

; Compute and save color table indicies
  IF	colorTable
	psrld	xmm0, 19			; high 7 bits of 4 Y values in dowrds
	psrlw	xmm1, 2				; high 7 bits of 4 U and 4 V values in words
	pmaddwd	xmm1, [esp + Locals.tableK]	; combine U and U
	paddd	xmm0, xmm1			; and Y for 3D table index
	movdqa	[esp + ecx*4 + SIZEOF Locals + 16 * phase], xmm0	; save on stack for later
  ENDIF

	ENDM

;// ****************
;// *              *
;// *  Outer Loop  *
;// *              *
;// ****************

; eax	source row pitch
; ebx	destination image pitch
; ecx	inner loop counter, ascending from -width to 0
; edx	-> destination white image
; esi	-> source top row
; edi	-> destination Y image

	mov	eax, [ebp + Args.pitch]
	mov	ebx, [ebp + Args.dstWd]
	imul	ebx, [ebp + Args.dstHt]
	mov	edi, [ebp + Args.dest]
	lea	edx, [edi + ebx*2]
	mov	esi, [ebp + Args.source]

  IF	colorTable
	mov	[esp + Locals.dPitch], ebx
  ENDIF

; Outer loop
yLoop:
	mov	ecx, [ebp + Args.dstWd]
	lea	edi, [edi + ecx*2]
	neg	ecx

;// *********************
;// *                   *
;// *  Inner Main Loop  *
;// *                   *
;// *********************

; Inner loop
xLoop:	xGroup	0
  IF	multiPhase
	xGroup	1
	add	ecx, 8
  ELSE
	add	ecx, 4
  ENDIF
	jl	xLoop

; end of inner loop
	add	esi, [esp + Locals.srcUpd]

;// ****************************
;// *                          *
;// *  Inner Color Table Loop  *
;// *                          *
;// ****************************
;
; eax	hold table index and byte fetched from table
; ebx	-> color table
; ecx	counter, ascending from -width to 0
; edx	-> end of current output row
; 
cGroup MACRO phase
	mov     eax, [esp + (SIZEOF Locals) + ecx*4 + phase*4]	; Load the color address from the stack
	movzx   eax, BYTE PTR[ebx + eax]		; Lookup color in table
        mov     BYTE PTR[edx + ecx + phase], al		; write it out
        ENDM

  IF	colorTable
	mov	ecx, [ebp + Args.table]
	cmp	ecx, 0
	je	noTable

	mov	ecx, [ebp + Args.dstWd]
	neg	ecx
	lea	eax, [ebx + ebx*2]
	add	edx, eax
	mov	ebx, [ebp + Args.table]
cLoop:	cGroup	0
	cGroup	1
	cGroup	2
	cGroup	3
	add	ecx, 4
	jl	cLoop

	mov	ebx, [esp + Locals.dPitch]	; restore main loop's registers
	lea	eax, [ebx + ebx*2]
	sub	edx, eax
	mov	eax, [ebp + Args.pitch]
noTable:
  ENDIF

;// **********************************
;// *                                *
;// *  Enf of Outer Loop and Return  *
;// *                                *
;// **********************************

	dec	[ebp + Args.dstHt]
	jg	yLoop

        rdtsc
        sub     eax, [esp + Locals.time]

; Restore necessary _cdecl calling convention registers
        mov     esp, ebp
        pop     edi
        pop     esi
        pop     ebx
        pop     ebp

	ret

	END
