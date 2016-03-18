// *************************
// *                       *
// *  Edge Detection 2015  *
// *                       *
// *************************

#ifndef _edge_
#define _edge_

#include "Stdafx.h"
#include "Vision.h"
#include "Homography.h"


// ******************************************************
// *                                                    *
// *  Hash Table of Objects Indexed by 8-bit Direction  *
// *                                                    *
// ******************************************************

// ************************
// *                      *
// *  AngleElement Class  *
// *                      *
// ************************

namespace man {
namespace vision {

class AngleElement
{
  AngleElement* _next;

  int _angle;

  static float cosTable[256];

public:
  static void init();

  // Copy/assign OK but make next null. element can't be on more than
  // one list.
  AngleElement(const AngleElement& ae) : _angle(ae.angle()), _next(0) {}
  AngleElement& operator=(const AngleElement& ae)
  {
    _angle = ae.angle();
    _next = 0;
    return *this;
  }

  // Default constructor leaves members uninitialized so AngleBins can allocate a large
  // pool without having to construct every element. Up to clients to set values.
  AngleElement() {}

  int angle() const { return _angle; }
  void angle(int a) { _angle = a & 0xFF;}

  const AngleElement* next() const { return _next;}
  AngleElement* next() { return _next;}
  void add(AngleElement* list) { _next = list;}

  float radians() const { return angle() * (float)(M_PI / 128); }

  float cos() const { return cosTable[angle()]; }
  float sin() const { return cosTable[(angle() - 0x40) & 0xFF]; }

};

// *********************
// *                   *
// *  AngleBins Class  *
// *                   *
// *********************

template <class T>
class AngleBins
{
  T* bins[256];

  T* pool;
  T* freeStore;
  int poolSize;
  int freeCount;

  // No copy/assign
  AngleBins(const AngleBins&);
  AngleBins& operator=(const AngleBins&);

public:

  AngleBins(int size)
  {
    pool = new T[size];
    poolSize = size;
    reset();
  }

  void reset();

  T* add(int angle)
  {
    T* ae = freeStore++;
    --freeCount;
    ae->angle(angle);
    ae->add(bins[ae->angle()]);
    bins[ae->angle()] = ae;
    return ae;
  }

  T* binList(int angle) { return bins[angle & 0xFF];}
  const T* binList(int angle) const { return bins[angle & 0xFF];}

  void remove(T* ae);

  int count() const { return poolSize - freeCount; }
};

template <class T>
void AngleBins<T>::reset()
{
  freeStore = pool;
  freeCount = poolSize;
  for (int i = 0; i < 256; ++i)
    bins[i] = 0;
}

template <class T>
void AngleBins<T>::remove(T* ae)
{
  T* p = bins[ae->angle()];
  T* q = 0;
  while (p && p != ae)
  {
    q = p;
    p = p->next();
  }
  if (p)
    if (q)
      q->next(p->next());
    else
      bins[ae->angle()] = p->next();
}

// ********************************
// *                              *
// *  AngleBins Iterator Classes  *
// *                              *
// ********************************

template <class T>
class AngleBinsIterator
{
  AngleBins<T>* ab;       // pointer rather than refernece so default copy/assign will work
  int binIndex;
  AngleElement* current;

public:
  AngleBinsIterator(AngleBins<T>& angleBins);

  T* operator*() { return (T*)current; }
  AngleBinsIterator& operator++();
};

template <class T>
class AngleBinsConstIterator : AngleBinsIterator<T>
{
public:
  AngleBinsConstIterator(const AngleBins<T>& angleBins)
    : AngleBinsIterator<T>((AngleBins<T>&)angleBins)
  {}

  const T* operator*() { return **((AngleBinsIterator<T>*)this); }
  AngleBinsConstIterator& operator++() { return (AngleBinsConstIterator&)AngleBinsIterator<T>::operator++(); }
};

template <class T>
  AngleBinsIterator<T>::AngleBinsIterator(AngleBins<T>& angleBins)
    : ab(&angleBins)
  {
    binIndex= 0;
    current = ab->binList(0);
    while (binIndex < 256 && !current)
      current = ab->binList(++binIndex);
  }


template <class T>
AngleBinsIterator<T>& AngleBinsIterator<T>::operator++()
{
  if (current)
  {
    current = current->next();
    while (!current && ++binIndex < 256)
      current = ab->binList(binIndex);
  }
  return *this;
}

// **********
// *        *
// *  Edge  *
// *        *
// **********

class HoughLine;
class Edge;
class FieldEdge {
  double data[3];

public:
  FieldEdge(double x = 0, double y = 0, double t = 0)
  {
    data[0] = x; data[1] = y; data[2] = t;
  }
  double x() { return data[0]; }
  double y() { return data[1]; }
  double t() { return data[2]; }
};

class Edge : public AngleElement
{
  int _x, _y;
  int _mag;

  HoughLine* _memberOf;
  Edge* _nextMember;

  FieldEdge _field;

public:
  // Default constructor leaves members uninitialized so AngleBins can allocate a large
  // pool without having to construct every element. Up to clients to set values.
  Edge() {}

  // Copy/assign zeros next and nextMember. Edge can't be on more than one list
  Edge(const Edge& e) { *this = e; }
  Edge& operator= (const Edge&);

  void set(int x, int y, int mag)
  {
    _x = x;
    _y = y;
    _mag = mag;
    _memberOf = 0;
  }

  int x() const { return _x;}
  int y() const { return _y;}
  int mag() const { return _mag;}

  const Edge* next() const { return (const Edge*)AngleElement::next(); }
  Edge* next() { return (Edge*)AngleElement::next(); }

  HoughLine* memberOf() const { return _memberOf; }
  void memberOf(HoughLine* hl) { _memberOf = hl; }

  const Edge* nextMember() const { return _nextMember; }
  Edge* nextMember() { return _nextMember; }
  void nextMember(Edge* e) { _nextMember = e; }

  FieldEdge field() const { return _field; }

  // Translate edge to field coordinates
  void setField(const FieldHomography& h);
};

class EdgeList : public AngleBins<Edge>
{
  // No copy/assign
  EdgeList(const EdgeList&);
  EdgeList& operator=(const EdgeList&);

  double _fx0, _fy0;

public:
  EdgeList(int size) : AngleBins(size) {}

  Edge* add(int x, int y, int mag, int angle)
  {
    Edge* e = AngleBins::add(angle);
    e->set(x, y, mag);
    return e;
  }
  void mapToField(const FieldHomography&);


  // The field coordinates of the robot at the time mapToField was called.
  double fx0() const { return _fx0; }
  double fy0() const { return _fy0; }
};

// *********************************
// *                               *
// *  Gradient and Edge Detection  *
// *                               *
// *********************************

class EdgeDetector
{
  // Thresholds
  int _gradThr, _edgeThr;
  int _greenThr;

  // Gradient and edge image data
  ImageLiteU16 _gradImage;
  void* gradMem;
  uint16_t* gradPixels;
  int gradAllocated;

  uint32_t _gradTime, _edgeTime;

  bool _fast;
  bool _correctEdgeDir;

  int32_t* runs;  // run-length buffer for edge detection
  int runSize;

  // Fixed-point cartesian to polar, to match ASM version
  static int16_t cosTab[5];
  static int16_t sinTab[5];
  static void cToP(int x, int y, int& mag, int& dir);

public:
  // Construct & destroy
  EdgeDetector();

  ~EdgeDetector()
  {
      //delete[] gradMem;
    delete[] runs;
  }

  static void init();

  // Copy/assign just copies parameters
  EdgeDetector& operator= (const EdgeDetector& ed);
  EdgeDetector(const EdgeDetector& ed);

  // Gradients (mag and dir) <= this magnitude are set to 0
  int gradientThreshold() const { return _gradThr;}
  void gradientThreshold(int t) { _gradThr = t;}

  // Gradient magnitude must exceed this threshold for edges
  int edgeThreshold() const { return _edgeThr;}
  void edgeThreshold(int t) { _edgeThr = t;}
  
  // Green differential must be >= this threshold for edges when green
  // processing is being used. The green differential is the difference between
  // fuzzy green values on the darker side of the edge, and fuzzy green values on
  // the lighter side of the edge, in percent of max possible difference.
  // Note that the getter rounds off to make sure the same value is read as
  // was written.
  int greenThreshold() const { return (_greenThr * 100 + 50) / (3 * 255);}
  void greenThreshold(int t) { _greenThr = t * (3 * 255) / 100;}

  // Use fast (ASM) version
  bool fast() const { return _fast;}
  void fast(bool b) { _fast = b;}

  // Apply a small direction correction for edges based on emperical tests of
  // measured direction with synthetic lines
  bool correctEdgeDirection() const { return _correctEdgeDir; }
  void correctEdgeDirection(bool b) { _correctEdgeDir = b; }

  // Compute gradient of source image, 10-bit pixels (i.e. sum of four Y pixels) in
  // 16-bit words. Pitch is pixels.
  //
  // Produce a gradient image with 16-bit pixels, where the high byte of each pixel
  // is gradient magnitude and the low byte is gradient direction (binary angle).
  // All pixels where gradient magnitude is less than the gradientThreshold() are
  // set to 0 (all 16 bits).
  //
  // Pitch must be a multiple of 16 (bytes), which is enforced by ImageFrontEnd. There
  // are no other alignment requirements.
  uint32_t gradient(const ImageLiteU16&);

  // Execution time of last run in clocks
  uint32_t gradientTime() const { return _gradTime;}

  // Gradient image from last call to gradient. 
  const ImageLiteU16& gradientImage() const { return _gradImage; };

  // Get individual pixels from gradient image
  uint16_t gradPixel(int x, int y) const { return *gradientImage().pixelAddr(x, y);}
  uint8_t mag(int x, int y) const { return gradPixel(x, y) >> 8;}
  uint8_t dir(int x, int y) const { return gradPixel(x, y) & 0xFF;}

  // Edge detection
  uint32_t edgeDetect(const ImageLiteU8& green, EdgeList& edgeList);

  // Execution time of last run in clocks
  uint32_t edgeTime() const { return _edgeTime;}
};

// ************************
// *                      *
// *  Static Initializer  *
// *                      *
// ************************

class EdgeInit
{
public:
  EdgeInit()
  {
    AngleElement::init();
    EdgeDetector::init();
  }
};

}
}

#endif
