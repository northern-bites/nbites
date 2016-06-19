#pragma once

/**
 * Functions to clip a variable between an upper and lower bound.
 * Bound can be either the same for +ve & -ve or different.
 */

template <class T>
inline static T CLIP(const T& x, const T& lowerBound, const T& upperBound) {
   if (lowerBound <= upperBound) {
      // if bounds are in order
      if (x < lowerBound)
         return lowerBound;
      else if (x > upperBound)
         return upperBound;
      else
         return x;
   } else {
      // if upperBound < lowerBound
      if (x < upperBound)
         return upperBound;
      else if (x > lowerBound)
         return lowerBound;
      else
         return x;
   }
}

template <class T>
inline static T CLIP(const T &x, const T &bound) {
   return CLIP(x, -bound, bound);
}
