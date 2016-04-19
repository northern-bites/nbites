// *****************************************
// *                                       *
// *  RoboCup Image Acquisition Front End  *
// *                                       *
// *****************************************

#include "FrontEnd.h"
#include "Stdafx.h"

#include <stdlib.h>
#include <iostream>

namespace man {
namespace vision {

// *********************
// *                   *
// *  Computed Colors  *
// *                   *
// *********************

inline int limit(int x, int bits)
{
  return min(max(x, 0), (1 << bits) - 1);
}

// Convert a floating point color parameter to an integer
int fix(float uv, int zero, bool invert)
{
  int w = limit((int)(UVRange * uv + zero + 0.5), UVBits);
  if (invert)
    w ^= UVMask;
  return w;
}

void ColorParams::load(float darkU0, float darkV0, float lightU0, float lightV0, float fuzzyU, float fuzzyV)
{
  bool invertV = fuzzyU * fuzzyV < 0;
  darkU0  -= 0.5f * fuzzyU;
  darkV0  -= 0.5f * fuzzyV;
  lightU0 -= 0.5f * fuzzyU;
  lightV0 -= 0.5f * fuzzyV;
  int du0 = fix(darkU0       , UVZero, false  );
  int dv0 = fix(darkV0       , UVZero, invertV);
  int lu0 = fix(lightU0      , UVZero, false  );
  int lv0 = fix(lightV0      , UVZero, invertV);
  int fu  = fix(fabsf(fuzzyU), 0     , false  );
  int fv  = fix(fabsf(fuzzyV), 0     , false  );

  darkZero.load(du0, dv0);
  yCoeff.load(((lu0 - du0) << 16) / YMax, ((lv0 - dv0) << 16) / YMax);
  fuzzySpread.load(fu, fv);
  inverseFuzzy.load((255 << 8) / fu, (255 << 8) / fv);
}

// *******************************
// *                             *
// *  Aligned Memory Allocation  *
// *                             *
// *******************************
//
// Allocate bytes using operator new. Place a pointer to the allocated buffer in block
// for operator delete. Return a pointer to at least the requested size and aligned as
// specified.
void* alignedAlloc(size_t size, int alignBits, void*& block)
{
  size_t extra = (1 << alignBits) - 1;
  block = new char[size + extra];
  return (void*)(((size_t)block + extra) & ~extra);
}

// ********************************
// *                              *
// *  Front End Image Processing  *
// *                              *
// ********************************
//
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
extern "C" uint32_t
  _acquire_image(const uint8_t* source, int width, int height, int pitch, const Colors* colors,
                 uint8_t* dest, uint8_t* colorTable = 0);

uint32_t
  testAcquire(const uint8_t* source, int width, int height, int pitch, const Colors* colors,
              uint8_t* dest, uint8_t* colorTable)
{
  TickTimer timer;

  // Ignore low three bits of width
  width &= ~7;

  // destination pointers
  short* py = (short*)dest;
  uint8_t* pw = dest + 2 * width * height;
  uint8_t* pg = pw + width * height;
  uint8_t* po = pg + width * height;
  uint8_t* pc = po + width * height;

  for (int j = 0; j < height; ++j)
  {
    const uint8_t* ps = source + 2 * pitch * j;
    for (int i = 0; i < width; ++i, ps += 4)
    {
      int y = ps[0] + ps[2] + ps[pitch] + ps[pitch + 2];
      int u = ps[1] + ps[pitch + 1];
      int v = ps[3] + ps[pitch + 3];

      *py++ = (short)y;
      *pw++ = colors->white .scoreMax(y, abs(u - UVZero) + UVZero, abs(v - UVZero) + UVZero);
      *pg++ = colors->green .scoreMax(y, u, v);
      //*po++ = colors->orange.scoreMax(y, u, v ^ UVMask); //making orange into black for experimentation
      *po++ = colors->orange.scoreMax(y, abs(u - UVZero) + UVZero, abs(v - UVZero) + UVZero);
      if (colorTable)
        *pc++ = colorTable[(u >> (UVBits - 7) << 14) + (v >> (UVBits - 7) << 7) + (y >> (YBits - 7))];
    }
  }

  return (uint32_t)timer.time();;
}

ImageFrontEnd::ImageFrontEnd()
{
  memBlock = 0;
  dstAllocated = 0;
  dstImages = 0;
  dstOffset = 0;

  fast(true);
}

ImageFrontEnd::~ImageFrontEnd()
{
  delete [] memBlock;
}

void ImageFrontEnd::run(const YuvLite& src, const Colors* colors, uint8_t* colorTable)
{
  dstBase = ImageLiteBase(src.x0(), src.y0(), src.width(), src.height(),
                          (src.width() + 15) & ~15);

  int size = 5 * imagePitch();
  if (colorTable)
    size += imagePitch();

  if (size > dstAllocated)
  {
    delete[] memBlock;
    dstImages = (uint8_t*)alignedAlloc(size, 4, memBlock);
    dstAllocated = size;
  }

  if (fast()) {
    _time = man::vision::_acquire_image(src.pixelAddr(), dstBase.pitch(),
                                        dstBase.height(), src.pitch(),
                                        colors, dstImages, colorTable);
  } else {
    _time = testAcquire(src.pixelAddr(), dstBase.pitch(), dstBase.height(), 
                        src.pitch(), colors, dstImages, colorTable);
  }
}

}
}
