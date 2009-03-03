/*
 * Northern Bites file with math expressions needed in many modules/threads
 */

#ifndef NBMath_h_DEFINED
#define NBMath_h_DEFINED
#include <cmath>

static const double PI = M_PI;
static const double DEG_OVER_RAD = 180.0 / M_PI;
static const double RAD_OVER_DEG = M_PI / 180.0;
static const float M_PI_FLOAT = static_cast<float>(M_PI);
static const float TO_DEG = 180.0f/M_PI_FLOAT;
#ifndef TO_RAD
static const float TO_RAD = M_PI_FLOAT/180.0f;
#endif


inline static int ROUND(float x) {
  if ((x-static_cast<int>(x)) >= 0.5) return (static_cast<int>(x)+1);
  if ((x-static_cast<int>(x)) <= -0.5) return (static_cast<int>(x)-1);
  else return (int)x;
}

static const float clip(const float value,const float minValue, const float maxValue) {
    if (value > maxValue)
        return maxValue;
    else if (value < minValue)
        return minValue;
    else
        return value;
}

static const float clip(const float value, const float minMax){
    return clip(value,-minMax,minMax);
}
#endif //NBMath_h
