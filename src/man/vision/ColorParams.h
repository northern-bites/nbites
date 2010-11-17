#ifndef _ColorParams_h_DEFINED
#define _ColorParams_h_DEFINED
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

struct ColorParams
{                           // words:      3         2         1         0
unsigned long long   yZero;              // | y0 << 2 | y0 << 2 | y0 << 2 | y0 << 2 |
unsigned long long   ySlope;             // |    0    |   yk    |    0    |   yk    |
unsigned long long   yLimit;             // |  yn-1   |  yn-1   |  yn-1   |  yn-1   |

unsigned long long   uvZero;             // | v0 << 1 | u0 << 1 | v0 << 1 | u0 << 1 |
unsigned long long   uvSlope;            // |   vk    |   uk    |   vk    |   uk    |
unsigned long long   uvLimit;            // |  vn-1   |  un-1   |  vn-1   |  un-1   |
unsigned long long   uvDim;              // | un * yn |   yn    | un * yn |   yn    |
};

#endif /* _ColorParams_h_DEFINED */
