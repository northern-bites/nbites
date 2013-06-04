#include "ColorParams.h"

using namespace std;

// See ColorParams.h for the layout of the struct variables
ColorParams::ColorParams(int y0, int u0, int v0,
                         int y1, int u1, int v1,
                         int yn, int un, int vn)
{
    // Must be using more than 0 values of each color range
    assert(y0 != y1 &&
           u0 != u1 &&
           v0 != v1);

    // Color table size must be equal to or smaller than actual spread
    // of used color values
    assert(yn <= y1 - y0 &&
           un <= u1 - u0 &&
           vn <= v1 - v0);

    // See ColorParams.h for details on values and their layouts.
    // For use with full resolution _acquire_image
    // yZero = MMX4(y0 << 2);
    // ySlope = MMX0101( (yn << 14) / (y1 - y0));
    // yLimit = MMX4(yn-1);

    // uvZero = MMX22(u0 << 1, v0 << 1);
    // uvSlope = MMX22( (un << 15) / (u1 - u0), (vn << 15) / (v1 - v0));
    // uvLimit = MMX22(un-1, vn-1);
    // uvDim = MMX22(yn, un * yn);

    // For use with _acquire_image_fast (does half the summing):
    yZero = MMX4(y0 << 1);
    ySlope = MMX0101( (yn << 15) / (y1 - y0));
    yLimit = MMX4(yn-1);

    uvZero = MMX22(u0, v0);
    uvSlope = MMX22( (un << 15) / (u1 - u0), (vn << 15) / (v1 - v0));
    uvLimit = MMX22(un-1, vn-1);
    uvDim = MMX22(yn, un * yn);
}

void ColorParams::printParams()
{
    printf("yZero:\n\t%016llx\n", yZero);
    printf("ySlope:\n\t%016llx\n", ySlope);
    printf("yLimit:\n\t%016llx\n", yLimit);

    printf("uvZero:\n\t%016llx\n", uvZero);
    printf("uvSlope:\n\t%016llx\n", uvSlope);
    printf("uvLimit:\n\t%016llx\n", uvLimit);
    printf("uvDim:\n\t%016llx\n", uvDim);
}
