// ********************************************
// *                                          *
// *  Vision 2105 General Utilities and Math  *
// *                                          *
// ********************************************

#include "Vision.h"
#include "Stdafx.h"

#include <stdio.h>
#include <stdarg.h>

using namespace std;

namespace man {
namespace vision {

// ***************
// *             *
// *  Utilities  *
// *             *
// ***************

string strPrintf(const char* format, ...)
{
  char buf[1024];
  va_list ap;
  va_start(ap, format);
#ifdef WIN32
  vsprintf_s(buf, format, ap);
#else
  vsprintf(buf, format, ap);
#endif
  va_end (ap);
  return buf;
}

// ***********************
// *                     *
// *  Linear Regression  *
// *                     *
// ***********************

void LineFit::clear()
{
  sumW = sumX = sumY = sumXY = sumX2 = sumY2 = _count = 0;
  solved = false;
}

void LineFit::add(double w, double x, double y)
{
  double wx = w * x;
  double wy = w * y;
  sumW  += w;
  sumX  += wx;
  sumY  += wy;
  sumX2 += wx * x;
  sumY2 += wy * y;
  sumXY += wx * y;
  _count++;
  solved = false;
}

void LineFit::sub(double w, double x, double y)
{
  sumW  -= w;;
  sumX  -= w * x;
  sumY  -= w * y;
  sumX2 -= w * x * x;
  sumY2 -= w * y * y;
  sumXY -= w * x * y;
  _count--;
  solved = false;
}

void LineFit::solve()
{
  if (solved)
    return;

  double a = sumW * sumX2 - sumX * sumX;
  double b = sumW * sumY2 - sumY * sumY;
  double c = 2.0 * ((double)sumW * sumXY - (double)sumX * sumY);
  double d = sqrt(c * c + (a - b) * (a - b));

  pLen1 = sqrt(1.5 * (a + b + d)) / sumW;
  pLen2 = sqrt(1.5 * (a + b - d)) / sumW;

  if (d == 0.0)
      // No principal monemts
      uPAI = vPAI = 0.0;
  else
  {
      uPAI = sqrt(((a - b) / d + 1) / 2);
      vPAI = sqrt((1 - (a - b) / d) / 2);
      if (c < 0)
          uPAI = -uPAI;
  }

  double cs = -vPAI, sn = uPAI;
  double r = cs * centerX() + sn * centerY();
  ssError = cs * cs * sumX2 + sn * sn * sumY2 + 2 * cs * sn * sumXY - 2 * r * (cs * sumX + sn * sumY) + r * r * sumW;
  ssError = max(ssError, 0.0);

  solved = true;
}


}
}
