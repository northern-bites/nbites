#include "ImageAcquisition.h"
#include "VisionDef.h"
#include "ColorParams.h"
#include "RatingParams.h"

using namespace std;  // delete later

#define CPP_ACQUIRE 0

// The ASM and C++ have identical function signatures.
// The source is a YUYV image. There are no pixel alignment requirements, although
// operation may be faster if source is QWORD or DQWORD aligned.
// width and height refer to the output images. The low three bits of width are ignored and
// assumed to be zero.
// pitch is source image row pitch in bytes, and can be >= width
// The destination is four or five images, concatenated with no padding:
//    Y image, 16-bit pixels
//    white image, 8-bit pixels
//    green image, 8-bit pixels
//    orange image, 8-bit pixels
//    optional image reulting from color table lookup
int ImageAcquisition::acquire_image(const unsigned char* source,
                                    int width, int height, int pitch,
                                    const Colors* colors,
                                    unsigned char* dest,
                                    unsigned char* colorTable)
{

#if 0
    _acquire_image(source, width, height, pitch, colors, dest, colorTable);
#else
 // Ignore low three bits of width
    width &= ~7;

    // destination pointers
    short* py = (short*)dest;
    unsigned char* pw = dest + 2 * width * height;
    unsigned char* pg = pw + width * height;
    unsigned char* po = pg + width * height;
    unsigned char* pc = po + width * height;

    for (int j = 0; j < height; ++j) {
        const unsigned char* ps = source + 2 * pitch * j;
        for (int i = 0; i < width; ++i, ps += 4) {
            int y = ps[0] + ps[2] + ps[pitch] + ps[pitch + 2];
            int u = ps[1] + ps[pitch + 1];
            int v = ps[3] + ps[pitch + 3];

            *py++ = (short)y;
            *pw++ = colors->white .scoreMax(y, abs(u - UVZero) + UVZero, abs(v - UVZero) + UVZero);
            *pg++ = colors->green .scoreMax(y, u, v);
            *po++ = colors->orange.scoreMax(y, u, v ^ UVMask);

            if (colorTable)
                *pc++ = colorTable[(u >> (UVBits - 7) << 14) + (v >> (UVBits - 7) << 7) + (y >> (YBits - 7))];
        }
    }

#endif
    return 0;
}