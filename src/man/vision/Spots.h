// ********************************
// *                              *
// *  Spot Filter, Ball detector  *
// *                              *
// ********************************

#ifndef _spots_
#define _spots_

#include "Stdafx.h"
#include "Vision.h"
#include "FrontEnd.h"
#include "Homography.h"

#include <list>

namespace man {
namespace vision {

// ********************
// *                  *
// *  Spot Structure  *
// *                  *
// ********************

// A spot is a place in a source image where the output of the spot filter
// is above a threshold and a local maximum. Optionally, when looking for
// balls, spots are rejected where the average green confidence is below a
// threshold, since the ball is white and its spots are black and neither
// is green.
//
// The integer spot coordinates have one bit to the right of the binary
// point (s32.1). There is a half-pixel shift in the filtered image, and
// hence the spots, when the filter diameters are even. The extra bit allows
// that to be captured. Spot coordinates are relative to the optical axis stored
// in all ImageLite<T>, which is also s32.1. Windows of images correctly
// keep track of the optical axis, so that spot coordinates will be the
// same if the spot detector is run on a window.
struct Spot
{
  int filterOutput;   // Filter output, [0 .. 255]
  int green;          // Average green confidence in inner region
  int x, y;           // Image coordinates of spot wrt optical axis (s32.1)
  int innerDiam, outerDiam;
  int spotType;       // Used for displaying debugging information
  int rawX, rawY;     // Raw coordinate value of center - used for display

  // Image coordinates of spot relative to optical axis, +y is up
  float ix() const { return  0.5f * x; }
  float iy() const { return -0.5f * y; }

  // These are intended for the overlap detection code. Note that the
  // quantities being shifted right will always be even, because
  // the (x, y) coordinates are off when innerDiam is even (half-
  // pixel shift).
  int xLo() const { return (x - innerDiam + 1) >> 1; }
  int xHi() const { return (x + innerDiam - 1) >> 1; }
  int yLo() const { return (y - innerDiam + 1) >> 1; }
  int yHi() const { return (y + innerDiam - 1) >> 1; }
};

typedef std::list<Spot> SpotList;
typedef std::list<Spot>::iterator SpotIterator;
typedef std::list<Spot>::const_iterator SpotConstIterator;

// *******************
// *                 *
// *  Spot Detector  *
// *                 *
// *******************

class SpotDetector
{
  int _initialInnerDiam;    // See public get/set functions
  int _initialOuterDiam;

  float _innerGrowRows;     // See public get/set functions
  float _outerGrowRows;

  float _innerDiamCm;       // See public get/set function

  bool _darkSpot;           // See public get/set function

  float _filterGain;        // See public get/set functions
  int _filterThreshold;
  int _greenThreshold;

  uint32_t _ticks;          // Execution time of last call to spotDetect

  // The following are internally managed memory buffers. They are allocated and grow as
  // needed with each call to the spot filter. Typically that will only happen on first
  // use, avoiding memory management overhead and heap fragmentation.
  void* filteredImageMemory;  // Block of memory that contains _filteredImage pixels
  uint8_t* filteredPixels;    // _filteredImage pixels, 16-byte aligned
  size_t filteredSize;        // size of filteredPixels array
  ImageLiteU8 _filteredImage; // Output of the last spotFilter run
  int* outerColumns;          // Spot filter column sums arrays, rounded up to multiple
  int* innerColumns;          //   of 16 for ASM code

  // Allocate (or grow) above memory buffers based on source image size
  void alloc(const ImageLiteBase&);

  // Find spots in filteredImage by peak detection, rejecting green spots if
  // green image is supplied.
  void spotDetect(const ImageLiteU8* green);

  // Vector of all spots found
  std::list<Spot> _spots;

  int _horizon;

public:
  SpotDetector();
  ~SpotDetector();

  // The inner spot filter diameter at the top of the source image
  // Note: the inner and outer diameters must be equal mod 2 (both
  // even or both odd)
  int initialInnerDiam() const { return _initialInnerDiam; }
  void initialInnerDiam(int d) { _initialInnerDiam = d; }

  // The outer spot filter diameter at the top of the source image
  int initialOuterDiam() const { return _initialOuterDiam; }
  void initialOuterDiam(int d) { _initialOuterDiam = d; }

  // The number of image rows that trigger the inner diameter to grow by 2.
  // If either value is 0, no growing occurs.
  float innerGrowRows() const { return _innerGrowRows; }
  void innerGrowRows(float r) { _innerGrowRows = r; }

  // The number of image rows that trigger the outer diameter to grow by 2
  float outerGrowRows() const { return _outerGrowRows; }
  void outerGrowRows(float r) { _outerGrowRows = r; }

  // The inner diameter in centimeters, to automatically control filter
  // region size using a homography
  float innerDiamCm() const { return _innerDiamCm; }
  void innerDiamCm(float cm) { _innerDiamCm = cm; }

  // Find bright (false) or dark (true) spots
  bool darkSpot() const { return _darkSpot; }
  void darkSpot(bool dark) { _darkSpot = dark; }

  // Filter output gain. Since the filter is averaging many input pixel
  // values, extra bits of accuracy arise. The gain effectively
  // controls how much is preserved when converting the output to a
  // uint8_t.
  // Note: Remember that for the 16-bit Y images produced by the
  // front end code, pixels are the sum of four 8-bit source pixels
  // and are therefore 10 bits. For those images, a gain of 0.25
  // is the same as a gain of 1.0 for 8-bit source images.
  float filterGain() const { return _filterGain; }
  void filterGain(float g) { _filterGain = g; }

  // A local maximum in the filter output must be at least this value
  // to be a spot.
  // Note: This threshold is in the same units as the source image,
  // independent of filterGain. Remember that 16-bit Y images have
  // 10-bit pixels.
  int filterThreshold() const { return _filterThreshold; }
  void filterThreshold(int t) { _filterThreshold = t; }

  // Reject spots whose average green confidence value in the inner
  // region is equal or above this threshold.
  int greenThreshold() const { return _greenThreshold; }
  void greenThreshold(int g) { _greenThreshold = g; }

  // Run the spot filter on a source image. The filtered image output
  // can be obtained with the filteredImage member function.
  // The template type T must be an integer type of no more than 32 bits
  template <class T>
  void spotFilter(const ImageLite<T>& src);

  // Using the specified homography and innerDiamCm, compute reasonable values
  // for the initial diameters and the grow amounts. Then run the spot filter
  // on a window of the specified source image within which the spot can be
  // resolved by the initial inner diameter. Then process the filtered image
  // to detect spots and put them in a list that can be fetched by the
  // spots member function. If a green image is specified, reject green spots.
  template <class T>
  void spotDetect(const ImageLite<T>& src, const FieldHomography& h, const ImageLiteU8* green = NULL);

  // Get the filtered image from the last run of spotFilter (and spotDetect, which
  // calls it).
  const ImageLiteU8& filteredImage() const { return _filteredImage; }

  // Get the detected spots
  const std::list<Spot>& spots() const { return _spots; }
        std::list<Spot>& spots()       { return _spots; }

  // Get horizon used by spotDetect (source image y coordinate). This is not the
  // true horizon because roll is ignored. This is primarily intended for display
  // so users can see what was done. Roll is ignored because the spot growing
  // can happen efficiently only in y.
  int horizon() const { return _horizon; }

  // Get the execution time of the last call to spotDetect
  uint32_t ticks() const { return _ticks; }
};

// ***************************
// *                         *
// *  Interface to ASM Code  *
// *                         *
// ***************************

// The following is a neat trick so that the spotFilter, which is a template function
// able to run on images of various pixel sizes, can use fast ASM code for some
// of thse sizes. First, a template function is defined that can do the column
// sum updates for any pixel type. It's inline because it's very simple and
// there's no need for function call overhead. Then, for pixel types where a
// fast ASM version exists, an inline template specialization is used to call the ASM.

template <class T>
inline void columnMove(const T* posRow, const T* negRow, int* columnSums, int count)
{
  for (int x = 0; x < count; ++x)
    columnSums[x] += posRow[x] - negRow[x];
}

#define FAST_SPOT 0

#if FAST_SPOT
extern "C" void
  _columnMove16(const uint16_t* posRow, const uint16_t* negRow, int* columnSums, int count);

template <>
inline void columnMove<uint16_t>(const uint16_t* posRow, const uint16_t* negRow, int* columnSums, int count)
{
  _columnMove16(posRow, negRow, columnSums, count);
}
#endif

// ***********************************
// *                                 *
// *  Spot Filter Template Function  *
// *                                 *
// ***********************************

template <class T>
void SpotDetector::spotFilter(const ImageLite<T>& src)
{
  alloc(src);

  // Start diameters two smaller because we're going to trigger a grow immediatly.
  // This prevents copy/paste  duplication of initialization code
  int outerDiam = initialOuterDiam() - 2;
  int innerDiam = initialInnerDiam() - 2;

  // If grow amounts are 0, use a value that will never trigger a grow
  float innerGrowRows = _innerGrowRows <= 0 ? (float)src.height() : _innerGrowRows;
  float outerGrowRows = _outerGrowRows <= 0 ? (float)src.height() : _outerGrowRows;

  // Initialize outer column sums
  for (int x = 0; x < src.width(); ++x)
    outerColumns[x] = *src.pixelAddr(x, 0);
  for (int y = 1; y < outerDiam; ++y)
    for (int x = 0; x < src.width(); ++x)
      outerColumns[x] += *src.pixelAddr(x, y);

  // Initialize inner column sums
  int innerOffset = (outerDiam - innerDiam) >> 1;
  if (innerDiam <= 0)
    for (int x = 0; x < src.width(); ++x)
      innerColumns[x] = 0;
  else
  {
    for (int x = 0; x < src.width(); ++x)
      innerColumns[x] = *src.pixelAddr(x, innerOffset);
    for (int y = 1; y < innerDiam; ++y)
      for (int x = 0; x < src.width(); ++x)
        innerColumns[x] += *src.pixelAddr(x, y + innerOffset);
  }

  // Starting the triggers at 0 forces a grow on the first row
  float outerGrowTrigger = 0;
  float innerGrowTrigger = 0;

  // Process image
  float nOuter;
  float nInner;
  float outerK, innerK;

  int yFilt = 0;  // Output image row index

  for (int y = outerDiam; y < src.height(); ++y)
  {
    bool grow = false;
    if (innerGrowTrigger <= 0)
    {
      // Grow inner region by 2
      T* pos0 = src.pixelAddr(0, y - innerOffset);
      T* pos1 = src.pixelAddr(0, y - innerOffset + 1);
      for (int x = 0; x < src.width(); ++x)
        innerColumns[x] += pos0[x] + pos1[x];

      innerDiam += 2;
      --innerOffset;
      nInner = (float)(innerDiam * innerDiam);
      innerGrowTrigger += innerGrowRows;
      grow = true;
    }
    else
    {
      // Keep inner region the same
      T* pos = src.pixelAddr(0, y - innerOffset);
      T* neg = src.pixelAddr(0, y - innerOffset - innerDiam);
      columnMove(pos, neg, innerColumns, src.width());

      innerGrowTrigger -= 1.f;
    }

    if (outerGrowTrigger <= 0 && y < src.height() - 1)
    {
      // Grow outer region by 2
      T* pos0 = src.pixelAddr(0, y);
      T* pos1 = src.pixelAddr(0, y + 1);
      for (int x = 0; x < src.width(); ++x)
        outerColumns[x] += pos0[x] + pos1[x];

      outerDiam += 2;
      ++innerOffset;
      nOuter = (float)(outerDiam * outerDiam);
      outerGrowTrigger += outerGrowRows;
      ++y;
      grow = true;
    }
    else
    {
      // Keep outer region the same
      T* pos = src.pixelAddr(0, y);
      T* neg = src.pixelAddr(0, y - outerDiam);
      columnMove(pos, neg, outerColumns, src.width());

      outerGrowTrigger -= 1.0f;
    }

    if (grow)
    {
      // If either region grew, update the filter constants
      outerK = filterGain() / (nOuter - nInner);
      innerK = filterGain() * nOuter / (nInner * (nOuter - nInner));
      if (darkSpot())
      {
        outerK = -outerK;
        innerK = -innerK;
      }
    }

    // initialize row sum
    int outerSum = outerColumns[0];
    for (int x = 1; x < outerDiam; ++x)
      outerSum += outerColumns[x];
    int innerSum = innerColumns[innerOffset];
    for (int x = 1; x < innerDiam; ++x)
      innerSum += innerColumns[x + innerOffset];

    // Generate output
    uint8_t* pDst = _filteredImage.pixelAddr(0, yFilt++);
    pDst[0] = (uint8_t)outerDiam;
    pDst[1] = (uint8_t)innerDiam;
    pDst += outerDiam >> 1;
    for (int x = outerDiam; x < src.width(); ++x)
    {
      *pDst++ = (uint8_t)min(max((int)(innerK * innerSum - outerK * outerSum), 0), 255);
      outerSum += outerColumns[x] - outerColumns[x - outerDiam];
      innerSum += innerColumns[x - innerOffset] - innerColumns[x - innerOffset - innerDiam];
    }
    *pDst++ = (uint8_t)min(max((int)(innerK * innerSum - outerK * outerSum), 0), 255);
    *pDst = 0;    // for peak detect
  }

  // Write extra row of 0's for peak detect
  uint8_t* pDst = _filteredImage.pixelAddr(outerDiam >> 1, yFilt);
  for (int x = 0; x < src.width() - outerDiam; ++x)
    pDst[x] = 0;

  _filteredImage = ImageLiteU8(_filteredImage, 0, 0, _filteredImage.width(), yFilt);
}

// *************************************
// *                                   *
// *  Spot Detector Template Function  *
// *                                   *
// *************************************

template <class T>
void SpotDetector::spotDetect(const ImageLite<T>& src, const FieldHomography& h, const ImageLiteU8* green)
{
  // Don't time this, since it will generally only do the allocation once
  alloc(src);

  TickTimer timer;

  double ct = cos(h.tilt());
  double st = sin(h.tilt());
  innerGrowRows((float)(2 * h.wz0() / (innerDiamCm() * st)));
  outerGrowRows(0.60f * innerGrowRows());

  double hy = 0.5 * src.y0() - h.flen() * ct / st;
  _horizon = (int)round(hy);
  if (hy < 0)
  {
    // Horizon above image
    int id = max((int)round((h.flen() * ct - 0.5 * src.y0() * st) * innerDiamCm() / h.wz0()), 3);
    int od = 5 * id / 3;    // Roughly balance inner and outer sizes
    od += (id ^ od) & 1;
    initialInnerDiam(id);
    initialOuterDiam(od);
    spotFilter(src);
  }
  else
  {
    // Horizon within image
    initialInnerDiam(3);
    initialOuterDiam(5);
    int y0 = (int)round(0.5 * src.y0() - (ct * h.flen() - initialInnerDiam() * h.wz0() / innerDiamCm()) / st);
    y0 = max(y0 - (initialOuterDiam() >> 1), 0);
    spotFilter(ImageLite<T>(src, 0, y0, src.width(), src.height() - y0));
  }

  spotDetect(green);

  _ticks = timer.time32();
}

}
}

#endif
