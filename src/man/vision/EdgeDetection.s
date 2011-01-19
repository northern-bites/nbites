.intel_syntax noprefix


	## _sobel_operator(uint16_t *yimg, uint16_t  *out)
_sobel_operator:
	

	## Assumes that eax has the address of where we write to memory
yGradient:
	## Load upper row and unpack
	## mm1: | y03 | y02 | y01 | y00 |
	movq 		mm0, [eax]
	movq		mm1, mm0

	## Load the lower row
	## mm3: | y13 | y12 | y11 | y10 |
	movq		mm2, [eax]
	movq		mm3, mm2

	## Compute the difference between the rows
	## mm3: | y3 | y2 | y1 | y0 | diffs
	psubsw		mm3, mm1

	## mm1 after pshufw: | y0 | y3 | y2 | y1 |
	pshufw		mm1, mm3, 0b00111001

	## mm1 after addition:
	## 	 | y0 + y3 | y3 + y2 | y2 + y1 | y0 + y1 |
	paddw		mm1, mm3

	## mm3 after 2nd pshufw:
	## 	| y0 + y1 | y0 + y3 | y3 + y2 | y2 + y1 |
	pshufw		mm3, mm1, 0b00111001

	## mm3:	| xxx | xxx | y1 + 2*y2 +  y3 | y2 + 2*y1 + y0 |
	paddw		mm3, mm1

	## Each time write out the 2 LSW
	movd		[mem], mm3

xGradient:
	## top row in mm0
	## middle row in mm2
	## Unpack bottom row
	movq		mm4, [mem]

	## Add middle to top, twice
	paddw		mm1, mm3
	paddw		mm1, mm3

	## Add bottom to accumulator
	paddw		mm1, mm5

	## Shuffle words and subtract to get gradient value
	pshufw		mm3, mm1, 0b00001110 # Rearrange from |4|3|2|1| to |1|1|4|3|

	## mm3 after subtract:
	## | xxx | xxx | 4 - 2 (diff over 3) | 3 - 1 (diff over 2) |
	psubssw		mm3, mm1    # IS THIS ORDER RIGHT? WANT TO BE LEFT TO RIGHT

	## Write out to memmory
	movd		[mem], mm3
