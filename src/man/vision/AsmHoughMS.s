
; ************************
; *                      *
; *  Smooth Hough Space  *
; *                      *
; ************************
;
; void houghSmooth(uint16_t* image, int dstWd, int dstHt, int pitch);
;
; In-place smooth Hough space using kernel:
;
;	1 2 1
;	2 4 2
;	1 2 1
;
; image, dstWd, and dstHt specify the result image, which will contain valid
; smoothed values. pitch must be a multiple of 16, but there are no other
; alignment requirements. Will read one row and column beyond result image
; on all sides, which rows and columns must contain valid values. May read
; more that one column beyond left and right sides, values there don't matter.
;
; Smoothed values replace original; the kernel is centered so there is no
; position shift of the result. Will overwrite one row above the result
; image with undefined values. May overwrite columns left and right with
; undefined values. The overwriting is OK because the active region of
; the Hough space is smaller than the allocated bins.

	PUBLIC	__houghSmooth
	.MODEL	FLAT


; *******************************
; *                             *
; *  Stack Frame and Arguments  *
; *                             *
; *******************************

Locals	STRUCT
rowBufs	XMMWORD ?
Locals	ENDS

Args	STRUCT
saves	DWORD 5 dup (?)		; 4 saved registers plus return address
image	DWORD ?
dstWd	DWORD ?
dstHt	DWORD ?
pitch	DWORD ?
Args	ENDS

; *****************
; *               *
; *  Entry Point  *
; *               *
; *****************

	.CODE
__houghSmooth:
; Preserve the required registers: ebp, ebx, esi, edi
        push    ebp
        push    ebx
        push    esi
        push    edi
        mov     ebp, esp

; Make width and source address DQ aligned
	mov	esi, [ebp + Args.image]
	mov	ecx, esi
	and	ecx, 0Fh
	sub	esi, ecx
	shr	ecx, 1
	add	ecx, [ebp + Args.dstWd]
	add	ecx, 7
	and	ecx, 0FFFFFFF8h
	mov	[ebp + Args.dstWd], ecx

; Allocate local variables, ensure that the stack pointer is DQ aligned
	neg	ecx
        lea	esp, [esp + ecx*4]
        and     esp, 0FFFFFFF0h

; Top and mid row addresses (point to end of buffers)
	mov	eax, [ebp + Args.dstWd]
	lea	edx, [esp + eax*2]
	lea	edi, [esp + eax*4]

; pitch
	mov	eax, [ebp + Args.pitch]
	add	eax, eax
	neg	eax

; Prefill top
	add	esi, eax
fill:	movdqa	xmm0, [esi]
	movdqu	xmm1, [esi - 2]
	movdqu	xmm2, [esi + 2]
	add	esi, 16
	paddw	xmm0, xmm0
	paddw	xmm1, xmm2
	paddw	xmm0, xmm1
	movdqa	[edi + ecx*2], xmm0
	add	ecx, 8
	jl	fill

; **********************
; *                    *
; *  Outer/Inner Loop  *
; *                    *
; **********************
;
;  eax	-pitch
;  ebx
;  ecx	inner loop counter
;  edx	top buffer address
;  esi	source address
;  edi	mid buffer address
yLoop:
	sub	esi, eax
	mov	ecx, [ebp + Args.dstWd]
	neg	ecx
	lea	esi, [esi + ecx*2]

xLoop:
; Next 121 smooth in x, bottom row
	movdqa	xmm0, [esi]
	movdqu	xmm1, [esi - 2]
	movdqu	xmm2, [esi + 2]
	paddw	xmm0, xmm0
	paddw	xmm1, xmm2
	paddw	xmm0, xmm1

	movdqa	xmm1, [edx + ecx * 2]
	movdqa	[edx + ecx * 2], xmm0
	paddw	xmm0, xmm1
	movdqa	xmm1, [edi + ecx * 2]
	paddw	xmm1, xmm1
	paddw	xmm0, xmm1

	movdqa	[esi + eax], xmm0
	add	esi, 16

; ******************
; *                *
; *  End of Loops  *
; *                *
; ******************

	add	ecx, 8
	jl	xLoop

; swap top and mid pointers
	xor	edx, edi
	xor	edi, edx
	xor	edx, edi

	dec	[ebp + Args.dstHt]
	jge	yLoop

; **********
; *        *
; *  Done  *
; *        *
; **********

; Restore necessary _cdecl calling convention registers
        mov     esp, ebp
        pop     edi
        pop     esi
        pop     ebx
        pop     ebp

	ret

	END
