; ***********************
; *                     *
; *  Run-length Coding  *
; *                     *
; ***********************
;
; void runLengthU16(uint16_t* source, int count, int thresh, int32_t* runs);
;
; Scan array of unsigned words for values above the specified threshold. Place
; in the runs array the offset of each such word found, terminated by -1.
;
; Source words are processed 16 at a time in parallel. runLengthU16 processes
; a multiple of 16 words, and so will read beyond the end of source if
; count is not a multiple of 16. If the memory does not exist, a fault will
; occur.
;
; The maximum size needed for runs is count, rounded up to a multiple of 16,
; plus 1 for the terminator.
;
; runs will not have any garbage due to the overreading, but some data in runs
; beyond the terminator may be overwritten if count is not a multiple of 16.

	PUBLIC	__runLengthU16, __runLengthU8
	.MODEL	FLAT

; *******************************
; *                             *
; *  Stack Frame and Arguments  *
; *                             *
; *******************************

Args	STRUCT
saves	DWORD 4 dup (?)		; 3 saved registers plus return address
source	DWORD ?
count	DWORD ?
thresh	DWORD ?
runs	DWORD ?
Args	ENDS

; ***************************************
; *                                     *
; *  Macro to Create an RLC Subroutine  *
; *                                     *
; ***************************************

rlc	MACRO	setup, getBits
	LOCAL	rlc16, nxtBit, noBits, over, done

; Preserve the required registers: ebx, esi, edi. Not using ebp.
	push	ebx
        push    esi
        push    edi

; Fetch and extend the threshold
	mov	eax, [esp + Args.thresh]
	movd	xmm2, eax

	setup

	mov	esi, [esp + Args.source]
	mov	edi, [esp + Args.runs]
	xor	edx, edx

; process next 16 values
rlc16:	getBits

; Scan the bits for run-length coding
	or	eax, eax		; any bits set?
	je	noBits
	lea	ebx, [edx - 1]		; yes, ebx counts bit position
nxtBit:	bsf	ecx, eax		; find next bit set
	add	ecx, 1
	add	ebx, ecx		; get index of corresponding word
	mov	[edi], ebx		; write it to runs
	add	edi, 4
	shr	eax, cl			; shift out the bit and all 0's before it
	jne	nxtBit			; continue if any 1's left

noBits:	add	edx, 16			; update base index
	cmp	edx, [esp + Args.count]	; done?
	jl	rlc16

; Remove indices beyond count
over:	cmp	edi, [esp + Args.runs]
	je	done
	mov	eax, [edi - 4]
	cmp	eax, [esp + Args.count]
	jl	done
	sub	edi, 4
	jmp	over

; write terminator
done:	mov	eax, -1
	mov	[edi], eax

; Restore necessary _cdecl calling convention registers
        pop     edi
        pop     esi
	pop	ebx

	ret
	ENDM

	.CODE

; *********************
; *                   *
; *  Unsigned 16-bit  *
; *                   *
; *********************

setu16	MACRO
	pshuflw	xmm2, xmm2, 0
	punpcklwd xmm2, xmm2

	pcmpeqw	xmm3, xmm3
	psllw	xmm3, 15
	pxor	xmm2, xmm3			; for unsigned compare
	ENDM

u16Bits	MACRO
	movdqu	xmm0, [esi]		; fetch
	movdqu	xmm1, [esi + 16]
	add	esi, 32
	pxor	xmm0, xmm3		; make comparison unsigned
	pxor	xmm1, xmm3
	pcmpgtw	xmm0, xmm2		; compare to theshold
	pcmpgtw	xmm1, xmm2
	packsswb xmm0, xmm1		; pack results into 16 bytes
	pmovmskb eax, xmm0		; copy sign bits to eax
	ENDM

__runLengthU16:
	rlc	setu16, u16Bits

; ********************
; *                  *
; *  Unsigned 8-bit  *
; *                  *
; ********************

setu8	MACRO
	punpcklbw xmm2, xmm2
	pshuflw	xmm2, xmm2, 0
	punpcklwd xmm2, xmm2

	pcmpeqb	xmm3, xmm3
	pabsb	xmm3, xmm3
	psllw	xmm3, 7
	pxor	xmm2, xmm3			; for unsigned compare
	ENDM

u8Bits	MACRO
	movdqu	xmm0, [esi]		; fetch
	add	esi, 16
	pxor	xmm0, xmm3		; make comparison unsigned
	pcmpgtb	xmm0, xmm2		; compare to theshold
	pmovmskb eax, xmm0		; copy sign bits to eax
	ENDM

__runLengthU8:
	rlc	setu8, u8Bits

	END
