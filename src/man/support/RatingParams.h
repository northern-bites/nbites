#pragma once

#include <algorithm>

#include <stdlib.h>
#include <math.h>
#include <list>
#include <fstream>


// ***************
// *             *
// *  Constants  *
// *             *
// ***************
//
// These constants define the number of bits for Y, U, and V after summing pixels

enum
{
  YBits   = 10,
  YRange  = 1 << YBits,
  YMax    = 255 << (YBits - 8),

  UVBits  = 9,
  UVRange = 1 << UVBits,
  UVMask   = UVRange - 1,
  UVZero  = 1 << (UVBits - 1),

  YCoeffBits = 16 - YBits + UVBits
};


// ***********************
// *                     *
// *  Utility Functions  *
// *                     *
// ***********************

inline int min(int x, int y)
{
  return x <= y ? x : y;
}

inline int max(int x, int y)
{
  return x >= y ? x : y;
}

inline int limit(int x, int bits)        
{
  return min(max(x, 0), (1 << bits) - 1);
}

// *****************************
// *                           *
// *  An SSE2 Double Quadword  *
// *                           *
// *****************************

struct DQWord
{
  short words[8];

  DQWord() {}

  void load(int x)
  {
    for (int i = 0; i < 8; ++i)
      words[i] = (short)x;
  }

  void load(int x, int y)
  {
    for (int i = 0; i < 8; i += 2)
    {
      words[i    ] = (short)x;
      words[i + 1] = (short)y;
    }
  }

};

// *************************************
// *                                   *
// *  Parameters for a Computed Color  *
// *                                   *
// *************************************
//
// Each of these parameters is a double quadword containing the U parameters
// in the even words and the V parameters in the odd words. The ASM code
// evaluates all fuzzy thresholds for U and V in parallel, which means that
// U and V have to be both min values or both max values. If they differ
// (e.g. orange), then V is inverted.
struct RatingParams
{
    DQWord darkZero;      // UV values that map to fuzzy 0 at Y = 0
    DQWord yCoeff;        // Change in darkZero values per unit Y above 0
    DQWord fuzzySpread;   // Fuzzy spread in UV units
    DQWord inverseFuzzy;  // 1 / spread, u16.8

    // These functions are to fetch parameters for the C++ version that is not SIMD parallel // ???
    int darkUV0   (int uv) const { return darkZero    .words[uv]; }
    int yuvCoeff  (int uv) const { return yCoeff      .words[uv]; }
    int fuzzyUV   (int uv) const { return fuzzySpread .words[uv]; }
    int invFuzzyUV(int uv) const { return inverseFuzzy.words[uv]; }

    // For C++, fetch the UV value that maps tp fuzzy 0 and the specified Y
    int uv0(int uv, int y) const { return darkUV0(uv) + (y * yuvCoeff(uv) >> 16);}

    // For C++, get the fuzzy value specifying how much x is above the fuzzy threshold
    // 0 <= result <= 255
    int fuzzyMin(int uv, int y, int x) const
    {
    return min(max(x - uv0(uv, y), 0), fuzzyUV(uv)) * invFuzzyUV(uv) >> 8;
    }

    // For C++, get the fuzzy value specifying how much x is below the fuzzy threshold
    // 0 <= result <= 255
    int fuzzyMax(int uv, int y, int x) const
    {
    return min(max(uv0(uv, y) - x, 0), fuzzyUV(uv)) * invFuzzyUV(uv) >> 8;
    }

    // For C++, get the score corresponding to (y,u,v), which is the fuzzy AND of
    // the amount that U and V are above the threshold
    unsigned char scoreMin(int y, int u, int v) const
    {
    return (unsigned char)min(fuzzyMin(0, y, u), fuzzyMin(1, y, v));
    }

    // For C++, get the score corresponding to (y,u,v), which is the fuzzy AND of
    // the amount that U and V are below the threshold
    unsigned char scoreMax(int y, int u, int v) const
    {
    return (unsigned char)min(fuzzyMax(0, y, u), fuzzyMax(1, y, v));
    }

    int fix(float uv, int zero, bool invert);

    
    // Load this color parameter from the indicated values. The fuzzy spreads are
    // positive if the threshold is a minimum value and negative if the threshold
    // is a maximum value.
    void load(float darkU0, float darkV0, float lightU0, float lightV0, float fuzzyU, float fuzzyV);


};


// A complete set of color parameters
struct Colors
{
  RatingParams white;   
  RatingParams orange;
  RatingParams green;

  Colors() {
    white .load(-0.02f, -0.02f,  0.25f,  0.25f, -0.055f, -0.055f);
    orange.load( 0.133f,  0.053f, -0.133f,  0.107f, -0.06f,  0.06f);
    green .load( 0.077f,  0.010f, -0.057f, -0.230f, -0.06f, -0.06f);
  }
};

