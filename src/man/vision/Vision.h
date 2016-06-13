// ********************************************
// *                                          *
// *  Vision 2105 General Utilities and Math  *
// *                                          *
// ********************************************

#ifndef _vision_
#define _vision_

#include "Stdafx.h"

#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include <iostream>

namespace man {
namespace vision {

	class DebugImage;

// Scan array of unsigned bytes or words for values above the specified
// threshold. Place in the runs array the offset of each such byte/word found,
// terminated by -1.
//
// Source values are processed 16 at a time in parallel. runLengthU8/16 processes
// a multiple of 16 bytes/words, and so will read beyond the end of source if
// count is not a multiple of 16. If the memory does not exist, a fault will
// occur.
//
// The maximum size needed for runs is count, rounded up to a multiple of 16,
// plus 1 for the terminator.
//
// runs will not have any garbage due to the overreading, but some data in runs
// beyond the terminator may be overwritten if count is not a multiple of 16.
//
// Signed versions can be produced with trivial effort using the macros in
// runLength.s.
extern "C" void _runLengthU8 (uint8_t * source, int count, int thresh, int32_t* runs);
extern "C" void _runLengthU16(uint16_t* source, int count, int thresh, int32_t* runs);

// ***********
// *         *
// *  Point  *
// *         *
// ***********

typedef std::pair<double, double> Point;

// *************
// *           *
// *  Utility  *
// *           *
// *************

void* alignedAlloc(size_t size, int alignBits, void*& block);

inline int min(int x, int y)
{
  return x <= y ? x : y;
}

inline int max(int x, int y)
{
  return x >= y ? x : y;
}

inline float min(float x, float y)
{
  return x <= y ? x : y;
}

inline float max(float x, float y)
{
  return x >= y ? x : y;
}

inline double min(double x, double y)
{
  return x <= y ? x : y;
}

inline double max(double x, double y)
{
  return x >= y ? x : y;
}

inline double dist(double x1, double y1, double x2, double y2)
{
    double x = x1 - x2;
    double y = y1 - y2;

    return sqrt(pow(x, 2) + pow(y, 2));
}

inline double uMod(double x, double mod)
{
  return x - floor(x / mod) * mod;
}

inline double sMod(double x, double mod)
{
  return x - floor(x / mod + 0.5) * mod;
}

inline double diffRadians(double a, double b)
{
  return fabs(sMod(a - b, 2 * M_PI));
}

inline void unitVec(double x, double y, double& u, double& v)
{
  double g = sqrt(x * x + y * y);
  u = x / g;
  v = y / g;
}

inline void translateRotate(double x, double y, double transX, double transY, 
                            double rotation, double& xTransformed, double& yTransformed)
{
    double xRotated = x*cos(rotation) - y*sin(rotation);
    double yRotated = x*sin(rotation) + y*cos(rotation);

    xTransformed = xRotated + transX;
    yTransformed = yRotated + transY;
}

inline void inverseTranslateRotate(double x, double y, double transX, double transY, 
                                   double rotation, double& xTransformed, double& yTransformed)
{
    double xTranslated = x - transX;
    double yTranslated = y - transY;

    xTransformed = xTranslated*cos(rotation) + yTranslated*sin(rotation);
    yTransformed = -xTranslated*sin(rotation) + yTranslated*cos(rotation);
}

inline void cartesianToPolar(double x, double y, double& r, double& t)
{
    r = sqrt(x * x + y * y);
    t = atan2(y, x);
}

inline void polarToCartesian(double r, double t, double& x, double& y)
{
    x = r * cos(t);
    y = r * sin(t);
}

std::string strPrintf(const char* format, ...);

// ************
// *          *
// *  Timing  *
// *          *
// ************
//
// High-resolution clock-tick timer
class TickTimer
{
  uint64_t startTime;

  uint64_t read()
  {
    uint32_t lo, hi;
#ifdef WIN32
    __asm
    {
      rdtsc;
      mov lo, eax
      mov hi, edx
    }
#else
    // TODO fix
    asm("rdtsc;\n"
        "mov %%eax, %0;\n"
        "mov %%edx, %1;\n"
        :"=r" (lo), "=r" (hi)
        :
        :"%eax", "%edx"
        );
#endif
    return ((uint64_t)hi << 32) | lo;
  }

public:
  // Copy/assign OK

  TickTimer() { start(); }

  void start()
  {
    startTime = read();
  }

  uint64_t time()
  {
    return read() - startTime;
  }

  uint32_t time32()
  {
    return (uint32_t)time();
  }
};

// *****************
// *               *
// *  Fuzzy Logic  *
// *               *
// *****************

// A fuzzy logic value is a number [0 .. 1] that can be produced by a fuzzy threshold and
// operated on by and, or, and not operations.

class FuzzyThr
{
  float _t0, _range;

public:
  // Copy/assign OK

  float t0() const { return _t0; }
  float t1() const { return _t0 + _range; }
  float range() const { return _range; }

  FuzzyThr(float t0, float t1)
  {
    _t0 = t0;
    _range = t1 - t0;
  }
};

class Fool
{
  float _f;

  Fool(float f)
    : _f(f)
  {}

public:
  // Copy/assign OK

  float f() const { return _f; }

  Fool(const FuzzyThr& t, float x)
  {
    if (t.range() == 0)
      _f = x >= t.t0() ? 1.0f : 0.0f;
    else
      _f = min(max((x - t.t0()) / t.range(), 0.0f), 1.0f);
  }

  Fool operator&(Fool a)
  {
    return Fool(min(f(), a.f()));
  }

  Fool operator|(Fool a)
  {
    return Fool(max(f(), a.f()));
  }

  Fool operator!()
  {
    return Fool(1 - f());
  }
};

inline Fool operator>(float x, const FuzzyThr& t)
{
  return Fool(t, x);
}

inline Fool operator<(float x, const FuzzyThr& t)
{
  return !(x > t);
}

// ***********************
// *                     *
// *  Linear Regression  *
// *                     *
// ***********************

class LineFit
{
  double sumW;
  double sumX, sumY, sumXY, sumX2, sumY2;
  int _count;

  bool solved;

  double pLen1, pLen2;         // principal lengths, pLen1 >= pLen2
  double uPAI, vPAI;           // components of unit vector along 1st principal axis
  double ssError;

  void solve();

public:
  LineFit()
  {
    clear();
  }

  void clear();

  void merge(LineFit b)
  {
      sumW += b.sumW;
      sumX += b.sumX;
      sumY += b.sumY;
  }


  double area() const { return sumW; }

  double centerX() const { return sumX / sumW;}

  double centerY() const { return sumY / sumW;}

  int count() const { return _count; }

  double firstPrincipalLength() { solve(); return pLen1;}

  double secondPrincipalLength() { solve(); return pLen2;}

  bool hasPrincipalAxes() { solve(); return uPAI != 0 || vPAI != 0;}

  double firstPrinciaplAxisU() { solve(); return uPAI;}

  double firstPrinciaplAxisV() { solve(); return vPAI;}

  double secondPrinciaplAxisU() { solve(); return -vPAI;}

  double secondPrinciaplAxisV() { solve(); return uPAI;}

  double firstPrincipalAngle() { solve(); return atan2(vPAI, uPAI);}

  double secondPrincipalAngle() { solve(); return atan2(uPAI, -vPAI);}

  double sumSquaredError() { solve(); return ssError;}

  double rmsError() { return sqrt(sumSquaredError() / sumW);}

  void add(double w, double x, double y);

  void add(double x, double y)
  {
    add(1.0, x, y);
  }

  void sub(double w, double x, double y);

  void sub(double x, double y)
  {
    sub(1.0, x, y);
  }
};

// ****************************
// *                           
// *  Simple Images in Memory  
// *                           
// ****************************
//
// Very lightweight images in memory.

class ImageLiteBase
{
  int _x0, _y0;

  int _wd, _ht;

  int _pitch;

public:
  // Copy/assign OK

  // The point (x0, y0) specifies the physical center of the image, e.g. as
  // acquired from a camera. It is necessary to know this point to compute
  // image/field mappings (homography). As images are derived from others, by
  // windowing or by neighborhood processing (e.g. gradient), this center point
  // moves relative to image pixel (0,0) and this shift is accounted for.
  //
  // (x0, y0) are in half-pixel units, i.e. s32.1 (signed 32-bit integers, one
  // bit to the right of the binary point). There are several reasons that a
  // half-pixel shift may occur:
  //    * The original source image (e.g. from a camera) may be of odd width
  //      or height (unlikely, of course).
  //    * A neighborhood processing operation may use an odd-diameter neighborhood
  //    * Features (e.g. edges) can be detected with half-pixel resolution.
  int x0() const { return _x0; }
  int y0() const { return _y0; }

  // Size of the image in whole pixels
  int width() const { return _wd;}
  int height() const { return _ht; }

  // Amount to add to an address to move +1 in y, in units of pixel size
  int pitch() const { return _pitch; }

  // Default construct zero size image
  ImageLiteBase() { _wd = _ht = 0; }

  // Construct, specifying all parameters
  ImageLiteBase(int x0, int y0, int wd, int ht, int pitch)
  {
    _x0 = x0;
    _y0 = y0;
    _wd = wd;
    _ht = ht;
    _pitch = pitch;
  }

  // Construct, placing (x0, y0) at the center.
  ImageLiteBase(int wd, int ht, int pitch)
  {
    _x0 = wd;   // This is wd/2 because x0 is in half-pixel units
    _y0 = ht;
    _wd = wd;
    _ht = ht;
    _pitch = pitch;
  }

  // Construct an image that results from neighborhood processing src with
  // specified neighborhood diameter
  ImageLiteBase(const ImageLiteBase& src, int diam, int pitch)
  {
    _x0 = src.x0() - diam;
    _y0 = src.y0() - diam;
    _wd = src.width() - diam;
    _ht = src.height() - diam;
    _pitch = pitch;
  }

  // Construct a window of src
  ImageLiteBase(const ImageLiteBase& src, int x0, int y0, int wd, int ht)
  {
    _x0 = src.x0() - (x0 << 1);
    _y0 = src.y0() - (y0 << 1);
    _wd = wd;
    _ht = ht;
    _pitch = src.pitch();
  }
};

// This template class adds pixels of type T to the base
template <class T>
class ImageLite : public ImageLiteBase
{
  T* _pixels;

public:
  ImageLite() { _pixels = 0; }

  ImageLite(int x0, int y0, int wd, int ht, int pitch, T* pixels)
    : ImageLiteBase(x0, y0, wd, ht, pitch), _pixels(pixels)
  {}

  ImageLite(int wd, int ht, int pitch, T* pixels)
    : ImageLiteBase(wd, ht, pitch), _pixels(pixels)
  {}

  ImageLite(const ImageLiteBase& src, int diam, int pitch, T* pixels)
    : ImageLiteBase(src, diam, pitch), _pixels(pixels)
  {}

  ImageLite(const ImageLiteBase& src, T* pixels)
    : ImageLiteBase(src), _pixels(pixels)
  {}

  ImageLite(const ImageLite& src, int x0, int y0, int wd, int ht)
    : ImageLiteBase(src, x0, y0, wd, ht)
  {
    _pixels = src.pixelAddr(x0, y0);
  }

  bool isNull() const { return _pixels == 0; }

  T* pixelAddr(int x, int y) const { return _pixels + y * pitch() + x; }
  T* pixelAddr() const { return _pixels; }
};

typedef ImageLite<uint8_t > ImageLiteU8;
typedef ImageLite<uint16_t> ImageLiteU16;

// A Yuv image is a special case. The width and height is defined to be one-half of
// the width and height of the Y pixels, and therefore the width and one-half the
// height of the U and V pixels. For example, a 640x480 YUV image as conventionally
// defined is 640x480 in Y and 320x480 in U and V. YuvLite calls this 320x240. This
// guarantees that windows are valid YuvLite images, and that pixels are square. So
// effectively each "pixel" of a YuvLite image has 2x2 Y values and 1x2 U and V values.
class YuvLite : public ImageLiteBase
{
  uint8_t* _pixels;

public:
  YuvLite() {}

  YuvLite(int wd, int ht, int pitch, uint8_t* pixels)
    : ImageLiteBase(wd, ht, pitch), _pixels(pixels)
  {}

  YuvLite(const YuvLite& src, int x0, int y0, int wd, int ht)
    : ImageLiteBase(src, x0, y0, wd, ht)
  {
    _pixels = src.pixelAddr(x0, y0);
  }

  // Pixel address of a Y 2x2, UV 1x2 pixel block
  uint8_t* pixelAddr(int x, int y) const { return _pixels + 2 * y * pitch() + 4 * x; }
  uint8_t* pixelAddr() const { return _pixels; }

  // Get and set Y pixels using conventional coordinates:
  //    0 <= i < 2 * width()
  //    0 <= j < 2 * height()
  int y(int i, int j) const { return (pixelAddr() + j * pitch())[2 * i]; }
  void y(int i, int j, uint8_t z) {  (pixelAddr() + j * pitch())[2 * i] = z; }

  // Get and set U,V pixels using conventional coordinates:
  //    0 <= i < width()
  //    0 <= j < 2 * height()
  int u(int i, int j) const { return (pixelAddr() + j * pitch())[4 * i + 1]; }
  void u(int i, int j, uint8_t z) {  (pixelAddr() + j * pitch())[4 * i + 1] = z; }
  int v(int i, int j) const { return (pixelAddr() + j * pitch())[4 * i + 3]; }
  void v(int i, int j, uint8_t z) {  (pixelAddr() + j * pitch())[4 * i + 3] = z; }
};

/* DebugImage
 * This class provides extra images that can be "drawn" on as a way of
 * debugging. The class is really just a 2D array filled with "colors"
 * that are just numbers. Later the nbtool can interpret those numbers
 * as whatever colors it likes.
 * Note: all code is #ifdef'd as absolute insurance it doesn't run on the robot
 */

class DebugImage : ImageLiteU8
{
private:
	//DebugImage(const DebugImage&);
	//DebugImage& operator= (const DebugImage&);

public:

	DebugImage(int wd, int ht, uint8_t *d)
	//: ImageLiteU8(wd, ht, wd, (uint8_t *)malloc(wd * ht * sizeof(uint8_t)))
		:ImageLiteU8(wd, ht, wd, d)
		{}

	DebugImage() {}

	uint8_t* pixArray() { return pixelAddr(); }

	//uint8_t  pixAddr(int x, int y) const { return * pixelAddr(x, y); }

	void reset() {
#ifdef OFFLINE
		if (pixelAddr(0, 0) == NULL) {
			return;
		}
		for (int i = 0; i < height(); i++) {
			for (int j = 0; j < width(); j++) {
				*pixelAddr(j, i) = 0;
			}
		}
#endif
	}

    /**
	 * Draw a rectangle in the debugging image.
	 *
	 * @param left     x value of left edge
	 * @param right    x value of right edge
	 * @param bottom   y value of bottom
	 * @param top      y value of top
	 * @param c        the color we'd like to draw
	 */
	void drawBox(int left, int right, int bottom, int top, int c) {
#ifdef OFFLINE
		if (left < 0) {
			left = 0;
		}
		if (top < 0) {
			top = 0;
		}
		int boxwidth = right-left;
		int boxheight = bottom-top;

		for (int i = left; i < left + boxwidth; i++) {
			if (top >= 0 &&
				top < height() &&
				i >= 0 &&
				i < width()) {
				*pixelAddr(i, top) = static_cast<unsigned char>(c);
			}
			if ((top + boxheight) >= 0 &&
				(top + boxheight) < height() &&
				i >= 0 &&
				i < width()) {
				*pixelAddr(i, top + boxheight) = static_cast<unsigned char>(c);
			}
		}
		for (int i = top; i < top + boxheight; i++) {
			if (i >= 0 &&
				i < height() &&
				left >= 0 &&
				left < width()) {
				*pixelAddr(left, i) = static_cast<unsigned char>(c);
			}
			if (i >= 0 &&
				i < height() &&
				(left+boxwidth) >= 0 &&
				(left+boxwidth) < width()) {
				*pixelAddr(left + boxwidth, i) = static_cast<unsigned char>(c);
			}
		}
#endif
	}

    /**
	 * Draw a rectangle in the debugging image.
	 *
	 * @param left     x value of left edge
	 * @param top      y value of the top edge
	 * @param width    width of the box
	 * @param height   height of the box
	 * @param c        the color we'd like to draw
	 */
	void drawRect(int left, int top, int rectwidth, int rectheight, int c)
		{
#ifdef OFFLINE
			if (left < 0) {
				rectwidth += left;
				left = 0;
			}
			if (top < 0) {
				rectheight += top;
				top = 0;
			}

			for (int i = left; i < left + rectwidth; i++) {
				if (top >= 0 && top < height() && i >= 0 && i < width()) {
					*pixelAddr(i, top) = static_cast<unsigned char>(c);
				}
				if ((top + rectheight) >= 0 &&
					(top + rectheight) < height() &&
					i >= 0 &&
					i < width()) {
					*pixelAddr(i, top + rectheight) = static_cast<unsigned char>(c);
				}
			}
			for (int i = top; i < top + rectheight; i++) {
				if (i >= 0 &&
					i < height() &&
					left >= 0 &&
					left < width()) {
					*pixelAddr(left, i) = static_cast<unsigned char>(c);
				}
				if (i >= 0 &&
					i < height() &&
					(left+rectwidth) >= 0 &&
					(left+rectwidth) < width()) {
					*pixelAddr(left + rectwidth, i) = static_cast<unsigned char>(c);
				}
			}
#endif
		}

    /**
	 * Draw a line at the specified coordinates on the debugging
	 * image.
	 *
	 * @param x       start x
	 * @param y       start y
	 * @param x1      finish x
	 * @param y1      finish y
	 * @param c       color we'd like to draw
	 */
	void drawLine(int x, int y, int x1, int y1, int c)
		{
#ifdef OFFLINE
			float slope = static_cast<float>(y - y1) / static_cast<float>(x - x1);
			int sign = 1;
			if ((abs(y - y1)) > (abs(x - x1))) {
				slope = 1.0f / slope;
				if (y > y1)  {
					sign = -1;
				}
				for (int i = y; i != y1; i += sign) {
					int newx = x +
						static_cast<int>(slope * static_cast<float>(i - y) );

					if (newx >= 0 && newx < width() && i >= 0 && i < height()) {
						*pixelAddr(newx, i) = static_cast<unsigned char>(c);
					}
				}
			} else if (slope != 0) {
				//slope = 1.0 / slope;
				if (x > x1) {
					sign = -1;
				}
				for (int i = x; i != x1; i += sign) {
					int newy = y +
						static_cast<int>(slope * static_cast<float>(i - x) );

					if (newy >= 0 && newy < height() && i >= 0 && i < width()) {
						*pixelAddr(i, newy) = static_cast<unsigned char>(c);
					}
				}
			}
			else if (slope == 0) {
				int startX = min(x, x1);
				int endX = max(x, x1);
				for (int i = startX; i <= endX; i++) {
					if (y >= 0 && y < height() && i >= 0 && i < width()) {
						*pixelAddr(i, y) = static_cast<unsigned char>(c);
					}
				}
			}
#endif
		}

	void drawDot(int x, int y, int c)
		{
#ifdef OFFLINE
			if (y > 0 && x > 0 && y < (height()) && x < (width())) {
				*pixelAddr(x, y) = static_cast<unsigned char>(c);
			}
#endif
		}

    // Prerequisite - point is within bounds of screen
	void drawX(int x, int y, int c)
		{
#ifdef OFFLINE
			// Mid point
/*			pixelAddr([y-2][x-2] = static_cast<unsigned char>(c);
			pixelAddr([y-1][x-1] = static_cast<unsigned char>(c);
			pixelAddr([y][x] = static_cast<unsigned char>(c);
			pixelAddr([y+1][x+1] = static_cast<unsigned char>(c);
			pixelAddr([y+2][x+2] = static_cast<unsigned char>(c);

			pixelAddr([y-2][x+2] = static_cast<unsigned char>(c);
			pixelAddr([y-1][x+1] = static_cast<unsigned char>(c);

			pixelAddr([y+1][x-1] = static_cast<unsigned char>(c);
			pixelAddr([y+2][x-2] = static_cast<unsigned char>(c);*/
#endif
		}

    /* drawPoint()
	 * Draws a crosshair or a 'point' on the fake image at some given x, y, and
	 * with a given color.
	 * @param x       center of the point
	 * @param y       center y value
	 * @param c       color to draw
	 */
	void drawPoint(int x, int y, int c)
		{
#ifdef OFFLINE
			if (y > 0 && x > 0 && y < (height()) && x < (width())) {
				*pixelAddr(x, y) = static_cast<unsigned char>(c);
			}if (y+1 > 0 && x > 0 && y+1 < (height()) && x < (width())) {
				*pixelAddr(x, y+1) = static_cast<unsigned char>(c);
			}if (y+2 > 0 && x > 0 && y+2 < (height()) && x < (width())) {
				*pixelAddr(x, y+2) = static_cast<unsigned char>(c);
			}if (y-1 > 0 && x > 0 && y-1 < (height()) && x < (width())) {
				*pixelAddr(x, y-1) = static_cast<unsigned char>(c);
			}if (y-2 > 0 && x > 0 && y-2 < (height()) && x < (width())) {
				*pixelAddr(x, y-2) = static_cast<unsigned char>(c);
			}if (y > 0 && x+1 > 0 && y < (height()) && x+1 < (width())) {
				*pixelAddr(x+1, y) = static_cast<unsigned char>(c);
			}if (y > 0 && x+2 > 0 && y < (height()) && x+2 < (width())) {
				*pixelAddr(x+2, y) = static_cast<unsigned char>(c);
			}if (y > 0 && x-1 > 0 && y < (height()) && x-1 < (width())) {
				*pixelAddr(x-1, y) = static_cast<unsigned char>(c);
			}if (y > 0 && x-2 > 0 && y < (height()) && x-2 < (width())) {
				*pixelAddr(x-2, y) = static_cast<unsigned char>(c);
			}
#endif
		}
};

template <typename T>
struct point {
	T x, y;
	point() : x(0), y(0) { }
	point(const T _x, const T _y)
		: x(_x), y(_y) { }

	bool operator== (const point& secondPt) const {
		return (x == secondPt.x && y == secondPt.y);
	}

	// NEWVISION
	//friend std::ostream& operator<< (std::ostream &o, const point &c) {
	//	return o << "(" << c.x << "," << c.y << ")";
	//}
};

// ***************
// *             *
// *  Rectangle  *
// *             *
// ***************

template <class T>
class Rectangle
{
  T _x0, _y0;
  T _wd, _ht;

public:
  // Copy/assign OK

  Rectangle() {}

  Rectangle(T x0, T y0, T wd, T ht);

  T x0() const { return _x0; }
  T y0() const { return _y0; }
  T wd() const { return _wd; }
  T ht() const { return _ht; }
  T x1() const { return x0() + wd(); }
  T y1() const { return y0() + ht(); }

  T area() const { return wd() * ht(); }

  Rectangle& intersect(const Rectangle& r);
  static Rectangle intersect(const Rectangle& r1, const Rectangle& r2)
  {
    return Rectangle(r1).intersect(r2);
  }

  T intersectArea(const Rectangle& r) const
  {
    T w = min(x1(), r.x1()) - max(x0(), r.x0());
    T h = min(y1(), r.y1()) - max(y0(), r.y0());
    if (w > 0 && h > 0)
      return w * h;
    else
      return 0;
  }
};

template <class T>
Rectangle<T>::Rectangle(T x0, T y0, T wd, T ht)
{
  if (wd > 0 && ht > 0)
  {
    _x0 = x0;
    _y0 = y0;
    _wd = wd;
    _ht = ht;
  }
  else
  {
    _x0 = 0;
    _y0 = 0;
    _wd = 0;
    _ht = 0;
  }
}


template <class T>
Rectangle<T>& Rectangle<T>::intersect(const Rectangle& r)
{
  T x = max(x0(), r.x0());
  T w = min(x1(), r.x1()) - x;
  T y = max(y0(), r.y0());
  T h = min(y1(), r.y1()) - y;
  *this = Rectangle(x, y, w, h);
}

typedef Rectangle<int   > RectangleI;
typedef Rectangle<float > RectangleF;
typedef Rectangle<double> RectangleD;

}
}

#endif
