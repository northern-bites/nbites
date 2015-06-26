// ********************************
// *                              *
// *  Perspective Transformation  *
// *                              *
// ********************************

#ifndef _homography_
#define _homography_

#include "Stdafx.h"
#include "Vision.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

namespace man {
namespace vision {

class GeoLine;
class FieldLineList;

class FieldHomography
{
  // Homography matrix. The h4* row is 0 0 0 1 and implied.
  double h11, h12, h13, h14;
  double h21, h22, h23, h24;
  double h31, h32, h33, h34;

  double _roll, _az, _tilt;
  double _ix0, _iy0;            // Image coordinates of optical axis
  double _f;                    // focal length, pixels

  double ca, ct, cr;            // cosine azimuth, tilt, roll
  double sa, st, sr;            // cosine azimuth, tilt, roll

  double _panRate;              // radians/pixel

  // Rotate (x, y) by the azimuth difference due to the exposure time difference
  // from the optical axis to iy that results from panRate
  void panAzimuth(double iy, double x, double y, double& xp, double& yp) const;

  // The homogaphy matrix is set from client-visible parameters and is invisible
  // to the client. It is computed on demand, not when the parameters are changed.
  // This makes some member functions that the client sees as const actually non-
  // const as far as C++ is concerned. We cast away const-ness to override the
  // compiler and keep the client's view of the class.
  bool needsCompute;
  void compute();
  void compute() const { ((FieldHomography*)this)->compute(); }

public:
  // Construct with default values;
  FieldHomography(bool topCamera = true);

  // Image coordinates of optical axis, relative to the center of the image
  double ix0() const { return _ix0; }
  void ix0(double x) { _ix0 = x; }
  double iy0() const { return _iy0; }
  void iy0(double y) { _iy0 = y; }

  // Camera rotations
  double roll() const { return _roll; }
  void roll(double r) { _roll = r; needsCompute = true; }

  double tilt() const { return _tilt; }
  void tilt(double t) { _tilt = t; needsCompute = true; }

  double azimuth() const { return _az; }
  void azimuth(double a) { _az = a; needsCompute = true; }

  // Focal length in pixels
  double flen() const { return _f; }
  void flen(double f) { _f = f; }

  // (Wx0, Wy0) is the origin of the world coordinate system relative to the point
  // on the field directly below the camera aperture. If it is (0,0), the mapping
  // is robot-relative.
  double wx0() const { return h14; }
  void wx0(double x) { h14 = x; }
  double wy0() const { return h24; }
  void wy0(double y) { h24 = y; }

  // Height above field of camera aperture
  double wz0() const { return -h34; }
  void wz0(double z) { h34 = -z; }

  // Pan rate corrects for the rolling shutter when the robot head is panning, i.e.
  // azimuth is changing at a constant rate. Pan rate is in radians/pixel, calculated
  // as follows:
  //
  //                         radians/second
  //   radians/pixel = ------------------------------
  //                   frames/second * y-pixels/frame
  //
  // So if the robot head is panning at 2 radians/second, the camera is running at 30
  // frames/second, and the image contans 240 y-pixels/frame, then
  //
  //                2
  //   panRate = -------- = 0.000278
  //             30 * 240
  //
  // panRate is positive for panning right to left, or maybe it's left to right, it's
  // really hard to get ths right so you better test it.
  double panRate() const { return _panRate; }
  void panRate(double rpp) { _panRate = rpp; }

  // Map image coordinates to field coordinates.
  // Returns true if the point is below the horizon and therefore actually on the field
  bool fieldCoords(double ix, double iy, double& wx, double& wy) const;

  // Map the image vector (dix, diy) at the image point (ix, iy) to the field vector
  // (dwx, dwy).
  void fieldVector(double ix, double iy, double dix, double diy, double& dwx, double& dwy) const;

  // Map the field vector (dwx, dwy) at the image point (ix, iy) to the field vector
  // (dix, diy).
  void imageVector(double ix, double iy, double dwx, double dwy, double& dix, double& diy) const;

  // Recover tilt from known perpendicular or parallel lines
  static double tiltSensitivity;    // min dE/dt for Newton's method
  static double tiltConvergeLimit;  // Newton can stop when error is < this
  static double tiltConvergeRate;   // Error must be reduced by this factor each iteration

  // Given two lines in image coordinates that are known to be perpendicular in world
  // coordinates, and an initial tilt guess in this object, compute the tilt that would
  // make the lines perpendicular in world coordinates.
  //
  // Return true if a valid result is obtained, based on above judgments of sensitivity
  // and convergence. If false is returned, tilt output is set to the current value.
  //
  // If diagnostics is not null, set the string to a report of convergence.
  bool visualTiltPerpendicular(const GeoLine& a, const GeoLine& b, double& tilt,
                               std::string* diagnostics) const;
  
  // Same as above, but for lines known to be parallel in world coordinates.
  bool visualTiltParallel(const GeoLine& a, const GeoLine& b, double& tilt,
                          std::string* diagnostics) const;

  // Calibrate tilt and roll using "star target". If successful returns true and
  // updates tilt and roll.
  bool calibrateFromStar(const FieldLineList& lines);
};

// *****************************
// *                           *
// *  Star Target Calibration  *
// *                           *
// *****************************
//
// Calibrate roll and tilt from one or more field line lists, each derived from
// an image of the star target.
//
// The two paired image lines of a field line are parallel. Their intersection
// is on the horizon. The horizontal leg of the star target has its intersection
// too far away to be useful (maybe infinitely far). Find the other three points
// and fit a line. A field line list is ignored if all three field lines are not
// present 

// The roll is the angle of the horizon, in the range [-PI/2 .. PI/2)

// The tilt is calculated from the distance from the optical axis to the horizon. That
// distance is the dot product of the unit vector normal to the line with a vector
// to any point on the line. The center of mass is on the line.

class StarCal
{
  double ix0, iy0;   // optical axis relative to center of image
  double f;          // focal length

  LineFit fit;

public:
  StarCal(const FieldHomography& fh) : ix0(fh.ix0()), iy0(fh.iy0()), f(fh.flen()) {}

  void clear() { fit.clear(); }

  // Add another set of field lines from a star target image.
  bool add(const FieldLineList&);

  // Returns the roll and tilt from the field lines seen so far.
  double tilt();
  double roll();
};

// ********************
// *                  *
// *  Geometric Line  *
// *                  *
// ********************

class GeoLine
{
  // Infinite polarized line in polar coordinates (r,t). Polarized means that
  // the line has a + side and a - side, which is used e.g. for image lines to
  // indicate gradient direction. Let u be a unit vector normal to the line in
  // the + direction, and let q be a vector from the origin to the line. Then r =
  // u * q and t is the angle of u anticlockwise from the x axis in radians, i.e.
  // u = (cos(t),sin(t)). Note that anticlockwise is conceptual, and lines may be
  // come from and be displayed on devices that typically have left-handed
  // coordinate systems. The line is all points (x,y) such that (x,y) * u = r. 
  double _r, _t;
  double _ux, _uy;    // components of u
  double _ep0, _ep1;  // endpoints, distance along line from origin

protected:
  void r(double x) { _r = x; }
  void t(double x)
  {
    _t = uMod(x, 2.0 * M_PI);
    _ux = cos(t());
    _uy = sin(t());

  }

  void setUnitVec(double dx, double dy)
  {
    unitVec(dx, dy, _ux, _uy);
    t(atan2(dy, dx));
  }

  void setEndPoints(double ep0, double ep1)
  {
    _ep0 = min(ep0, ep1);
    _ep1 = max(ep0, ep1);
  }

public:
  double r() const { return _r; }
  double t() const { return _t; }

  double ux() const { return _ux; } 
  double uy() const { return _uy; }

  // Endpoints, coordinate parallel to the line relative to the point closest to
  // the origin. See qDist below. ep0 <= ep1.
  double ep0() const { return _ep0; }
  double ep1() const { return _ep1; }

  double length() const { return ep1() - ep0(); }

  // effect   Construct from specified data
  // note     End points may be swapped so that ep0 <= ep1.
  void set(double r, double t, double ep0 = 0, double ep1 = 0)
  {
    this->r(r);
    this->t(t);
    setEndPoints(ep0, ep1);
  }

  // Copy/assign OK

  // Polarized distance from specified point to this line. Positive on + side
  // of line. 
  double pDist(double x, double y) const { return x * ux() + y * uy() - r(); }

  // Polarized distance parallel to the line from the origin to the specified point.
  // Facing towards the + side of the line, positive distance is to the right.
  double qDist(double x, double y) const { return x * uy() - y * ux(); }

  // effect   Compute the intersection of this line with another line. The coordinates
  //          of the point are (px/pg, py/pg). If there is no intersection (lines
  //          are parallel), pg will be 0.
  // note     The reason for the odd output format is that some clients need the
  //          ratios for numerical accuracy. If you just want the point, use
  //          the overload below.
  void intersect(const GeoLine& other, double& px, double& py, double& pg) const;

  // returns  true iff this line and the other line intersect
  // effect   set px, py to the intersection of this line and the other line
  bool intersect(const GeoLine& other, double& px, double& py) const;

  // Get endpoints.
  void endPoints(double& x1, double& y1, double& x2, double& y2) const;

  // Get endpoints in raw image coordinates, i.e. in a left-handed system where
  // the center of the image is at (x0, y0). Useful for drawing.
  void rawEndPoints(double x0, double y0,
                    double& x1, double& y1, double& x2, double& y2) const;

  // The separation between two lines is the sum of the polarized distance from
  // the center of all four endpoints to each line. For parallel lines, this is
  // the correct geometric separation. For nearly parallel lines it is
  // well-defined and reasonable. The center of all four endpoints is a point on
  // the line that bisects the two lines. Separation is positive when said center
  // is on the positive side of both lines (gradients pointing towards each
  // other), and negative when the center is on the negative side of both lines
  // (gradients pointing away from each other. Separation is approximately zero
  // otherwise. 
  double separation(const GeoLine& other) const;
  
  // Assuming this and other should be the same line, calculate the error.
  // NOTE used in particle filter.
  double error(const GeoLine& other, bool test = false) const;

  // Translation rotation of line in plane. 
  // NOTE used in particle filter.
  void translateRotate(double xTrans, double yTrans, double rotation);

  // Map this image line to what we would see if roll were 0 and the optical axis
  // was at the center of the image
  void correctRollAndAxis(const FieldHomography&);

  // Map this image line to field coordinates.  Preserve polarity and endpoints.
  void imageToField(const FieldHomography&);

  // "pretty" is for human consumption. Not pretty is for a high-precision string
  // that is easy to parse by another computer program, e.g. C#
  std::string print(bool pretty = false) const;
};

// *****************************
// *                           *
// *  Synthetic RoboCup Field  *
// *                           *
// *****************************

// Fill in the specified image with a synthetically rendered regulation RoboCup field as
// seen by a robot whose pose is specified by the specified homography. The image can
// be of any size. Remember that a YuvLite image of size 320x240 corresponds to what is
// more conventionally called 640x480 (see above comment with YuvLite). Remember to get
// the focal length in the homography correct, which generally is a function of image
// size (544 for 640x480, 272 for 320x240 as YUV is conventionally defined).
//
// Note that the fh argument is not a reference, but a copy because it is modified
// internally.
void syntheticField(YuvLite& img, FieldHomography fh);

class CalibrationParams {
  double roll;
  double tilt;

public:
  CalibrationParams() { roll = tilt = 0.0; }
  CalibrationParams(double r, double t) { roll = r; tilt = t; }
  
  double getRoll() { return roll; }
  void setRoll(double r) { roll = r; }
  double getTilt() { return tilt; }
  void setTilt(double t) { tilt = t; }

};

}
}

#endif
