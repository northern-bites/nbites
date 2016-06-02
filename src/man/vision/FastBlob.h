// ********************************
// *                              *
// *  Fast Connectivity Analysis  *
// *                              *
// ********************************

#ifndef _fastBlob_
#define _fastBlob_

#include "Stdafx.h"
#include "Vision.h"

#include <list>

namespace man {
namespace vision {

// *****************
// *               *
// *  Blob Result  *
// *               *
// *****************

class Blob : public LineFit
{
  int _areaAboveFirst;
  int _w1;

public:
  Blob(int weightOne)
  {
    clear();
    _areaAboveFirst = 0;
    _w1 = weightOne;
  }

  void add(double w, double x, double y, bool aboveFirst)
  {
    LineFit::add(w, x, y);
    _areaAboveFirst += (int)aboveFirst;
  }

  void merge(Blob b)
  {
      LineFit::merge(b);
  }

  int areaAboveFirst() const { return _areaAboveFirst; }

  double area() const { return LineFit::area() / _w1; }

  std::string print() const;
};

// ***************************
// *                         *
// *  Connectivity Analysis  *
// *                         *
// ***************************

class Connectivity
{
  // FIFO parameters
  enum
  {
    LogFifoSize = 14,
    FifoSize = 1 << LogFifoSize,
    FifoMask = FifoSize - 1
  };

  // Point is 8 bytes so compiler can use scaled indexed addressing
  struct Point
  {
    uint8_t* p;     // address of pixel
    uint32_t xy;    // packed (x,y) coords: x + (y << 16)
  };

  // The fifo is allocated on the heap so it can be large. Overflow checking is not done,
  // so the fifo needs to be pretty big. The allocation is done in the constructor, not
  // each time connectivity is run.
  Point* fifo;

  // Fast inline write to fifo
  void fifoWrite(int& writeP, uint8_t* p, uint32_t xy)
  {
    fifo[writeP].p = p;
    fifo[writeP].xy = xy;
    writeP = (writeP + 1) & FifoMask;
  }

  // Fast inline read from fifo
  void fifoRead(int& readP, uint8_t*& p, uint32_t& xy)
  {
    p = fifo[readP].p;
    xy = fifo[readP].xy;
    readP = (readP + 1) & FifoMask;
  }

  void connect(int x, int y, uint8_t* p, int pitch);

  int32_t* runs;  // run-length buffer for fast version
  int runSize;

  // Parameters, see public comments
  int _t1, _t2;
  int _minAboveFirst;
  float _minWeight;
  bool _fast;
  int _wt0, _wtRange;

  // Run info
  uint32_t _ticks;
  int _alignment;

  // no copy/assign
  Connectivity(const Connectivity&);
  Connectivity& operator=(const Connectivity&);

public:
  // Blob must contain minAreaAboveFirst "seed" pixels above firstThreshold and minArea
  // pixels connected to a seed and above secondThreshold. LSB of these thresholds
  // is ignored and reads as 0, since LSB of pixels is ignored.
  int firstThreshold() const { return _t1; }
  void firstThreshold(int t) { _t1 = t & ~1; }
  int secondThreshold() const { return _t2; }
  void secondThreshold(int t) { _t2 = t & ~1; }

  // The first and second thresholds are hard thresholds as needed for connectivity.
  // The weight fuzzy threshold maps gray levels to weights for the blob
  // geometric data. Use integers instead of FuzzyThr class to avoid all the divides.
  int weightThr0() const { return _wt0; }
  int weightThrRange() const { return _wtRange; }
  int weightThr1() const { return _wt0 + _wtRange; }
  void weightThr(int t0, int t1) { _wt0 = t0; _wtRange = t1 - t0; }

  float minWeight() const { return _minWeight; }
  void minWeight(double w) { _minWeight = (float)w; }
  int minAreaAboveFirst() const { return _minAboveFirst; }
  void minAreaAboveFirst(int a) { _minAboveFirst = a; }

  // Use fast version (ASM helpers)
  bool fast() const { return _fast; }
  void fast(bool b) { _fast = b; }

  // List of blobs found by last run
  std::list<Blob> blobs;

  // Execution time of last run in clock ticks
  uint32_t ticks() const { return _ticks; }

  // image address mod 16 of last run, for testing
  int alignment() const { return _alignment; }

  Connectivity();

  ~Connectivity()
  {
    delete[] fifo;
    delete[] runs;
  }

  // Run connectivity and put results in blobs.
  // Low bit of image pixels is ignored and left in an undefined state.
  // There are no restrictions on image alignment.
  // width must be >= 17 - (image & 15)
  // pitch must be a multiple of 16 or a GP exception will occur
  // First and last row and column are not included in any blob
  void run(uint8_t* image, int width, int height, int pitch);

  // Generate synthetic test image, run connectivity, check results. Return true if
  // test passes.
  bool test(std::string& message);

  std::string print() const;
};

}
}

#endif
