;// *********************************
;// *                               *
;// *  Pixel Marking for Fast Blob  *
;// *                               *
;// *********************************

; void connectMark(unsigned char* image, int width, int height, int pitch, int thresh);
;
; pitch must be a multiple of 16 or a GP exception will occur
; There are no alignment restrictions, and all pixel accesses are arranged to be DQ aligned
; width must be >= 17 - (image & 15) or no marking will occur
;
; In order to maintain DQ alignment for pixel access without imposing any alignment restrictions,
; each row is processed as one partial left DQ group, zero or more complete DQ groups, and one partial
; right DQ group. The partial groups mark from 1 to 16 pixels, and the complete groups mark all
; 16. The image address and width determine the size of the partial groups and number of
; complete groups. Partial groups are processed by the same set of intructions as the complete
; groups, but with modified masks so they don't change the pixels outside the bounds of the
; image. Note that because cache lines are larger than and aligned with DQ, those pixels are
; guaranteed to exist and so can be read and written. We just have to avoid modifying them.

        public __connectMark
        .model flat

; Arguments accessed via esp
Args	STRUCT
saveEbp	DWORD ?			; saved ebp
retAdr	DWORD ?			; return address
image	DWORD ?			; address of source image
wd	DWORD ?			; image width, low 4 bits ignored and assumed 0
ht	DWORD ?			; image height
pitch	DWORD ?			; image pitch in bytes
thr	DWORD ?			; threshold
Args	ENDS

; Local variables
Locals	STRUCT
maskL1	XMMWORD ?	; modified FE mask (xmm2) for left  partial group
maskL2	XMMWORD ?	; modified 01 mask (xmm5) for left  partial group
maskR1	XMMWORD ?	; modified FE mask (xmm2) for right partial group
maskR2	XMMWORD ?	; modified 01 mask (xmm5) for right partial group
Locals	ENDS

;// *********************************
;// *                               *
;// *  Macro for Marking 16 Pixels  *
;// *                               *
;// *********************************

; xmm0	holds pixels being marked
; xmm1	compute mark bits to OR into xmm0
; xmm2	0xFE to clear low bit
; xmm3	(thr ^ 0x80), converted to excess-128 for signed comparison
; xmm5	0x01 to mask bit 0 of compare results
; xmm6	0x80 for conversion of pixels to excess-128

mark	MACRO address, clearMask, setMask
	movdqa	xmm0, address		; Fetch 16 pixels
	pand	xmm0, clearMask		; Clear LSB

	movdqa	xmm1, xmm0		; Copy pixels
	pxor	xmm1, xmm6		; Convert to excess-128 for signed compare
	pcmpgtb	xmm1, xmm3		; Compare to threshold
	pand	xmm1, setMask		; Keep just bit 0 of results
	por	xmm0, xmm1		; Set bit 0 in source pixels
	
	movdqa	address, xmm0		; Update marked pixels

	ENDM

;// *****************
;// *               *
;// *  Entry Point  *
;// *               *
;// *****************

	.CODE
__connectMark:
	push	ebp
	mov	ebp, esp
	sub	esp, SIZEOF Locals
	and	esp, 0FFFFFFF0h
	pxor	xmm0, xmm0		; xmm0 = 0 for pshufb to copy low byte to all bytes

	mov	eax, 0FEh		; FC mask
	movd	xmm2, eax
	pshufb	xmm2, xmm0

	mov	eax, [ebp + Args.thr]	; threshold
	xor	eax, 80h
	movd	xmm3, eax
	pshufb	xmm3, xmm0

	mov	eax, 1			; 01 mask
	movd	xmm5, eax
	pshufb	xmm5, xmm0

	mov	eax, 80h		; 80 for excess-128
	movd	xmm6, eax
	pshufb	xmm6, xmm0

; Make left partial masks, update image address and width
	movdqa	[esp + Locals.maskL1], xmm2
	movdqa	[esp + Locals.maskL2], xmm5
	mov	ecx, [ebp + Args.image]
	and	ecx, 15
	mov	eax, 16
	sub	eax, ecx
	add	[ebp + Args.image], eax
	sub	[ebp + Args.wd], eax
	xor	eax, eax
	mov	edx, 255
	jmp	mle	
ml:	mov	BYTE PTR[esp + ecx + Locals.maskL1], dl
	mov	BYTE PTR[esp + ecx + Locals.maskL2], al
mle:	dec	ecx
	jge	ml
	
; make right partial masks, update width again
	movdqa	[esp + Locals.maskR1], xmm2
	movdqa	[esp + Locals.maskR2], xmm5
	mov	ecx, [ebp + Args.wd]
	mov	eax, 16
	and	ecx, 15
	cmove	ecx, eax
	sub	[ebp + Args.wd], ecx
	jl	quit
	sub	ecx, eax
	jz	mre
	xor	eax, eax
mr:	mov	BYTE PTR[esp + ecx + Locals.maskR1 + 16], dl
	mov	BYTE PTR[esp + ecx + Locals.maskR2 + 16], al
	inc	ecx
	jl	mr
mre:	

; eax	address of end of current set of complete groups
; ecx	complete group counter

	mov	eax, [ebp + Args.image]
	add	eax, [ebp + Args.wd]
	
;// ***************************
;// *                         *
;// *  Outer and Inner Loops  *
;// *                         *
;// ***************************

; Outer loop, mark left partial group
outer:	mov	ecx, [ebp + Args.wd]
	neg	ecx
	mark	[eax + ecx - 16], [esp + Locals.maskL1], [esp + Locals.maskL2]
	je	oEnd			; requires that mark does not change condition codes

; Inner loop, mark complete groups
inner:	mark	[eax + ecx], xmm2, xmm5
	add	ecx, 16			; Loop counter
	jl	inner

; End of outer loop, mark right partial group
oEnd:	mark	[eax], [esp + Locals.maskR1], [esp + Locals.maskR2]
	add	eax, [ebp + Args.pitch]
	dec	[ebp + Args.ht]
	jg	outer

; return
quit:	mov	esp, ebp
	pop	ebp
	ret

	END
