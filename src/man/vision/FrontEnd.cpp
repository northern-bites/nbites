// *****************************************
// *                                       *
// *  RoboCup Image Acquisition Front End  *
// *                                       *
// *****************************************

#include "stdafx.h"
#include <stdlib.h>
#include "FrontEnd.h"

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
      *po++ = colors->orange.scoreMax(y, u, v ^ UVMask);

      if (colorTable)
        *pc++ = colorTable[(u >> (UVBits - 7) << 14) + (v >> (UVBits - 7) << 7) + (y >> (YBits - 7))];
    }
  }

  return (uint32_t)timer.time();;
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
