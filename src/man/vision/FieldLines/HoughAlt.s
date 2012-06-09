/* -*- mode: asm; indent-tabs-mode: nil -*- */
.intel_syntax noprefix

#	********************************
#	*			       *
#	*  Hough Transform Main Loops  *
#	*			       *
#	********************************
#
# Strategy:
# For each edge, a pattern of Hough bins in angleCount = 2 * angleSpread Hough space rows
# are incremented. The pattern might look like this, for example:
#
# -----> R                      Span   Inc
# | . . . . . . . . . . . . .
# | . . x x . . . . . . . . .     2     2
# | . . . x x x . . . . . . .     4     2
# T . . . . . x x . . . . . .     2     2
#   . . . . . . x x . . . . .     2     2
#   . . . . . . . x . . . . .     0     2
#   . . . . . . . x . . . . .     0     2
#   . . . . . . x x . . . . .    -2    -2
#   . . . . . x x . . . . . .    -2    -2
#   . . . x x x . . . . . . .    -4    -2
#   . . x x . . . . . . . . .    -2    -2
#   . . . . . . . . . . . . .
#
# Each consecutive run of Hough bins in one row (one value of T, sequential values of R)
# is called an R-run. The starting R for the R-run at T is the same as the ending R for
# the R-run at T-1, so that when incrementing bins, there are only three possibilities
# for getting the address of the next bin: +2 (next R), -2 (prev R), or +2*houghWd (next T).
# This means that a Hough space address is calculated only once per edge, and from there
# it is updated by an add instruction. The "2" here is because Hough bins are 2 bytes.
#
# The integer unit (IU) handles updating the address, incrementing the bins, and prefetching
# the next R-run. The span and inc values needed by IU are calculated by the MMX unit, since
# it has better instructions for the purpose (particularly pmaddwd, which does two dot
# products) and can do two calculations in parallel. By mingling IU and MMX instructions,
# we can take advantage of the separate IU and MMX pipelines on the Geode, as well as
# address calculation delays (updating a register and then using it in an address calculation
# too soon). In order to avoid the significant penalties of communicating MMX results to IU,
# we implement a two-stage software pipeline using a pair of ping-pong buffers to hold the
# span and inc values. While IU is working on the current edge, MMX is working on the next
# edge, so they don't interfere with each other. MMX stores its results in one of the
# ping-pong buffers while IU reads from the other. At the end of each edge, the ping-pong
# buffers are exchanged (by exchanging pointers in registers).

#
# Constants
#
	.equiv	angleSpread, 5
	.equiv	angleCount, 2 * angleSpread
        .equiv  pfEnable, 0

	.equiv	houghWd, 320	# all values allowed
	.equiv	houghSize, 2	# some code assumes this value
	.equiv	houghPitch, houghSize * houghWd

#	***************************
#	*			  *
#	*  Structure Definitions  *
#	*			  *
#	***************************
#
# Edge structure
#
        .struct 0
edgeT:	.skip	2
edgeX:	.skip	2
edgeY:	.skip	2
edgeSize:

#
# Ping/pong buffer structure
#
	.struct 0
ppAddr:	.skip	4			# Hough space address of start of first R-run
ppInc:	.skip	4 * angleCount		# Hough address increment, either +2 or -2
ppSpan:	.skip	4 * angleCount		# R-run address difference from first word to last
ppSize:

#
# Stack frame, for esp-based addressing
#
	.struct	0
ppBuf0:		.skip	ppSize		# ping-pong buffers
ppBuf1:		.skip	ppSize
localVarSize:
		.skip	4 * 4		# save 4 registers
		.skip	4		# return address
		# start of arguments
houghSpace:	.skip	4		# address of Hough space
edgeList:	.skip	4		# address of edge list
edgeCount:	.skip	4		# number of edges

#	*******************
#	*		  *
#	*  Sin/Cos Table  *
#	*		  *
#	*******************
#
# sin and cos values are S16.14. angleSpread = 5 is built into this table. The T & Deg
# values are the angles of the rows in Hough space. Based on the implementation of
# arctangent in the edge detection code, each 8-bit binary edge angle T corresponds
# approximately to the range T-0.5 to T+0.5, i.e. T is approximately at the center of
# the range (it is approximate due to the limited precision of the tables). Because
# angleCount is always an even number, the rows of Hough space are shifted by half an
# angle bin. See the example below, where for an edge angle of 10, with angleSpread 5,
# we process 10 rows of Hough space from 5.5 to 14.5, which is evenly spaced around 10.

	.section .data
sincosTable:				# Hough space rows
	#	  cos     sin               T      Deg
	.word	 16284,  -1806		# 251.5   -6.33
	.word	 16324,  -1406		# 252.5   -4.92
	.word	 16353,  -1005		# 253.5   -3.52
	.word	 16373,   -603		# 254.5   -2.11
	.word	 16383,   -201		# 255.5   -0.70
	.word	 16383,    201		#   0.5    0.70
	.word	 16373,    603		#   1.5    2.11
	.word	 16353,   1005		#   2.5    3.52
	.word	 16324,   1406		#   3.5    4.92
	.word	 16284,   1806		#   4.5    6.33  Example
	.word	 16235,   2205		#   5.5    7.73  ------------
	.word	 16176,   2603		#   6.5    9.14    ^
	.word	 16107,   2999		#   7.5   10.55    |
	.word	 16029,   3393		#   8.5   11.95    |
	.word	 15941,   3786		#   9.5   13.36    |  edge T
	.word	 15843,   4176		#  10.5   14.77    |   = 10
	.word	 15736,   4563		#  11.5   16.17    |
	.word	 15619,   4948		#  12.5   17.58    |
	.word	 15493,   5330		#  13.5   18.98    v
	.word	 15357,   5708		#  14.5   20.39  ------------
	.word	 15213,   6084		#  15.5   21.80
	.word	 15059,   6455		#  16.5   23.20
	.word	 14896,   6823		#  17.5   24.61
	.word	 14724,   7186		#  18.5   26.02
	.word	 14543,   7545		#  19.5   27.42
	.word	 14354,   7900		#  20.5   28.83
	.word	 14155,   8250		#  21.5   30.23
	.word	 13949,   8595		#  22.5   31.64
	.word	 13733,   8935		#  23.5   33.05
	.word	 13510,   9269		#  24.5   34.45
	.word	 13279,   9598		#  25.5   35.86
	.word	 13039,   9921		#  26.5   37.27
	.word	 12792,  10238		#  27.5   38.67
	.word	 12537,  10549		#  28.5   40.08
	.word	 12274,  10853		#  29.5   41.48
	.word	 12004,  11151		#  30.5   42.89
	.word	 11727,  11442		#  31.5   44.30
	.word	 11442,  11727		#  32.5   45.70
	.word	 11151,  12004		#  33.5   47.11
	.word	 10853,  12274		#  34.5   48.52
	.word	 10549,  12537		#  35.5   49.92
	.word	 10238,  12792		#  36.5   51.33
	.word	  9921,  13039		#  37.5   52.73
	.word	  9598,  13279		#  38.5   54.14
	.word	  9269,  13510		#  39.5   55.55
	.word	  8935,  13733		#  40.5   56.95
	.word	  8595,  13949		#  41.5   58.36
	.word	  8250,  14155		#  42.5   59.77
	.word	  7900,  14354		#  43.5   61.17
	.word	  7545,  14543		#  44.5   62.58
	.word	  7186,  14724		#  45.5   63.98
	.word	  6823,  14896		#  46.5   65.39
	.word	  6455,  15059		#  47.5   66.80
	.word	  6084,  15213		#  48.5   68.20
	.word	  5708,  15357		#  49.5   69.61
	.word	  5330,  15493		#  50.5   71.02
	.word	  4948,  15619		#  51.5   72.42
	.word	  4563,  15736		#  52.5   73.83
	.word	  4176,  15843		#  53.5   75.23
	.word	  3786,  15941		#  54.5   76.64
	.word	  3393,  16029		#  55.5   78.05
	.word	  2999,  16107		#  56.5   79.45
	.word	  2603,  16176		#  57.5   80.86
	.word	  2205,  16235		#  58.5   82.27
	.word	  1806,  16284		#  59.5   83.67
	.word	  1406,  16324		#  60.5   85.08
	.word	  1005,  16353		#  61.5   86.48
	.word	   603,  16373		#  62.5   87.89
	.word	   201,  16383		#  63.5   89.30
	.word	  -201,  16383		#  64.5   90.70
	.word	  -603,  16373		#  65.5   92.11
	.word	 -1005,  16353		#  66.5   93.52
	.word	 -1406,  16324		#  67.5   94.92
	.word	 -1806,  16284		#  68.5   96.33
	.word	 -2205,  16235		#  69.5   97.73
	.word	 -2603,  16176		#  70.5   99.14
	.word	 -2999,  16107		#  71.5  100.55
	.word	 -3393,  16029		#  72.5  101.95
	.word	 -3786,  15941		#  73.5  103.36
	.word	 -4176,  15843		#  74.5  104.77
	.word	 -4563,  15736		#  75.5  106.17
	.word	 -4948,  15619		#  76.5  107.58
	.word	 -5330,  15493		#  77.5  108.98
	.word	 -5708,  15357		#  78.5  110.39
	.word	 -6084,  15213		#  79.5  111.80
	.word	 -6455,  15059		#  80.5  113.20
	.word	 -6823,  14896		#  81.5  114.61
	.word	 -7186,  14724		#  82.5  116.02
	.word	 -7545,  14543		#  83.5  117.42
	.word	 -7900,  14354		#  84.5  118.83
	.word	 -8250,  14155		#  85.5  120.23
	.word	 -8595,  13949		#  86.5  121.64
	.word	 -8935,  13733		#  87.5  123.05
	.word	 -9269,  13510		#  88.5  124.45
	.word	 -9598,  13279		#  89.5  125.86
	.word	 -9921,  13039		#  90.5  127.27
	.word	-10238,  12792		#  91.5  128.67
	.word	-10549,  12537		#  92.5  130.08
	.word	-10853,  12274		#  93.5  131.48
	.word	-11151,  12004		#  94.5  132.89
	.word	-11442,  11727		#  95.5  134.30
	.word	-11727,  11442		#  96.5  135.70
	.word	-12004,  11151		#  97.5  137.11
	.word	-12274,  10853		#  98.5  138.52
	.word	-12537,  10549		#  99.5  139.92
	.word	-12792,  10238		# 100.5  141.33
	.word	-13039,   9921		# 101.5  142.73
	.word	-13279,   9598		# 102.5  144.14
	.word	-13510,   9269		# 103.5  145.55
	.word	-13733,   8935		# 104.5  146.95
	.word	-13949,   8595		# 105.5  148.36
	.word	-14155,   8250		# 106.5  149.77
	.word	-14354,   7900		# 107.5  151.17
	.word	-14543,   7545		# 108.5  152.58
	.word	-14724,   7186		# 109.5  153.98
	.word	-14896,   6823		# 110.5  155.39
	.word	-15059,   6455		# 111.5  156.80
	.word	-15213,   6084		# 112.5  158.20
	.word	-15357,   5708		# 113.5  159.61
	.word	-15493,   5330		# 114.5  161.02
	.word	-15619,   4948		# 115.5  162.42
	.word	-15736,   4563		# 116.5  163.83
	.word	-15843,   4176		# 117.5  165.23
	.word	-15941,   3786		# 118.5  166.64
	.word	-16029,   3393		# 119.5  168.05
	.word	-16107,   2999		# 120.5  169.45
	.word	-16176,   2603		# 121.5  170.86
	.word	-16235,   2205		# 122.5  172.27
	.word	-16284,   1806		# 123.5  173.67
	.word	-16324,   1406		# 124.5  175.08
	.word	-16353,   1005		# 125.5  176.48
	.word	-16373,    603		# 126.5  177.89
	.word	-16383,    201		# 127.5  179.30
	.word	-16383,   -201		# 128.5  180.70
	.word	-16373,   -603		# 129.5  182.11
	.word	-16353,  -1005		# 130.5  183.52
	.word	-16324,  -1406		# 131.5  184.92
	.word	-16284,  -1806		# 132.5  186.33
	.word	-16235,  -2205		# 133.5  187.73
	.word	-16176,  -2603		# 134.5  189.14
	.word	-16107,  -2999		# 135.5  190.55
	.word	-16029,  -3393		# 136.5  191.95
	.word	-15941,  -3786		# 137.5  193.36
	.word	-15843,  -4176		# 138.5  194.77
	.word	-15736,  -4563		# 139.5  196.17
	.word	-15619,  -4948		# 140.5  197.58
	.word	-15493,  -5330		# 141.5  198.98
	.word	-15357,  -5708		# 142.5  200.39
	.word	-15213,  -6084		# 143.5  201.80
	.word	-15059,  -6455		# 144.5  203.20
	.word	-14896,  -6823		# 145.5  204.61
	.word	-14724,  -7186		# 146.5  206.02
	.word	-14543,  -7545		# 147.5  207.42
	.word	-14354,  -7900		# 148.5  208.83
	.word	-14155,  -8250		# 149.5  210.23
	.word	-13949,  -8595		# 150.5  211.64
	.word	-13733,  -8935		# 151.5  213.05
	.word	-13510,  -9269		# 152.5  214.45
	.word	-13279,  -9598		# 153.5  215.86
	.word	-13039,  -9921		# 154.5  217.27
	.word	-12792, -10238		# 155.5  218.67
	.word	-12537, -10549		# 156.5  220.08
	.word	-12274, -10853		# 157.5  221.48
	.word	-12004, -11151		# 158.5  222.89
	.word	-11727, -11442		# 159.5  224.30
	.word	-11442, -11727		# 160.5  225.70
	.word	-11151, -12004		# 161.5  227.11
	.word	-10853, -12274		# 162.5  228.52
	.word	-10549, -12537		# 163.5  229.92
	.word	-10238, -12792		# 164.5  231.33
	.word	 -9921, -13039		# 165.5  232.73
	.word	 -9598, -13279		# 166.5  234.14
	.word	 -9269, -13510		# 167.5  235.55
	.word	 -8935, -13733		# 168.5  236.95
	.word	 -8595, -13949		# 169.5  238.36
	.word	 -8250, -14155		# 170.5  239.77
	.word	 -7900, -14354		# 171.5  241.17
	.word	 -7545, -14543		# 172.5  242.58
	.word	 -7186, -14724		# 173.5  243.98
	.word	 -6823, -14896		# 174.5  245.39
	.word	 -6455, -15059		# 175.5  246.80
	.word	 -6084, -15213		# 176.5  248.20
	.word	 -5708, -15357		# 177.5  249.61
	.word	 -5330, -15493		# 178.5  251.02
	.word	 -4948, -15619		# 179.5  252.42
	.word	 -4563, -15736		# 180.5  253.83
	.word	 -4176, -15843		# 181.5  255.23
	.word	 -3786, -15941		# 182.5  256.64
	.word	 -3393, -16029		# 183.5  258.05
	.word	 -2999, -16107		# 184.5  259.45
	.word	 -2603, -16176		# 185.5  260.86
	.word	 -2205, -16235		# 186.5  262.27
	.word	 -1806, -16284		# 187.5  263.67
	.word	 -1406, -16324		# 188.5  265.08
	.word	 -1005, -16353		# 189.5  266.48
	.word	  -603, -16373		# 190.5  267.89
	.word	  -201, -16383		# 191.5  269.30
	.word	   201, -16383		# 192.5  270.70
	.word	   603, -16373		# 193.5  272.11
	.word	  1005, -16353		# 194.5  273.52
	.word	  1406, -16324		# 195.5  274.92
	.word	  1806, -16284		# 196.5  276.33
	.word	  2205, -16235		# 197.5  277.73
	.word	  2603, -16176		# 198.5  279.14
	.word	  2999, -16107		# 199.5  280.55
	.word	  3393, -16029		# 200.5  281.95
	.word	  3786, -15941		# 201.5  283.36
	.word	  4176, -15843		# 202.5  284.77
	.word	  4563, -15736		# 203.5  286.17
	.word	  4948, -15619		# 204.5  287.58
	.word	  5330, -15493		# 205.5  288.98
	.word	  5708, -15357		# 206.5  290.39
	.word	  6084, -15213		# 207.5  291.80
	.word	  6455, -15059		# 208.5  293.20
	.word	  6823, -14896		# 209.5  294.61
	.word	  7186, -14724		# 210.5  296.02
	.word	  7545, -14543		# 211.5  297.42
	.word	  7900, -14354		# 212.5  298.83
	.word	  8250, -14155		# 213.5  300.23
	.word	  8595, -13949		# 214.5  301.64
	.word	  8935, -13733		# 215.5  303.05
	.word	  9269, -13510		# 216.5  304.45
	.word	  9598, -13279		# 217.5  305.86
	.word	  9921, -13039		# 218.5  307.27
	.word	 10238, -12792		# 219.5  308.67
	.word	 10549, -12537		# 220.5  310.08
	.word	 10853, -12274		# 221.5  311.48
	.word	 11151, -12004		# 222.5  312.89
	.word	 11442, -11727		# 223.5  314.30
	.word	 11727, -11442		# 224.5  315.70
	.word	 12004, -11151		# 225.5  317.11
	.word	 12274, -10853		# 226.5  318.52
	.word	 12537, -10549		# 227.5  319.92
	.word	 12792, -10238		# 228.5  321.33
	.word	 13039,  -9921		# 229.5  322.73
	.word	 13279,  -9598		# 230.5  324.14
	.word	 13510,  -9269		# 231.5  325.55
	.word	 13733,  -8935		# 232.5  326.95
	.word	 13949,  -8595		# 233.5  328.36
	.word	 14155,  -8250		# 234.5  329.77
	.word	 14354,  -7900		# 235.5  331.17
	.word	 14543,  -7545		# 236.5  332.58
	.word	 14724,  -7186		# 237.5  333.98
	.word	 14896,  -6823		# 238.5  335.39
	.word	 15059,  -6455		# 239.5  336.80
	.word	 15213,  -6084		# 240.5  338.20
	.word	 15357,  -5708		# 241.5  339.61
	.word	 15493,  -5330		# 242.5  341.02
	.word	 15619,  -4948		# 243.5  342.42
	.word	 15736,  -4563		# 244.5  343.83
	.word	 15843,  -4176		# 245.5  345.23
	.word	 15941,  -3786		# 246.5  346.64
	.word	 16029,  -3393		# 247.5  348.05
	.word	 16107,  -2999		# 248.5  349.45
	.word	 16176,  -2603		# 249.5  350.86
	.word	 16235,  -2205		# 250.5  352.27
	.word	 16284,  -1806		# 251.5  353.67
	.word	 16324,  -1406		# 252.5  355.08
	.word	 16353,  -1005		# 253.5  356.48
	.word	 16373,   -603		# 254.5  357.89
	.word	 16383,   -201		# 255.5  359.30
	.word	 16383,    201		#   0.5  360.70
	.word	 16373,    603		#   1.5  362.11
	.word	 16353,   1005		#   2.5  363.52
	.word	 16324,   1406		#   3.5  364.92
	.word	 16284,   1806		#   4.5  366.33

#	************
#	*	   *
#	*  Macros  *
#	*	   *
#	************

# Inner loop register use
#	eax	Hough space address and count increment (+-houghSize); also temporary
#	ebx	Hough space address
#	ecx	R count, + or -, counts towards 0
#	edx	-> sin/cos table
#	esi	-> ping/pong buffer used by integer unit
#	edi	-> ping/pong buffer used by MMX unit
#	ebp	-> edge list
#	mm0	edge location  | y | x | y | x |
#	mm1	working register
#	mm2	previous R value for computing spans in low 32 bits
#	mm3	| 0x0001 | 0x0001 | 0x0001 | 0x0001 |
#	mm4	| min R | min R |
#	mm5	| max R | max R |

####
# Setup for processing next edge:
#    - Put sin/cos table address for T-angleSpread in edx
#    - Put (x, y) in mm0
#    - Update edge pointer
#    - Initialize "previous R" value in mm2
#    - Put address of start of first R-run in ppAddr
#    - If "live" (not pipe fill for first edge), get address for first R-run of current edge
#      and prefetch first R-run

	.macro	NextEdge live
	movd	mm0, [ebp + edgeX]			# mm0 = edge location  | y | x | y | x |
	punpckldq mm0, mm0

	movzx	eax, byte ptr[ebp + edgeT]		# fetch angle (T)
	add	ebp, edgeSize				# update edge pointer
	.if	\live					# if live
	mov	ebx, [esi + ppAddr]			#   get address for first R-run for current edge
	mov	ecx, [esi + ppSpan]			#   get first span for subsequent prefetch
	.endif
	lea	edx, [sincosTable + eax*4]		# get sin/cos table address for starting T

	.if	\live && pfEnable			# if live, prefetch first and last word
	prefetch [ebx]
	prefetch [ebx + ecx]
	.endif

	movd	mm2, [edx]				# fetch sin/cos for first T
	pmaddwd	mm2, mm0				# dot product to get R, S32.14
	psrad	mm2, 14					# | R[p+1] |  R[p]  |  S32.0
	pmaxsw	mm2, mm4				# clamp R at lower edge of Hough space
	pminsw	mm2, mm5				# clamp R at upper edge of Hough space
	movd	ecx, mm2				# mm2 is initialized, now get R to calculate address

	imul	eax, houghPitch				# calculate address for first R-run of next edge
	add	eax, [esp + houghSpace]
	lea	ecx, [eax + ecx * houghSize + houghWd / 2 * houghSize]
	.if	\live					# store address for next stage
	mov	[edi + ppAddr], ecx
	.else
	mov	[esi + ppAddr], ecx
	.endif
	.endm
####
# R loop. Increment 1 or more consecutive Hough bins at fixed T
#
	.macro	RLoop
rl\@:	inc	word ptr [ebx]
	add	ebx, eax
	sub	ecx, eax
	jne	rl\@
	.endm

####
# T loop, pipe fill for first edge.
#
# These are the MMX instructions used to calculate the span and increment for a pair of
# R-runs for a given T. Two are done in parallel. The values are stored in one of the
# ping-pong buffers, for later use by the second stage of the pipeline. The version of
# these instructions in this macro is used only during initialization to fill the pipeline.
# These instructions are duplicated, with slight modification for address generation,
# in the TLoop0 and TLoop1 macros below. In those macros, these instructions are
# mingled with the integer unit instructions of the second pipeline stage to take
# advantage of the separate integer and MMX pipelines on the Geode. The integer unit
# instructions are duplicated in TLoop0 and TLoop1, since each set does one R-run. The
# mmx instructions are divided into two parts, one for each macro, since the complete
# set does two R-runs in parallel.
#
	.macro	TLoopFill
	movq	mm1, [edx + ecx*8 + 4]			# | sin[p+1] | cos[p+1] | sin[p] | cos[p] | S16.14

	pmaddwd	mm1, mm0				# | R[p+1] |  R[p]  |  S32.14
	psrad	mm1, 14					# | R[p+1] |  R[p]  |  S32.0
	pmaxsw	mm1, mm4				# clamp R at lower edge of Hough space
	pminsw	mm1, mm5				# clamp R at upper edge of Hough space

	punpckldq mm2, mm1				# |  R[p]  | R[p-1] |
	psubd	mm1, mm2				# | span[p+1] |  span[p]  |
	paddd	mm2, mm1				# | R[p+1] |  R[p]  |

	psrlq	mm2, 32					# |  ---   | R[p+1] | for next time
	paddd	mm1, mm1				# span *= 2 since houghSize = 2

	movq	[esi + ppSpan + ecx*8], mm1		# store span pair

	psrad	mm1, 31					# make positive spans 0, negative spans -1
	por	mm1, mm3				# make positive spans 1, negative spans -1
	paddd	mm1, mm1				# make positive spans 2, negative spans -2
	movq	[esi + ppInc + ecx*8], mm1		# store increment pair
	.endm

####
# T loop, even phases
#
	.macro	TLoop0	phase
	# integer unit					# integer unit comments
				# mmx unit					# mmx unit comments
	.if	\phase
	sub	ebx, eax				# back up Hough address to point to last bin accessed
	add	ebx, houghPitch				# update Hough pointer to current T
	.endif

	mov	ecx, [esi + ppSpan + 4 * \phase]	# fetch span for current set of R values

				movq	mm1, [edx + 4 * \phase + 4]		# | sin[p+1] | cos[p+1] | sin[p] | cos[p] | S16.14
				pmaddwd	mm1, mm0				# | R[p+1] |  R[p]  |  S32.14

        .if     pfEnable
	prefetch [ebx + ecx + houghPitch]		# prefetch byte at start of next set of R
	mov	eax, [esi + ppSpan + 4 * (\phase + 1)]	# fetch next span
	add	eax, ecx				# current + next span
        .endif

				psrad	mm1, 14					# | R[p+1] |  R[p]  |  S32.0
				pmaxsw	mm1, mm4				# clamp R at lower edge of Hough space
				pminsw	mm1, mm5				# clamp R at upper edge of Hough space
        .if     pfEnable
	prefetch [ebx + eax + houghPitch]		# prefetch byte at end of next set of R
        .endif
				punpckldq mm2, mm1				# |  R[p]  | R[p-1] |
	mov	eax, [esi + ppInc + 4 * \phase]		# fetch increment for current set of R

	                        psubd	mm1, mm2				# | span[p+1] |  span[p]  |
        add	ecx, eax				# span + increment is the starting count
				paddd	mm2, mm1				# | R[p+1] |  R[p]  |
 	.endm

#
# T loop, odd phases
#
	.macro	TLoop1	phase
	# integer unit					# integer unit comments
				# mmx unit					# mmx unit comments
	sub	ebx, eax				# back up Hough address to point to last bin acessed
	add	ebx, houghPitch				# update Hough poinnter to current T
	mov	ecx, [esi + ppSpan + 4 * \phase]	# fetch span for current set of R values

	                        psrlq	mm2, 32					# |  ---   | R[p+1] | for next time
				paddd	mm1, mm1				# span *= 2 since houghSize = 2

	.if	\phase - angleCount + 1 && pfEnable     # If not the last phase (phase = angleCount-1),
	prefetch [ebx + ecx + houghPitch]		# prefetch byte at start of next set of R
	mov	eax, [esi + ppSpan + 4 * (\phase + 1)]	# fetch next span
	add	eax, ecx				# current + next span
	.endif
				movq	[edi + ppSpan + 4 * (\phase - 1)], mm1	# store span pair
				psrad	mm1, 31					# make positive spans 0, negative spans -1

	.if	\phase - angleCount + 1 && pfEnable
	prefetch [ebx + eax + houghPitch]		# prefetch byte at end of next set of R
	.endif

				por	mm1, mm3				# make positive spans 1, negative spans -1
	mov	eax, [esi + ppInc + 4 * \phase]		# fetch increment for current set of R

				paddd	mm1, mm1				# make positive spans 2, negative spans -2

	add	ecx, eax				# span + increment is the starting count
				movq	[edi + ppInc + 4 * (\phase - 1)], mm1	# store increment pair
	.endm

#	***************************
#	*			  *
#	*  houghMain Entry Point  *
#	*			  *
#	***************************
#
# typedef struct
# {
#   uint16_t	angle;		// 8-bit binary angle (T)
#   int16_t     x, y;		// edge position relative to center of image
# }
# Edge;
#
# void houghMain(uint16_t* houghSpace, Edge* edgeList, int edgeCount)
#
# houghSpace should be quadword aligned. It must be
#	(houghPitch * (256 + angleCount - 1) + 7) & ~7
# bytes, which is 169,600 bytes with the constants defined above. The rows in houghSpace
# correspond to the angles listed in sincosTable above. As can be seen, there are
# angleCount duplicated rows, which is needed for the calculations here and is useful for
# any following neighborhood processing, since houghSpace wraps around in angle.

	.section .text
break:
        nop
        ret

	.globl	_houghMain
_houghMain:
	push	ebx
	push	esi
	push	edi
	push	ebp
	sub	esp, localVarSize

# Initialize registers
	lea	esi, [esp + ppBuf0]
	lea	edi, [esp + ppBuf1]
	mov	ebp, [esp + edgeList]
	pcmpeqd	mm3, mm3
	psrld	mm3, 31
	mov	eax, -(houghWd / 2)
	movd	mm4, eax
	punpckldq mm4, mm4
	mov	eax, houghWd / 2
	movd	mm5, eax
	punpckldq mm5, mm5

# Zero Hough space
	mov	eax, [esp + houghSpace]
	mov	ecx, houghPitch * (256 + angleCount - 1)
	pxor	mm0, mm0
zero:   movntq	[eax], mm0
	add	eax, 8
	sub	ecx, 8
	jg	zero

# Fill first stage of pipeline
	NextEdge 0
	xor	ecx, ecx
fill:	TLoopFill
	inc	ecx
	cmp	ecx, angleSpread
	jne	fill

#
# Edge loop
#
edgeLoop:
	NextEdge 1

	phase = 0
        nop
	.rept	angleSpread
	TLoop0	phase
        RLoop
	TLoop1	(phase + 1)
	RLoop
        phase = phase + 2
	.endr

	# exchange ping/pong buffers
	xor	edi, esi
	xor	esi, edi
	xor	edi, esi

	# loop control
	dec	dword ptr[esp + edgeCount]
	jne	edgeLoop

#
# Done processing edges, handle Hough space angle wrap-around
#
	mov	eax, [esp + houghSpace]
	mov	ecx, houghWd * (angleCount - 1)
wrap:	movq	mm0, [eax]
	paddw	mm0, [eax + 256 * houghPitch]
	movq	[eax], mm0
	movq	[eax + 256 * houghPitch], mm0
	add	eax, 8
	sub	ecx, 8/houghSize
	jg	wrap

#
# return
#
	add	esp, localVarSize
	pop	ebp
	pop	edi
	pop	esi
	pop	ebx
	emms
	ret
