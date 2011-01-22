.intel_syntax noprefix


.globl _sobel_operator

.section .data

	.struct 0
x_output:
	.struct x_output + 4
y_output:
	.struct y_output + 4
row_count:

.equiv top_of_stack, row_count + 4


.section .text

	## _sobel_operator(uint16_t *yimg, uint16_t  *outX, uint16_t *outY)
_sobel_operator:
	push 	ebp
	mov	ebp, esp

	push 	esi
	push 	edi

	sub	esp, top_of_stack

	## Load arguments into registers and onto the stack

	## We have to move destination registers to the ends of the next row
	##
	## | o | o | o | <- source comes from this row
	## | o | Q | o | <- destination needs to write to this row, plus one byte forward for 'Q' gradient value
	## | o | o | o |
	mov	esi, [ebp+8]
	add	esi, 320 * 2	# Move pointer to end of row

	mov	eax, [ebp+12]
	add	eax, 320 * 2 * 2 + 2
	mov	dword ptr[esp + x_output], eax

	mov	eax, [ebp+16]
	add	eax, 320 * 2 * 2 + 2
	mov	dword ptr[esp + y_output], eax

	mov	dword ptr[esp + row_count], 238 # Actually only does from top row through third to bottom
yLoop:

	## Two pixels processed each time
	mov	ecx, -636
xLoop:

	## Put y gradient output destination in register
	mov	edi, [esp + y_output]

	## Assumes that eax has the address of where we write to memory
yGradient:
	## Load upper row and unpack
	## mm1: | y03 | y02 | y01 | y00 |
	movq 	mm0, [esi + ecx-4]
	movq	mm1, mm0

	## Load the lower row
	## mm3: | y13 | y12 | y11 | y10 |
	movq	mm2, [esi + ecx-4 + 320 * 2 * 2] # Two rows after top row
	movq	mm3, mm2

	## Compute the difference between the rows
	## mm3: | y3 | y2 | y1 | y0 | diffs
	psubsw	mm3, mm1

	## mm1 after pshufw: | y0 | y3 | y2 | y1 |
	pshufw	mm1, mm3, 0b00111001

	## mm1 after addition:
	## 	 | y0 + y3 | y3 + y2 | y2 + y1 | y0 + y1 |
	paddw	mm1, mm3

	## mm3 after 2nd pshufw:
	## 	| y0 + y1 | y0 + y3 | y3 + y2 | y2 + y1 |
	pshufw	mm3, mm1, 0b00111001

	## mm3:	| xxx | xxx | y1 + 2*y2 +  y3 | y2 + 2*y1 + y0 |
	paddw	mm3, mm1

	## Each time write out the 2 LSW
	movd	[edi + ecx-4], mm3

	## Put x gradient output destination in register
	mov	edi, [esp + x_output]

xGradient:
	## top row in mm0
	## bottom row in mm2
	## Unpack middle row
	movq	mm4, [esi + ecx-4 + 320*2]

	## Add middle to top, twice
	paddw	mm0, mm4
	paddw	mm0, mm4

	## Add bottom to accumulator
	paddw	mm0, mm2

	## Shuffle words and subtract to get gradient value
	pshufw	mm3, mm0, 0b01001110 # Rearrange from |4|3|2|1| to |2|1|4|3|

	## mm3 after subtract:
	## | xxx | xxx | 4 - 2 (diff over 3) | 3 - 1 (diff over 2) |
	psubsw	mm3, mm0

	## Write out to memmory
	movd	[edi + ecx-4], mm3

	## xLoop finish
	add	ecx, 4
	jne	xLoop

	## Move source pointers
	add	esi, 320 * 2	# next source row (2 bytes * 320 pixels)

	# next dest rows
	add	dword ptr[esp + x_output], 320 * 2
	add	dword ptr[esp + y_output], 320 * 2

	dec	dword ptr[esp + row_count]
	jne 	yLoop

	## Fix stack
	mov	esp, ebp
	sub	esp, 8

	pop	edi
	pop	esi

	pop	ebp

	emms

	ret
