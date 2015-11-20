/**
 * angles.hpp
 * Description: Functions for converting between degrees and radians
 * Modified: 2009-11-06
 */
#pragma once

#include <cmath>
#include "basic_maths.hpp"

static const float DEG_OVER_RAD = 180 / M_PI;
static const float RAD_OVER_DEG = M_PI / 180;

inline static float RAD2DEG(const float x) {
   return ((x) * DEG_OVER_RAD);
}

inline static float DEG2RAD(const float x) {
   return ((x) * RAD_OVER_DEG);
}

/* convert radiance from our coordinate system into degrees mod 360
 * ASSUMES theta is within -Pi <= 0 < Pi!
 */
inline static float RAD2DEG360(const float theta) {
   float degree = theta * DEG_OVER_RAD; /* clip the theta within 0 ~ 2M_PI */
   if (degree < 0) {
      degree += 360;
   }
   return degree;
}

inline static float DEG360TORAD(const float degree) {
   float tmp = degree - 360 * ((int)(degree / 360));
   if (tmp >= 180) {
      tmp = tmp - 360;
   }
   return (DEG2RAD(tmp));
}

/* returns min difference between two angles, might need to add/subtract 360 as
 * needed */
inline static float MIN_ANGLE_360(const float angleA, const float angleB) {
   return (((int)(angleA + 180 - angleB) % 360 - 180));
}

// Returns the minimum number of radians between two thetas.
inline static float MIN_THETA_DIFF(float thetaA, float thetaB) {
   while (thetaA > M_PI) thetaA -= 2.0f*M_PI;
   while (thetaA < -M_PI) thetaA += 2.0f*M_PI;

   while (thetaB > M_PI) thetaB -= 2.0f*M_PI;
   while (thetaB < -M_PI) thetaB += 2.0f*M_PI;

   return fmin(fabs(thetaA - thetaB), fabs(fabs(thetaA - thetaB) - 2.0f*M_PI));
}

inline float NORMALISE(float t) {
   float r = fmod(t - M_PI, 2 * M_PI);
   return r + ((r > 0) ? -M_PI : M_PI);
}

inline float NORMALISE_PI_2(float t) {
   if (t < -M_PI_2) {
      return t + M_PI;
   } else if (t > M_PI_2) {
      return t - M_PI;
   } else {
      return t;
   }
}
