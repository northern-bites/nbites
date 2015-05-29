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

using namespace std;

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
extern "C" void runLengthU8 (uint8_t * source, int count, int thresh, int32_t* runs);
extern "C" void runLengthU16(uint16_t* source, int count, int thresh, int32_t* runs);

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

inline double min(double x, double y)
{
  return x <= y ? x : y;
}

inline double max(double x, double y)
{
  return x >= y ? x : y;
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

string strPrintf(const char* format, ...);


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
    __asm
    {
      rdtsc;
      mov lo, eax
      mov hi, edx
    }
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


#endif
