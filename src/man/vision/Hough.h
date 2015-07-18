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
#include "Field.h"

#include <list>
#include <vector>
#include <array>
#include <iostream>

namespace man {
namespace vision {

// *******************************
// *                             *
// *  Line from Hough Transform  *
// *                             *
// ******************************* // // Hough lines use centered image coordinates

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
  bool _onField;

  int _fieldLine;

  Edge* members;

  // Used for debug print outs
  int _index;

public:
  int rIndex() const { return _rIndex; }
  int tIndex() const { return _tIndex; }

  int binaryAngle() const
  {
    return (int)(t() * (128 / M_PI) + 0.5) & 0xFF;
  }

  double score() const { return _score; }
  double fitError() const { return _fitError; }

  // The line in field coordinates
  const GeoLine& field() const { return _field; }

  // effect   Map image line to field coordinates
  void setField(const FieldHomography& h) { _field = *this; _field.imageToField(h); }

  // China 2015 hack
  // We require that line is sufficiently below field horizon to be a hough line
  bool onField() { return _onField; }
  void onField(bool newOnField) { _onField = newOnField; }

  // Index of field line that this line a part of, or -1 if none.
  int fieldLine() const { return _fieldLine; }
  void fieldLine(int fl) { _fieldLine = fl; }

  // List of edges that are members of this line
  Edge* edgeMembers() const { return members; }

  // For debug
  int index() const { return _index; }

  // effect   Set image line from specified data
  void set(int rIndex, int tIndex, double r, double t, double score, int index);

  // Copy/assign OK

  bool adjust(EdgeList& edges, const AdjustParams& p, bool capture);

  // For sorting lines in decreasing order of score. 
  bool operator<(const HoughLine& hl) { return score() > hl.score(); }

  std::string print() const;
};

// The list<T> container is used primarily to be able to sort. vector<T>
// does not have a sort member. Secondarily, we use erase(), which is
// faster with lists but can be done with vectors. 
class HoughLineList : public std::list<HoughLine>
{
  double _fx0, _fy0;

public:
  HoughLineList(int size) : list(size) {}

  void add(int rIndex, int tIndex, double score)
  {
    HoughLine hl;
    hl.set(rIndex, tIndex, rIndex + 0.5, tIndex * (M_PI / 128), score / 16.0, size());
    push_back(hl);
  }

  // Map all lines on the list to field coordinates
  void mapToField(const FieldHomography&, Field&);

  // The field coordinates of the robot at the time mapToField was called.
  double fx0() const { return _fx0; }
  double fy0() const { return _fy0; }
};

// *****************
// *               *
// *  Field Lines  *
// *               *
// *****************

// Forward declerations
class FieldLine;
class FieldLineList;

enum class CornerID {
  Concave,
  Convex,
  T,

  // NOTE detector uses None, TFirst, and TSecond internally, the client will 
  //      never see such a corner ID
  None,
  TFirst,
  TSecond
};

// Corner object used in corner detection and field line classification.
// If id == T, then corner.first is the horizontal part of the T and corner.second
// is the vertical part of the T (assuming the T is written like the english letter).
struct Corner : public std::pair<FieldLine*, FieldLine*>
{
  Corner() : std::pair<FieldLine*, FieldLine*>() {}
  Corner(FieldLine* first_, FieldLine* second_, CornerID id_);

  double x;
  double y;

  CornerID id;

  std::string print() const;
};

// Detects goalbox.
// First is index of top goalbox line.
// Second is index of endline.
class GoalboxDetector : public std::pair<FieldLine*, FieldLine*>
{
  double parallelThreshold_;
  double seperationThreshold_;
  double lengthThreshold_;

  bool validBox(const HoughLine& line1, const HoughLine& line2) const;

public:
  GoalboxDetector();
  bool find(FieldLineList& list);

  double parallelThreshold() const { return parallelThreshold_; }
  void parallelThreshold(double newThreshold) { parallelThreshold_ = newThreshold; }

  double seperationThreshold() const { return seperationThreshold_; }
  void seperationThreshold(double newThreshold) { seperationThreshold_ = newThreshold; }

  double lengthThreshold() const { return lengthThreshold_; }
  void lengthThreshold(double newThreshold) { lengthThreshold_ = newThreshold; }

  std::string print() const;
};

// Detects corners
// Stores all detected corners in vector
class CornerDetector : public std::vector<Corner>
{
  int width;
  int height;
  double orthogonalThreshold_;
  double intersectThreshold_;
  double closeThreshold_;
  double farThreshold_;
  double edgeImageThreshold_;
  double lengthThreshold_;

  bool isCorner(const HoughLine& line1, const HoughLine& line2) const;
  CornerID classify(const HoughLine& line1, const HoughLine& line2) const; 
  bool isConcave(double end1X, double end1Y, 
                 double end2X, double end2Y, 
                 double intersectX, double intersectY) const;
  bool ccw(double ax, double ay, double bx, double by, double cx, double cy) const;

public:
  CornerDetector(int width_, int height_);
  void findCorners(FieldLineList& list);

  double orthogonalThreshold() const { return orthogonalThreshold_; }
  void orthogonalThreshold(double newThreshold) { orthogonalThreshold_ = newThreshold; }

  double intersectThreshold() const { return intersectThreshold_; }
  void intersectThreshold(double newThreshold) { intersectThreshold_ = newThreshold; }

  double closeThreshold() const { return closeThreshold_; }
  void closeThreshold(double newThreshold) { closeThreshold_ = newThreshold; }

  double farThreshold() const { return farThreshold_; }
  void farThreshold(double newThreshold) { farThreshold_ = newThreshold; }

  double edgeImageThreshold() const { return edgeImageThreshold_; }
  void edgeImageThreshold(double newThreshold) { edgeImageThreshold_ = newThreshold; }

  double lengthThreshold() const { return lengthThreshold_; }
  void lengthThreshold(double newThreshold) { lengthThreshold_ = newThreshold; }
};

// Dectects center circle
class CenterCircleDetector
{
  double _ccx;
  double _ccy;
  bool _on;

  // For debugging (retreived by nbfunc)
  std::vector<Point> _potentials;


  // Parameters
  int minPotentials;              // Min number of potential edges
  double maxEdgeDistanceSquared;  // Max considered distance of an edge
  double ccr;                     // Center circle radius
  double minVotesInMaxBin;        // Ratio of potentials required in the most populated bin
  double fieldTestDistance;       // Distance of projected points to check on-fieldness
  
  void set();
  bool findPotentialsAndCluster(EdgeList& edges, double& x0, double& y0);
  bool getMaxBin(const std::vector<Point>& vec, double& x0, double& y0);
  bool onField(Field& field);
  inline int roundDown(int v) { return binWidth*(v/binWidth); }

  enum ccconst
  {
    /*
      binCount MUST BE EVEN
      binCount*binWidth MUST EQUAL maxEdgeDistance
     */
    binCount = 20,    // Bins per row and col
    binWidth = 25,    // In centimeters
  };

public:
  CenterCircleDetector();
  bool detectCenterCircle(EdgeList& edges, Field& field);

  double x() { return _ccx; }
  double y() { return _ccy; }
  bool on() { return _on; }

  void on(bool on) { _on = on; }
  void adjustCC(double x, double y);

#ifdef OFFLINE
  std::vector<Point> getPotentials() { return _potentials; }
#endif
};

enum class LineID {
  // Most general
  Line,

  // Multiple possibilities
  EndlineOrSideline,
  EndlineSidelineTopGoalboxOrSideGoalbox,
  TopGoalboxOrSideGoalbox,
  SideGoalboxOrMidline,
  
  // More specific
  Sideline,
  SideGoalbox,

  // Most specific
  Endline,
  TopGoalbox,
  Midline
};

class FieldLine
{
  HoughLine* _lines[2];
  LineID id_;
  std::vector<Corner> corners_;
  int index_;
  bool repeat_;

public:
  // Copy/assign OK

  HoughLine& operator[](int index) { return *_lines[index]; }
  const HoughLine& operator[](int index) const  { return *_lines[index]; }

  FieldLine(HoughLine& line1, HoughLine& line2, int index = -1, double fx0 = 0, double fy0 = 0);

  LineID id() const { return id_; }
  void id(LineID newId) { id_ = newId; }

  // For debug
  int index() const { return index_; }

  bool repeat() const { return repeat_; }
  void repeat(bool newRepeat) { repeat_ = newRepeat; }

  void addCorner(Corner newCorner) { corners_.push_back(newCorner); }
  std::vector<Corner> corners() const { return corners_; }

  double separation() const { return _lines[0]->field().separation(_lines[1]->field()); }

  // Get length of the longer line
  double maxLength() const { return max(_lines[0]->field().length(), _lines[1]->field().length()); }

  std::string print() const;
};

// Either list or vector could be used here. Generally a field line list is not
// edited (insert/delete) after being created. Mostly just need a collection that
// grows as needed. I prefer vector because list iteration is awkward.
class FieldLineList : public std::vector<FieldLine>
{
  float _maxLineAngle;
  float _maxLineSeparation;
  float _maxCalAngle;

public:
  // Maximum angle between two Hough lines, in degrees as mapped to field coordinates,
  // for the pair to be considered a candidate field line
  float maxLineAngle() const { return _maxLineAngle; }
  void maxLineAngle(float t) { _maxLineAngle = t; }

  // Maximum separation between two Hough lines, in centimeters, for the pair to be
  // considered a candidate field line.
  float maxLineSeparation() const { return _maxLineSeparation; }
  void maxLineSeparation(float t) { _maxLineSeparation = t; }

  // Max deviation in degrees from parallel or perpendicular for calibration
  float maxCalibrateAngle() const { return _maxCalAngle; }
  void maxCalibrateAngle(float a) { _maxCalAngle = a; }

  // Construct with default parameters
  FieldLineList();

  // Find field lines
  void find(HoughLineList&, bool blackStar = false);

  // Classify field lines
  void classify(GoalboxDetector&, CornerDetector&, CenterCircleDetector&);

  // Calibrate tilt if possible.
  bool tiltCalibrate(FieldHomography&, std::string* message = 0);

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
  void adjust(EdgeList&, EdgeList&, HoughLineList&);

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

  void run(EdgeList&, EdgeList&, HoughLineList&);
};

}
}

#endif
