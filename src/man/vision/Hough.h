// ***************************************************
// *                                                 *
// *  Hough Transform and Field Line Detection 2015  *
// *                                                 *
// ***************************************************

#ifndef _hough_
#define _hough_

#include "Stdafx.h"
#include "Vision.h"
#include "Edge.h"
#include "Homography.h"

#include <list>
#include <vector>

// *******************************
// *                             *
// *  Line from Hough Transform  *
// *                             *
// *******************************
//
// Hough lines use centered image coordinates

// The adjustment algorithm considers all edges and for each computes a fuzzy logic value
// for the confidence that the edge belongs to the line. The fuzzy logic value is then
// used as a weight in a weighted linear regression that is used to update the line's
// parameters (R,T). Adjustment can be run multiple times with different parameters.
// Best practice is to run twice, with tighter parameters for the second tme.
struct AdjustParams
{
  FuzzyThr angleThr;            // radians
  FuzzyThr distanceThr;         // pixels
  FuzzyThr magnitudeThr;        // gray levels

  // To calculate endpoints, discard ths much wieght on both ends.
  float lineEndWeight;

  // Discard if this is >= 0 and RMS fit error above this
  double fitThresold;

  // Discard if adjusted score < this threshold
  double scoreThreshold;

  // Constructor is necessary to initialize fuzzy thresholds, which have no
  // default constructor.
  AdjustParams();
};

struct AdjustSet
{
  enum
  {
    MaxAdjustSteps = 2
  };
  AdjustParams params[MaxAdjustSteps];

  AdjustSet();
};

class HoughLine : public GeoLine
{
  int _rIndex;
  int _tIndex;
  double _score;
  double _fitError;

  GeoLine _field;

  Edge* members;

public:
  int rIndex() const { return _rIndex; }
  int tIndex() const { return _tIndex; }

  int binaryAngle() const
  {
    return (int)(t() * (128 / M_PI) + 0.5) & 0xFF;
  }

  double score() const { return _score; }
  double fitError() const { return _fitError; }

  const GeoLine& field() const { return _field; }

  // effect   Set field line
  void setField(const FieldHomography& h) { _field = *this; _field.imageToField(h); }

  // Which field line is this line a part of, or -1 if none.
  int fieldLineIndex;

  // effect   Set image line from specified data
  void set(int rIndex, int tIndex, double r, double t, double score);

  // Copy/assign OK

  bool adjust(EdgeList& edges, const AdjustParams& p, bool capture);

  // For sorting lines in decreasing order of score. 
  bool operator<(const HoughLine& hl) { return score() > hl.score(); }

  std::string print() const;
};

class HoughLineList : public std::list<HoughLine>
{
public:
  HoughLineList(int size) : list(size) {}

  void add(int rIndex, int tIndex, double score)
  {
    HoughLine hl;
    hl.set(rIndex, tIndex, rIndex + 0.5, tIndex * (M_PI / 128), score / 16.0);
    push_back(hl);
  }

  void mapToField(const FieldHomography&);
};

// *****************
// *               *
// *  Hough Space  *
// *               *
// *****************
//
// A cylindrical Hough space covering the range -rSpan/2 <= R < rSpan/2 pixels, with direction T covering
// a full 8-bit binary angle.
class HoughSpace
{
public:
   enum
  {
    NumTimes = 6,
  };

private:
 enum
  {
    TBits         = 8,
    TSpan         = 1 << TBits,
    TMask         = TSpan - 1,

    AngleSpread   = 3
  };

  int _rPitch;
  int _initRadius;
  int _rRadius;
  void* spaceBlock;        // memory block
  uint16_t* spaceMem;      // upper left corner of space
  uint16_t* _space00;      // (0,0)

  void rErode(int r) { _rRadius -= r; }

  uint16_t* space00() { return _space00; }
  uint16_t* space(int r, int t) { return space00() + (t & TMask) * rPitch() + r; }
  uint16_t* spaceUL() { return space00() - rRadius(); }
  int spaceSize() { return rPitch() * (TSpan + 2) * sizeof(uint16_t);}

  int _accThr;
  bool _fast;

  uint32_t times[NumTimes];

  int32_t* runs;

  int _adjustSteps;
  AdjustSet _adjustSet;

  void clear();
  void processEdges(EdgeList&);
  void wrapAround();
  void smooth();
  void peaks(HoughLineList&);
  void adjust(EdgeList&, HoughLineList&);

  static bool tableInit;
  static int16_t sincosTable[0x140 + 2 * AngleSpread + 1];

  // No copy/assign
  HoughSpace(const HoughSpace&);
  HoughSpace& operator=(const HoughSpace&);

public:
  int rRadius() const { return _rRadius; }
  int rPitch() const { return _rPitch; }
  int rWidth() const { return 2 * _rRadius; }
  const uint16_t* pSpace(int r, int t) const { return ((HoughSpace*)this)->space(r, t); }

  int acceptThreshold() const { return _accThr; }
  void acceptThreshold(int t) { _accThr = t; }

  int adjustSteps() const { return _adjustSteps; }
  void adjustSteps(int t) { _adjustSteps = min(t, _adjustSet.MaxAdjustSteps); }
  const AdjustSet& adjustSet() const { return _adjustSet; }
  void adjustSet(const AdjustSet& a) { _adjustSet = a; }

  bool fast() const { return _fast;}
  void fast(bool b) { _fast = b;}

  HoughSpace(int maxImageWd, int maxImageHt);
  ~HoughSpace();

  uint32_t time(int i) const { return times[i]; }
  static const char* timeNames[NumTimes];

  void run(EdgeList&, HoughLineList&);
};

#endif
