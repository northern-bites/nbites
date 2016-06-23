#include <math.h>
#include <stdint.h>
#include <limits>
#include "LeastSquaresLine.hpp"

LeastSquaresLine::LeastSquaresLine()
{
   sumX2 = 0;
   sumY2 = 0;
   sumXY = 0;
   sumX  = 0;
   sumY  = 0;
   sum1  = 0;
}

bool LeastSquaresLine::getLineABC(int *a, int *b, int *c)
{
   /* Standard least squares. We are solving the equation
    *
    * |sum (x^2)   sum (x)| |c1|   |sum (y * x)|
    * |sum (x)     sum (1)| |c2| = |sum (y)    |
    */

   /* Ints can overflow here if the numbers are too large. From here
    * on use 64 bit types
    */
   int64_t sumX2 = this->sumX2;
   int64_t sumY2 = this->sumY2;
   int64_t sumXY = this->sumXY;
   int64_t sumX  = this->sumX;
   int64_t sumY  = this->sumY;
   int64_t sum1  = this->sum1;

   int64_t detX = sumX2 * sum1 - sumX * sumX;
   int64_t detY = sumY2 * sum1 - sumY * sumY;

   if (detX == 0 && detY == 0) {
      return false;
   }

   /* Choose the larger determinate. The larger determinate results
    * in smaller error. We should use the smaller of the x and y errors
    * to get the most accurate fit
    *
    * Now we just solve the equation above
    */

   /* Annoyingly the results can also overflow standard ints */
   int64_t aa, bb, cc;
   if (detX > detY) {
      aa = (sum1 * sumXY - sumX * sumY );
      bb = -detX;
      cc = (sumX2 * sumY - sumX * sumXY);
   } else {
      aa = -detY;
      bb = (sum1 * sumXY - sumY * sumX );
      cc = (sumY2 * sumX - sumY * sumXY);
   }

   /* Check for overflows */
   const int64_t max_ret = std::numeric_limits<int>::max() / 1024;

   int64_t max_overflow = aa / max_ret;
   if (max_overflow < bb / max_ret) {
      max_overflow = bb / max_ret;
   }
   if (max_overflow < cc / max_ret) {
      max_overflow = cc / max_ret;
   }

   if (max_overflow != 0) {
      aa /= max_overflow;
      bb /= max_overflow;
      cc /= max_overflow;
   }

   *a = aa;
   *b = bb;
   *c = cc;

   return true;
}

