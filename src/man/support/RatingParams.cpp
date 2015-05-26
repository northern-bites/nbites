#include "RatingParams.h"

using namespace std;

// Convert a floating point color parameter to an integer
int RatingParams::fix(float uv, int zero, bool invert)
{
  int w = limit((int)(UVRange * uv + zero + 0.5), UVBits);
  if (invert)
    w ^= UVMask;
  return w;
}

void RatingParams::load(float darkU0, float darkV0, float lightU0, float lightV0, float fuzzyU, float fuzzyV)
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
  yCoeff.load(((lu0 - du0) << YCoeffBits) / YMax, ((lv0 - dv0) << YCoeffBits) / YMax);
  fuzzySpread.load(fu, fv);
  inverseFuzzy.load((255 << 8) / fu, (255 << 8) / fv);
}
