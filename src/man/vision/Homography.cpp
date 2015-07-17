// ********************************
// *                              *
// *  Perspective Transformation  *
// *                              *
// ********************************

#include "Homography.h"
#include "Hough.h"
#include "NBMath.h"

#include <boost/math/distributions/normal.hpp>

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
  panRate(0);
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

void FieldHomography::panAzimuth(double iy, double x, double y, double& xp, double& yp) const
{
  double panAz = -panRate() * iy;   // negative because +iy is earlier in rolling exposure
  double cp = cos(panAz);           // Could make this faster with the small angle approx
  double sp = sin(panAz);           //   cp = 1, sp = panAz
  xp = x * cp - y * sp;
  yp = x * sp + y * cp;
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
  panAzimuth(iy, h11 * cx + h12 * cy + h13 * cz, h21 * cx + h22 * cy + h23 * cz, wx, wy);
  wx += h14;
  wy += h24;

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

  double cp, sp;
  panAzimuth(iy, ca, sa, cp, sp);

  double j11 =   cp * st * iy + ( sp * sr - cp * cr * ct) * flen();
  double j12 = -(cp * st * ix + ( sp * cr + cp * sr * ct) * flen());
  double j21 =  -sp * st * iy + ( cp * sr + sp * cr * ct) * flen();
  double j22 =   sp * st * ix + (-cp * cr + sp * sr * ct) * flen();
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

  double cp, sp;
  panAzimuth(iy, ca, sa, cp, sp);

  double j11 = sp * st * ix + (-cp * cr + sp * sr * ct) * flen();
  double j12 = cp * st * ix + ( sp * cr + cp * sr * ct) * flen();
  double j21 = sp * st * iy - ( cp * sr + sp * cr * ct) * flen();
  double j22 = cp * st * iy + ( sp * sr - cp * cr * ct) * flen();
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

bool FieldHomography::calibrateFromStar(const FieldLineList& lines)
{
  StarCal sc(*this);
  if (sc.add(lines))
  {
    roll(sc.roll());
    tilt(sc.tilt());
    return true;
  }
  return false;
}

// *****************************
// *                           *
// *  Star Target Calibration  *
// *                           *
// *****************************

bool StarCal::add(const FieldLineList& lines)
{
  for (int i = 0; i < (int)lines.size(); ++i)
  {
    double vpx, vpy;
    if (lines[i][0].intersect(lines[i][1], vpx, vpy) && fabs(lines[i][0].ux()) > 0.3)
      fit.add(vpx - ix0, vpy - iy0);  // relative to optical axis
  }

  // If we didn't find exactly three suitable field lines, fail
  if (fit.area() != 3) {
    std::cout << "Failed" << std::endl;
    return false;
  } else {
    std::cout << "Success" << std::endl;
    return true;
  }
}

double StarCal::roll()
{
  return sMod(fit.firstPrincipalAngle(), M_PI);
}

double StarCal::tilt()
{
  double imageDistanceToHorizon
    = fabs(fit.secondPrinciaplAxisU() * fit.centerX() + fit.secondPrinciaplAxisV() * fit.centerY());
  return (M_PI / 2) - atan(imageDistanceToHorizon / f);
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

// TODO params
double GeoLine::error(const GeoLine& other, bool test) const
{
  double normalizedT = (r() > 0 ? t() : t() - M_PI);
  double rDiff = fabs(fabs(r()) - fabs(other.r()));
  double tDiff = fabs(sMod(normalizedT - other.t(), M_PI));

  boost::math::normal_distribution<> rGaussian(0, 100);
  boost::math::normal_distribution<> tGaussian(0, 10*TO_RAD);

  // TODO properly sample
  double rProb = pdf(rGaussian, rDiff);
  double tProb = pdf(tGaussian, tDiff);

  if (test) {
    std::cout << "In error," << std::endl;
    std::cout << "Model, " << r() << "," << t() << std::endl;
    std::cout << "Observation, " << other.r() << "," << other.t() << std::endl;
    std::cout << rDiff << "-" << tDiff << std::endl;
    std::cout << rProb << "+" << tProb << std::endl;
  }

  return rProb * tProb;
}

void GeoLine::translateRotate(double xTrans, double yTrans, double rotation)
{
    // Find point on line pre-transformation (use endpoints)
    double x1, y1, x2, y2;
    endPoints(x1, y1, x2, y2);

    // Translate and rotate
    double x1t, y1t, x2t, y2t;
    man::vision::translateRotate(x1, y1, xTrans, yTrans, rotation, x1t, y1t);
    man::vision::translateRotate(x2, y2, xTrans, yTrans, rotation, x2t, y2t);

    // Calculate new t and unit vector
    t(rotation + t());

    // Dot product of point on line with new unit vector to find new r
    r(ux() * x1t + uy() * y1t);

    // Set endpoints
    setEndPoints(qDist(x1t, y1t), qDist(x2t, y2t));
}

void GeoLine::inverseTranslateRotate(double xTrans, double yTrans, double rotation, bool debug)
{
    // Find point on line pre-transformation (use endpoints)
    double x1, y1, x2, y2;
    endPoints(x1, y1, x2, y2);

    // Translate and rotate
    double x1t, y1t, x2t, y2t;
    man::vision::inverseTranslateRotate(x1, y1, xTrans, yTrans, rotation, x1t, y1t);
    man::vision::inverseTranslateRotate(x2, y2, xTrans, yTrans, rotation, x2t, y2t);

    // Calculate new t and unit vector
    t(-rotation + t());

    // Dot product of point on line with new unit vector to find new r
    r(ux() * x1t + uy() * y1t);

    // If negative r, then make r positive and set t accordingly
    if (r() < 0) {
        r(fabs(r()));
        t(t() + M_PI);
    }

    if (debug) {
      std::cout << "In inverseTranslateRotate:" << std::endl;
      std::cout << x1 << "," << y1 << std::endl;
      std::cout << x1t << "," << y1t << std::endl;
    }

    // Set endpoints
    setEndPoints(qDist(x1t, y1t), qDist(x2t, y2t));
}

void GeoLine::imageToField(const FieldHomography& h)
{
  if (valid())
  {
    // Get endpoints in image coords
    double ix1, ix2, iy1, iy2;
    endPoints(ix1, iy1, ix2, iy2);

    // Convert to field coords
    double fx1, fx2, fy1, fy2;
    bool belowHorizon1 = h.fieldCoords(ix1, iy1, fx1, fy1);
    bool belowHorizon2 = h.fieldCoords(ix2, iy2, fx2, fy2);

    // If both endpoints are below the horizon, result is invalid
    if (!belowHorizon1 && !belowHorizon2)
    {
      setInvalid();
      return;
    }

    // If one endpoint is below the horizon, check the midpoint. If it is above
    // the horizon, the result is invalid. Otherwise adjust the endpoint that is
    // above so that keepFraction of the line segment below the horizon is used.
    if (!(belowHorizon1 && belowHorizon2))
    {
      double xMid, yMid;
      if (!h.fieldCoords(0.5 * (ix1 + ix2), 0.5 * (iy1 + iy2), xMid, yMid))
      {
        setInvalid();
        return;
      }

      GeoLine horizon;
      horizon.setToHorizon(h, 0);
      double hx, hy;
      intersect(horizon, hx, hy);
      const double keepFraction = 0.875;
      if (belowHorizon1)
        h.fieldCoords(ix1 + keepFraction * (hx - ix1), iy1 + keepFraction * (hy - iy1),
                      fx2, fy2);
      else
        h.fieldCoords(ix2 + keepFraction * (hx - ix2), iy2 + keepFraction * (hy - iy2),
                      fx1, fy1);
    }

    // For lines with endpoints reasonably separated, get field line by mapping
    // endpoints and then constructing line between them. Mapping endpoints is
    // more accurate when h.panRate() is non-zero.
    if (length() > 1)
    {
      setUnitVec(fy1 - fy2, fx2 - fx1);
      r(ux() * fx1 + uy() * fy1);
    }

    // For lines with endpoints too close together, get field line by mapping
    // unit normal vector. This would be used, for example, if a line is made
    // with no endpoints, in which case both endpoints are 0. 
    else
    {
      // Get field coordinates of the line origin (any point on the line will do),
      // which in general will not be the line origin in field coords
      double ix0 = r() * ux();
      double iy0 = r() * uy();
      double xf, yf;
      h.fieldCoords(ix0, iy0, xf, yf);

      // Map a unit vector (length doesn't matter but must not be 0) in the +
      // direction along the line to a field vector, which will not in general
      // be of unit length. Use it to set angle and unit vector in field coords.
      double uxf, uyf;
      h.fieldVector(ix0, iy0, uy(), -ux(), uxf, uyf);
      setUnitVec(-uyf, uxf);

      // Now we can set r in field coords
      r(ux() * xf + uy() * yf);
    }

    // Now that we have r and t, we can set endpoints
    setEndPoints(qDist(fx1, fy1), qDist(fx2, fy2));
  }
}

void GeoLine::setToHorizon(const FieldHomography& fh, int imageWidth)
{
  t(fh.roll() + M_PI / 2);
  r(fh.flen() * tan(M_PI / 2 - fh.tilt()) - (fh.ix0() * ux() + fh.iy0() * uy()));

  double ep = fabs(imageWidth / (2 * uy()));
  setEndPoints(-ep, ep);
}

string GeoLine::print(bool pretty) const
{
  if (pretty)
  {
    if (valid())
      return strPrintf("%8.2f,%7.2f  [%7.1f .. %7.1f]",
                       r(), t() * (180 / M_PI), ep0(), ep1());
    else
      return "---- (invalid) ----";
  }
  else
    return strPrintf("%.8g %.8g %.8g %.8g", r(), t(), ep0(), ep1());
}

// *****************************
// *                           *
// *  Synthetic RoboCup Field  *
// *                           *
// *****************************

void syntheticField(YuvLite& img, FieldHomography fh)
{
  // The field is defined by a list of non-intersecting oriented rectangles in millimeters.
  static RectangleF fieldLines[] =
  {
    RectangleF(-3025, -4525,   50, 9050),   // left sideline
    RectangleF( 2975, -4525,   50, 9050),   // right sideline
    RectangleF(-2975,  4475, 5950,   50),   // back line
    RectangleF(-2975,   -25, 5950,   50),   // center line
    RectangleF(-1125,  3875, 2250,   50),   // front goal box line
    RectangleF(-1125,  3925,   50,  550),   // left goal box line
    RectangleF( 1075,  3925,   50,  550),   // right goal box line
    RectangleF(  -75,  3175,  150,   50),   // penalty mark horizontal arm
    RectangleF(  -25,  3225,   50,   50),   // penalty mark back vertical arm
    RectangleF(  -25,  3125,   50,   50),   // penalty mark front vertical arm
    RectangleF(  -25,    25,   50,   50),   // center mark back arm
    RectangleF(  -25,   -75,   50,   50)    // center mark front arm
  };

  // Used to make center circle
  RectangleF ccTangent = RectangleF(725, -50, 50, 100);

  // Convert homography to mm (a metter of convenience in defining the field)
  fh.wx0(fh.wx0() * 10);
  fh.wy0(fh.wy0() * 10);
  fh.wz0(fh.wz0() * 10);

  // Loop over every image pixel
  for (int j = 0; j < 2 * img.height(); ++j)
    for (int i = 0; i < 2 * img.width(); ++i)
    {
      double ix = i - img.width(), iy = img.height() - j;
      double wx, wy;
      int i2 = i / 2;
      if (fh.fieldCoords(ix, iy, wx, wy))
      {
        // Pixel below he horizon, get oriented rectangle in field coordinates that
        // approximately corresponds to the pixel.
        const double pixelRadius = 0.6;
        double rxx, rxy, ryx, ryy;
        fh.fieldVector(ix, iy, pixelRadius, 0, rxx, rxy);
        fh.fieldVector(ix, iy, 0, pixelRadius, ryx, ryy);
        double rx = max(fabs(rxx), fabs(ryx));
        double ry = max(fabs(rxy), fabs(ryy));
        RectangleF pixel((float)(wx - rx), (float)(wy - ry), (float)(2 * rx), (float)(2 * ry));

        // Sum the areas of intersection of the pixel with each rectangle.
        double z = 0;
        for (int r = 0; r < sizeof(fieldLines) / sizeof(fieldLines[0]); ++r)
          z += fieldLines[r].intersectArea(pixel);
        z /= pixel.area();

        // Center circle
        double wr = sqrt(wx * wx + wy * wy);
        double cs = wx / wr;
        double sn = wy / wr;
        double cxx = cs * rxx - sn * rxy;
        double cxy = sn * rxx + cs * rxy;
        double cyx = cs * ryx - sn * ryy;
        double cyy = sn * ryx + cs * ryy;
        rx = max(fabs(cxx), fabs(cyx));
        ry = max(fabs(cxy), fabs(cyy));
        pixel = RectangleF((float)(wr - rx), (float)(-ry), (float)(2 * rx), (float)(2 * ry));
        double c = ccTangent.intersectArea(pixel) / pixel.area();
        z = z + c - z * c;

        // Convert area to brightness, store in image
        uint8_t y = (uint8_t)((0.625 * z + 0.25) * 255);
        img.y(i, j, y);

        // Color the field green
        if ((i & 1) == 0)
          if (fabs(wx) <= 3700 && fabs(wy) <= 5200)
          {
            y = (uint8_t)(0.3 * (z - 1) * 128 + 128);
            img.u(i2, j, y);
            img.v(i2, j, y);
          }
          else
          {
            img.u(i2, j, 128);
            img.v(i2, j, 128);
          }
      }
      else
      {
        // Color the sky blue
        img.y(i, j, 192);
        if ((i & 1) == 0)
        {
          img.u(i2, j, 144);
          img.v(i2, j, 112);
        }
      }
    }
}

}
}
