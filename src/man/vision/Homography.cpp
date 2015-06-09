// ********************************
// *                              *
// *  Perspective Transformation  *
// *                              *
// ********************************

#include "Homography.h"
#include "Hough.h"

#include <iostream>

using namespace std;

namespace man {
namespace vision {

double FieldHomography::tiltSensitivity   = 0.08;   // min dE/dt for Newton's method
double FieldHomography::tiltConvergeLimit = 1.0e-6; // Newton can stop when error is < this
double FieldHomography::tiltConvergeRate  = 4;      // Error must be reduced by this factor each iteration

FieldHomography::FieldHomography(bool topCamera)
{
  ix0(0);
  iy0(0);
  roll(0);
  azimuth(0);
  wx0(0);
  wy0(0);
  wz0(52);

  if (topCamera) {
      tilt(70 * (M_PI / 180));
      flen(272);
  } else {
      tilt((70 - 39.7) * (M_PI / 180));
      flen(272 / 2);
  }
}

void FieldHomography::compute()
{
  if (needsCompute)
  {
    ca = cos(azimuth());
    ct = cos(tilt());
    cr = cos(roll());
    sa = sin(azimuth());
    st = sin(tilt());
    sr = sin(roll());

    h11 = ca * cr - sa * ct * sr;
    h12 = ca * sr + sa * ct * cr;
    h13 = sa * st;
    h21 = -sa * cr - ca * ct * sr;
    h22 = -sa * sr + ca * ct * cr;
    h23 = ca * st;
    h31 = st * sr;
    h32 = -st * cr;
    h33 = ct;

    needsCompute = false;
  }
}

bool FieldHomography::fieldCoords(double ix, double iy, double& wx, double& wy) const
{
  compute();

  // Image coordinates
  ix -= ix0();
  iy -= iy0();

  // Camera coordinates
  double cz = -h34 / (ix * h31 + iy * h32 + flen() * h33);
  double cx = ix * cz;
  double cy = iy * cz;
  cz *= flen();

  // World coordinates
  wx = h11 * cx + h12 * cy + h13 * cz + h14;
  wy = h21 * cx + h22 * cy + h23 * cz + h24;

  double wz = h31 * cx + h32 * cy + h33 * cz + h34;
  if (fabs(wz) > 1.0e-6) {
    // TODO call exception "Internal error in FieldHomography"
    throw exception();
  }

  return cz > 0;
}

void FieldHomography::fieldVector(double ix, double iy, double dix, double diy,
                                  double& dwx, double& dwy) const
{
  compute();
  ix -= ix0();
  iy -= iy0();
  double j11 =   ca * st * iy + ( sa * sr - ca * cr * ct) * flen();
  double j12 = -(ca * st * ix + ( sa * cr + ca * sr * ct) * flen());
  double j21 =  -sa * st * iy + ( ca * sr + sa * cr * ct) * flen();
  double j22 =   sa * st * ix + (-ca * cr + sa * sr * ct) * flen();
  double js = h31 * ix + h32 * iy + h33 * flen();
  js = h34 / (js * js);
  dwx = js * (dix * j11 + diy * j12);
  dwy = js * (dix * j21 + diy * j22);
}

void FieldHomography::imageVector(double ix, double iy, double dwx, double dwy,
                                  double& dix, double& diy) const
{
  compute();
  ix -= ix0();
  iy -= iy0();
  double j11 = sa * st * ix + (-ca * cr + sa * sr * ct) * flen();
  double j12 = ca * st * ix + ( sa * cr + ca * sr * ct) * flen();
  double j21 = sa * st * iy - ( ca * sr + sa * cr * ct) * flen();
  double j22 = ca * st * iy + ( sa * sr - ca * cr * ct) * flen();
  double js = (h31 * ix + h32 * iy + h33 * flen()) / (flen() * h34);
  dix = js * (dwx * j11 + dwy * j12);
  diy = js * (dwx * j21 + dwy * j22);
}

bool FieldHomography::visualTiltPerpendicular(const GeoLine& a, const GeoLine& b,
                                              double& tilt, string* diagnostics) const
{
  compute();
  double ix, iy;
  a.intersect(b, ix, iy);
  double ab1 = a.uy() * b.uy();
  double ab2 = a.ux() * b.ux();
  double ab = -(a.uy() * b.ux() + a.ux() * b.uy());
      
  double j21 = sr * flen();
  double j22 = -cr * flen();

  double t = this->tilt();
  double lastE = 0;
  bool ok = true;

  if (diagnostics)
    diagnostics->clear();

  for (int i = 0; i < 4; ++i)
  {
    double ct = cos(t);
    double st = sin(t);

    double j11 = st * iy - cr * ct * flen();
    double j12 = -(st * ix + sr * ct * flen());
    double e = ab1 * (j11 * j11 + j21 * j21) + ab * (j11 * j12 + j21 * j22) + ab2 * (j12 * j12 + j22 * j22);

    double g = st * sr * ix - st * cr * iy + ct * flen();
    g *= g;
    double eMag = e / g;

    if (diagnostics)
      (*diagnostics) += strPrintf(" %8.2g,", eMag);

    eMag = fabs(eMag);

    double dj11 =  ct * iy + st * cr * flen();
    double dj12 = -ct * ix + st * sr * flen();
    double dedt = 2 * (ab1 * j11 * dj11 + ab2 * j12 * dj12) + ab * (j11 * dj12 + j12 * dj11);

    if (fabs(dedt) < tiltSensitivity * g)
    {
      if (diagnostics)
        (*diagnostics) += " ill-conditioned";
      ok = false;
      break;
    }

    if (eMag < tiltConvergeLimit)
      break;

    if (i > 0 && eMag * tiltConvergeRate > lastE)
    {
      if (diagnostics)
        (*diagnostics) += " convergence failed";
      ok = false;
      break;
    }
    lastE = eMag;

    t -= e / dedt;
  }

  tilt = ok ? t : this->tilt();
  return ok;
}

bool FieldHomography::visualTiltParallel(const GeoLine& a, const GeoLine& b,
                                         double& tilt, string* diagnostics) const
{
  compute();
  double x1, x2, y1, y2;
  a.endPoints(x1, y1, x2, y2);
  double ixa = (x1 + x2) / 2;
  double iya = (y1 + y2) / 2;
  double a1 = a.uy();
  double a2 = -a.ux();

  b.endPoints(x1, y1, x2, y2);
  double ixb = (x1 + x2) / 2;
  double iyb = (y1 + y2) / 2;
  double b1 = b.uy();
  double b2 = -b.ux();
      
  double j21 = sr * flen();
  double j22 = -cr * flen();

  double aw2 = (a1 * j21 + a2 * j22);
  double bw2 = (b1 * j21 + b2 * j22);

  double t = this->tilt();
  double lastE = 0;
  bool ok = true;

  if (diagnostics)
    diagnostics->clear();

  for (int i = 0; i < 4; ++i)
  {
    double ct = cos(t);
    double st = sin(t);

    double j11a = st * iya - cr * ct * flen();
    double j11b = st * iyb - cr * ct * flen();
    double j12a = -(st * ixa + sr * ct * flen());
    double j12b = -(st * ixb + sr * ct * flen());

    double e = (a1 * j11a + a2 * j12a) * bw2 - (b1 * j11b + b2 * j12b) * aw2;

    double g = (st * sr * ixa - st * cr * iya + ct * flen()) *
                (st * sr * ixb - st * cr * iyb + ct * flen());
    g = fabs(g);
    double eMag = e / g;

    if (diagnostics)
      (*diagnostics) += strPrintf(" %8.2g,", eMag);

    eMag = fabs(eMag);

    double dj11a =  ct * iya + st * cr * flen();
    double dj11b =  ct * iyb + st * cr * flen();
    double dj12a = -ct * ixa + st * sr * flen();
    double dj12b = -ct * ixb + st * sr * flen();
    double dedt = (a1 * dj11a + a2 * dj12a) * bw2 - (b1 * dj11b + b2 * dj12b) * aw2;

    if (fabs(dedt) < tiltSensitivity * g)
    {
      if (diagnostics)
        (*diagnostics) += " ill-conditioned";
      ok = false;
      break;
    }

    if (eMag < tiltConvergeLimit)
      break;

    if (i > 0 && eMag * tiltConvergeRate > lastE)
    {
      if (diagnostics)
        (*diagnostics) += " convergence failed";
      ok = false;
      break;
    }
    lastE = eMag;

    t -= e / dedt;
  }

  tilt = ok ? t : this->tilt();
  return ok;
}

// ********************
// *                  *
// *  Geometric Line  *
// *                  *
// ********************

void GeoLine::intersect(const GeoLine& other, double& px, double& py, double& pg) const
{
  px =  other.uy() * r() - uy() * other.r();
  py = -other.ux() * r() + ux() * other.r();
  pg = ux() * other.uy() - uy() * other.ux();
}

bool GeoLine::intersect(const GeoLine& other, double& px, double& py) const
{
  double x, y, g;
  intersect(other, x, y, g);
  if (g == 0)
  {
    px = py = 0;
    return false;
  }

  px = x / g;
  py = y / g;
  return true;
}

void GeoLine::endPoints(double& x1, double& y1, double& x2, double& y2) const
{
  double x0 = r() * ux();
  double y0 = r() * uy();
  x1 = x0 + ep0() * uy();
  y1 = y0 - ep0() * ux();
  x2 = x0 + ep1() * uy();
  y2 = y0 - ep1() * ux();
}

void GeoLine::rawEndPoints(double x0, double y0,
                           double& x1, double& y1, double& x2, double& y2) const
{
  endPoints(x1, y1, x2, y2);
  x1 += x0;
  x2 += x0;
  y1 = y0 - y1;
  y2 = y0 - y2;
}

double GeoLine::separation(const GeoLine& other) const
{
  double x1, y1, x2, y2, x3, y3, x4, y4;
  endPoints(x1, y1, x2, y2);
  other.endPoints(x3, y3, x4, y4);
  double x0 = (x1 + x2 + x3 + x4) / 4;
  double y0 = (y1 + y2 + y3 + y4) / 4;
  return pDist(x0, y0) + other.pDist(x0, y0);
}

void GeoLine::imageToField(const FieldHomography& h)
{
  // Get field coordinates of the line origin (any point on the line will do),
  // which in general will not be the line origin in field coords
  double ix0 = r() * ux();
  double iy0 = r() * uy();
  double xf, yf;
  h.fieldCoords(ix0, iy0, xf, yf);

  // Get endpoints in image coords
  double x1, x2, y1, y2;
  endPoints(x1, y1, x2, y2);

  // Map a unit vector (length doesn't matter but must not be 0) in the +
  // direction along the line to a field vector, which will not in general
  // be of unit length. Use it to set angle and unit vector in field coords.
  double uxf, uyf;
  h.fieldVector(ix0, iy0, uy(), -ux(), uxf, uyf);
  setUnitVec(-uyf, uxf);

  // Now we can set r in field coords
  r(ux() * xf + uy() * yf);

  // Map endpoints to field coords
  h.fieldCoords(x1, y1, x1, y1);
  h.fieldCoords(x2, y2, x2, y2);
  setEndPoints(qDist(x1, y1), qDist(x2, y2));
}

#if 0
  // effect   Transform this line as specified by roll and (x0, y0) in fcp. Translate then rotate.
  public void Transform(FixedCameraParams fcp)
  {
    // Rotate the line's unit vector, but don't set it yet
    double cs = Math.Cos(fcp.Roll);
    double sn = Math.Sin(fcp.Roll);
    double ux = Ux * cs - Uy * sn;
    double uy = Ux * sn + Uy * cs;

    // Get a point on the new line by mapping with fcp, then get the new R by dot product
    // with the new unit vector
    double x0, y0;
    fcp.ImageCoords(R * Ux, R * Uy, out x0, out y0);
    R = ux * x0 + uy * y0;

    // The endpoints move by the cross product of the old unit vector and the origin vector
    // in fcp. This is because rotation does not affect the U values, and translation is done
    // before rotation.
    double du = fcp.X0 * Uy - fcp.Y0 * Ux;
    U0 -= du;
    U1 -= du;

    // Now we can set the new unit vector
    setUintVec(ux, uy);
  }
#endif

string GeoLine::print() const
{
  return strPrintf("%5.1f, %5.1f, %5.1f, %5.1f, %5.1f, %5.1f", r(), t()*(180 / M_PI), ep0(), ep1(), ux(), uy());
}

}
}
