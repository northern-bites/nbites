// ********************************************
// *                                          *
// *  Vision 2105 General Utilities and Math  *
// *                                          *
// ********************************************

#ifndef _vision_
#define _vision_

#include "Stdafx.h"

#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>

namespace man {
namespace vision {

// Scan array of unsigned bytes or words for values above the specified
// threshold. Place in the runs array the offset of each such byte/word found,
// terminated by -1.
//
// Source values are processed 16 at a time in parallel. runLengthU8/16 processes
// a multiple of 16 bytes/words, and so will read beyond the end of source if
// count is not a multiple of 16. If the memory does not exist, a fault will
// occur.
//
// The maximum size needed for runs is count, rounded up to a multiple of 16,
// plus 1 for the terminator.
//
// runs will not have any garbage due to the overreading, but some data in runs
// beyond the terminator may be overwritten if count is not a multiple of 16.
//
// Signed versions can be produced with trivial effort using the macros in
// runLength.s.
extern "C" void _runLengthU8 (uint8_t * source, int count, int thresh, int32_t* runs);
extern "C" void _runLengthU16(uint16_t* source, int count, int thresh, int32_t* runs);

// *************
// *           *
// *  Utility  *
// *           *
// *************

void* alignedAlloc(size_t size, int alignBits, void*& block);

inline int min(int x, int y)
{
  return x <= y ? x : y;
}

inline int max(int x, int y)
{
  return x >= y ? x : y;
}

inline float min(float x, float y)
{
  return x <= y ? x : y;
}

inline float max(float x, float y)
{
  return x >= y ? x : y;
}

inline double min(double x, double y)
{
  return x <= y ? x : y;
}

inline double max(double x, double y)
{
  return x >= y ? x : y;
}

inline double dist(double x1, double y1, double x2, double y2)
{
    double x = x1 - x2;
    double y = y1 - y2;

    return sqrt(pow(x, 2), + pow(y, 2));
}

inline double uMod(double x, double mod)
{
  return x - floor(x / mod) * mod;
}

inline double sMod(double x, double mod)
{
  return x - floor(x / mod + 0.5) * mod;
}

inline double diffRadians(double a, double b)
{
  return fabs(sMod(a - b, 2 * M_PI));
}


inline void unitVec(double x, double y, double& u, double& v)
{
  double g = sqrt(x * x + y * y);
  u = x / g;
  v = y / g;
}

std::string strPrintf(const char* format, ...);


// ************
// *          *
// *  Timing  *
// *          *
// ************
//
// High-resolution clock-tick timer
class TickTimer
{
  uint64_t startTime;

  uint64_t read()
  {
    uint32_t lo, hi;
#ifdef WIN32
    __asm
    {
      rdtsc;
      mov lo, eax
      mov hi, edx
    }
#else
    // TODO fix
    asm("rdtsc;\n"
        "mov %%eax, %0;\n"
        "mov %%edx, %1;\n"
        :"=r" (lo), "=r" (hi)
        :
        :"%eax", "%edx"
        );
#endif
    return ((uint64_t)hi << 32) | lo;
  }

public:
  // Copy/assign OK

  TickTimer() { start(); }

  void start()
  {
    startTime = read();
  }

  uint64_t time()
  {
    return read() - startTime;
  }

  uint32_t time32()
  {
    return (uint32_t)time();
  }
};

// *****************
// *               *
// *  Fuzzy Logic  *
// *               *
// *****************

// A fuzzy logic value is a number [0 .. 1] that can be produced by a fuzzy threshold and
// operated on by and, or, and not operations.

class FuzzyThr
{
  float _t0, _range;

public:
  // Copy/assign OK

  float t0() const { return _t0; }
  float t1() const { return _t0 + _range; }
  float range() const { return _range; }

  FuzzyThr(float t0, float t1)
  {
    _t0 = t0;
    _range = t1 - t0;
  }
};

class Fool
{
  float _f;

  Fool(float f)
    : _f(f)
  {}

public:
  // Copy/assign OK

  float f() const { return _f; }

  Fool(const FuzzyThr& t, float x)
  {
    if (t.range() == 0)
      _f = x >= t.t0() ? 1.0f : 0.0f;
    else
      _f = min(max((x - t.t0()) / t.range(), 0.0f), 1.0f);
  }

  Fool operator&(Fool a)
  {
    return Fool(min(f(), a.f()));
  }

  Fool operator|(Fool a)
  {
    return Fool(max(f(), a.f()));
  }
    
  Fool operator!()
  {
    return Fool(1 - f());
  }
};

inline Fool operator>(float x, const FuzzyThr& t)
{
  return Fool(t, x);
}

inline Fool operator<(float x, const FuzzyThr& t)
{
  return !(x > t);
}

// ***********************
// *                     *
// *  Linear Regression  *
// *                     *
// ***********************

class LineFit
{
  double sumW;
  double sumX, sumY, sumXY, sumX2, sumY2;

  bool solved;

  double pLen1, pLen2;         // principal lengths, pLen1 >= pLen2
  double uPAI, vPAI;           // components of unit vector along 1st principal axis
  double ssError;

  void solve();

public:
  LineFit()
  {
    clear();
  }

  void clear();

  double area() const { return sumW; }

  double centerX() const { return sumX / sumW;}

  double centerY() const { return sumY / sumW;}

  double firstPrincipalLength() { solve(); return pLen1;}

  double secondPrincipalLength() { solve(); return pLen2;}

  bool hasPrincipalAxes() { solve(); return uPAI != 0 || vPAI != 0;}

  double firstPrinciaplAxisU() { solve(); return uPAI;}

  double firstPrinciaplAxisV() { solve(); return vPAI;}

  double secondPrinciaplAxisU() { solve(); return -vPAI;}

  double secondPrinciaplAxisV() { solve(); return uPAI;}

  double firstPrincipalAngle() { solve(); return atan2(vPAI, uPAI);}

  double secondPrincipalAngle() { solve(); return atan2(uPAI, -vPAI);}

  double sumSquaredError() { solve(); return ssError;}

  double rmsError() { return sqrt(sumSquaredError() / sumW);}

  void add(double w, double x, double y);

  void add(double x, double y)
  {
    add(1.0, x, y);
  }

  void sub(double w, double x, double y);

  void sub(double x, double y)
  {
    sub(1.0, x, y);
  }
};

// ****************************
// *                           
// *  Simple Images in Memory  
// *                           
// ****************************
//
// Very lightweight images in memory.

class ImageLiteBase
{
  int _x0, _y0;

  int _wd, _ht;

  int _pitch;

public:
  // Copy/assign OK

  // The point (x0, y0) specifies the physical center of the image, e.g. as
  // acquired from a camera. It is necessary to know this point to compute
  // image/field mappings (homography). As images are derived from others, by
  // windowing or by neighborhood processing (e.g. gradient), this center point
  // moves relative to image pixel (0,0) and this shift is accounted for.
  //
  // (x0, y0) are in half-pixel units, i.e. s32.1 (signed 32-bit integers, one
  // bit to the right of the binary point). There are several reasons that a
  // half-pixel shift may occur:
  //    * The original source image (e.g. from a camera) may be of odd width
  //      or height (unlikely, od course).
  //    * A neighborhood processing operation may use an odd-diameter neighborhood
  //    * Features (e.g. edges) can be detected with half-pixel resolution.
  int x0() const { return _x0; }
  int y0() const { return _y0; }

  // Size of the image in whole pixels
  int width() const { return _wd;}
  int height() const { return _ht; }

  // Amount to add to an address to move +1 in y, in units of pixel size
  int pitch() const { return _pitch; }

  // Default construct zero size image
  ImageLiteBase() { _wd = _ht = 0; }

  // Construct, specifying all parameters
  ImageLiteBase(int x0, int y0, int wd, int ht, int pitch)
  {
    _x0 = x0;
    _y0 = y0;
    _wd = wd;
    _ht = ht;
    _pitch = pitch;
  }

  // Construct, placing (x0, y0) at the center. 
  ImageLiteBase(int wd, int ht, int pitch)
  {
    _x0 = wd;   // This is wd/2 because x0 is in half-pixel units
    _y0 = ht;
    _wd = wd;
    _ht = ht;
    _pitch = pitch;
  }

  // Construct an image that results from neighborhood processing src with
  // specified neighborhood diameter
  ImageLiteBase(const ImageLiteBase& src, int diam, int pitch)
  {
    _x0 = src.x0() - diam;
    _y0 = src.y0() - diam;
    _wd = src.width() - diam;
    _ht = src.height() - diam;
    _pitch = pitch;
  }

  // Construct a window of src
  ImageLiteBase(const ImageLiteBase& src, int x0, int y0, int wd, int ht)
  {
    _x0 = src.x0() - (x0 << 1);
    _y0 = src.y0() - (y0 << 1);
    _wd = wd;
    _ht = ht;
    _pitch = src.pitch();
  }
};

// This template class adds pixels of type T to the base
template <class T>
class ImageLite : public ImageLiteBase
{
  T* _pixels;

public:
  ImageLite() { _pixels = 0; }

  ImageLite(int x0, int y0, int wd, int ht, int pitch, T* pixels)
    : ImageLiteBase(x0, y0, wd, ht, pitch), _pixels(pixels)
  {}

  ImageLite(int wd, int ht, int pitch, T* pixels)
    : ImageLiteBase(wd, ht, pitch), _pixels(pixels)
  {}

  ImageLite(const ImageLiteBase& src, int diam, int pitch, T* pixels)
    : ImageLiteBase(src, diam, pitch), _pixels(pixels)
  {}

  ImageLite(const ImageLiteBase& src, T* pixels)
    : ImageLiteBase(src), _pixels(pixels)
  {}

  ImageLite(const ImageLite& src, int x0, int y0, int wd, int ht)
    : ImageLiteBase(src, x0, y0, wd, ht)
  {
    _pixels = src.pixelAddr(x0, y0);
  }

  bool isNull() const { return _pixels == 0; }

  T* pixelAddr(int x, int y) const { return _pixels + y * pitch() + x; }
  T* pixelAddr() const { return _pixels; }
};

typedef ImageLite<uint8_t > ImageLiteU8;
typedef ImageLite<uint16_t> ImageLiteU16;

// A Yuv image is a special case.
class YuvLite : public ImageLiteBase
{
  uint8_t* _pixels;

public:
  YuvLite() {}

  YuvLite(int wd, int ht, int pitch, uint8_t* pixels)
    : ImageLiteBase(wd, ht, pitch), _pixels(pixels)
  {}

  YuvLite(const YuvLite& src, int x0, int y0, int wd, int ht)
    : ImageLiteBase(src, x0, y0, wd, ht)
  {
    _pixels = src.pixelAddr(x0, y0);
  }

  uint8_t* pixelAddr(int x, int y) const { return _pixels + 2 * y * pitch() + 4 * x; }
  uint8_t* pixelAddr() const { return _pixels; }
};

}
}

#endif
