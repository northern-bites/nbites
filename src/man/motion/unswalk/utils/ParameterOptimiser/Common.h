#pragma once

#include <cstdlib>
#include <cmath>

inline float rand_range(float floor, float ceiling){
    return ((float)rand()/(float)RAND_MAX) * (ceiling-floor) + floor;
}

inline float clip(float var, float min, float max){
    if(var < min){ var = min; }
    else if(var > max){ var = max; }
    return var;
}

inline float gaussian_noise(float mean, float sd){
    // Taken from GSL Library Gaussian random distribution.
    double x, y, r2;

    do{
      /* choose x,y in uniform square (-1,-1) to (+1,+1) */
      x = -1 + 2 * (double)rand()/(double)RAND_MAX;
      y = -1 + 2 * (double)rand()/(double)RAND_MAX;

      /* see if it is in the unit circle */
      r2 = x * x + y * y;
    } while (r2 > 1.0 || r2 == 0);

    /* Box-Muller transform */
    return mean + sd * y * sqrt (-2.0 * log (r2) / r2);
}
