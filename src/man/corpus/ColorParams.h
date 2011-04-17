#ifndef _ColorParams_h_DEFINED
#define _ColorParams_h_DEFINED

#include <stdio.h>
#include <assert.h>
#include <stdint.h>

/*
  The mapping from y pixel values to y table indicies is:

           y index
              |             _____________
      (yn-1) -|            /
              |           /
              |          /
              |         /
           0 -| _______/
               -----------------------------  y
                |      |    |            |
                0     y0   y1           255

    The mapping for u and v is similar. The values yn, un, and vn give the
    table size in each of the three dimensions. The total table size is yn
    * un * vn. These values need not be powers of 2. The slopes of the
    active segment of the maps are given by

      yk = (yn << 14) / (y1 - y0)
      uk = (un << 15) / (u1 - u0)
      vk = (vn << 15) / (v1 - v0)
*/

#define MMX4(n) ((n) * 0x0001000100010001ULL)
#define MMX0101(n) ( (n) * 0x0000000100000001ULL)

#define MMX22(u,v) ( ((v) * 0x0001000000010000ULL) | \
                     ( (u) * 0x0000000100000001ULL))

typedef uint64_t MMXWord;

struct ColorParams
{
    ColorParams(int y0, int u0, int v0,
                int y1, int u1, int v1,
                int yn, int un, int vn);
    void printParams();

                       // words:      3         2         1         0
    MMXWord yZero;              // | y0 << 2 | y0 << 2 | y0 << 2 | y0 << 2 |
    MMXWord ySlope;             // |    0    |   yk    |    0    |   yk    |
    MMXWord yLimit;             // |  yn-1   |  yn-1   |  yn-1   |  yn-1   |

    MMXWord uvZero;             // | v0 << 1 | u0 << 1 | v0 << 1 | u0 << 1 |
    MMXWord uvSlope;            // |   vk    |   uk    |   vk    |   uk    |
    MMXWord uvLimit;            // |  vn-1   |  un-1   |  vn-1   |  un-1   |
    MMXWord uvDim;              // | un * yn |   yn    | un * yn |   yn    |

};

#endif /* _ColorParams_h_DEFINED */
