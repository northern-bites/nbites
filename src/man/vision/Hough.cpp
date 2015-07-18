// ***************************************************
// *                                                 *
// *  Hough Transform and Field Line Detection 2015  *
// *                                                 *
// ***************************************************

#include "Hough.h"
#include "FrontEnd.h"
#include "FieldConstants.h"
#include "NBMath.h"

#include <iostream>

using namespace std;

namespace man {
namespace vision {

// *******************************
// *                             *
// *  Line from Hough Transform  *
// *                             *
// *******************************

AdjustParams::AdjustParams()
  : angleThr(0.10f, 0.15f), distanceThr(2.00f, 4.00f), magnitudeThr(12, 20)
{
  lineEndWeight = 8.0f;
  fitThresold = -1;
  scoreThreshold = 32;
}

AdjustSet::AdjustSet() {
  params[1].angleThr = FuzzyThr(0.08f, 0.12f);
  params[1].distanceThr = FuzzyThr(0.7f, 2.0f);
  params[1].fitThresold = 0.55;

}

void HoughLine::set(int rIndex, int tIndex, double r, double t, double score, int index)
{
  GeoLine::set(r, t);
  _rIndex = rIndex;
  _tIndex = tIndex;
  _score = score;
  _fitError = -1;
  members = 0;
  fieldLine(-1);
  _index = index;
  _onField = true;
}

enum
{
  UHistSize = 801
};
  
static inline void uAdd(double u, float w, float* uHistogram)
{
  int i = min(max((int)floor(u + 0.5 * UHistSize), 0), UHistSize - 1);
  uHistogram[i] += w;
}

static inline double uBin(int index)
{
  return index - 0.5 * (UHistSize - 1);
}

bool HoughLine::adjust(EdgeList& edges, const AdjustParams& p, bool capture)
{
  float uHistogram[UHistSize];
  LineFit fit;

  if (capture)
    for (int i = 0; i < UHistSize; ++i)
      uHistogram[i] = 0;

  int lineAngle = binaryAngle();

  // The angle range to consider is derived from the angle fuzzy threshold
  int angleLimit = (int)ceil(p.angleThr.t1() * 128 / M_PI);

  members = 0;

  // Consider all edges at an angle similar to this line
  for (int bin = lineAngle - angleLimit; bin <= lineAngle + angleLimit; ++bin)
    for (Edge* e = edges.binList(bin); e; e = e->next())
      if (!e->memberOf())
      {
        float angle = (float)diffRadians(e->radians(), t());

        double x = e->x();
        double y = e->y();

        float distance = (float)fabs(pDist(x, y));

        float w = (angle < p.angleThr & distance < p.distanceThr & (float)e->mag() > p.magnitudeThr).f();
        if (w > 0)
        {
          if (capture)
          {
            double q = qDist(x, y);
            uAdd(q, w, uHistogram);
          }
          fit.add(w, x, y);
        }

        if (capture && w > 0.5)
        {
          e->memberOf(this);
          e->nextMember(members);
          members = e;
        }
      }

  _score = fit.area();
  if (score() < p.scoreThreshold || (p.fitThresold >= 0 && fit.rmsError() > p.fitThresold))
  {
    for (Edge* e = members; e; e = e->nextMember())
      e->memberOf(0);
    return false;
  }

  _fitError = fit.rmsError();

  if (capture)
  {
    // Find first endpoint
    float tw = 0;
    double u0 = 0, u1 = 0;
    for (int i = 0; i < UHistSize; ++i)
    {
      tw += uHistogram[i];
      if (tw >= p.lineEndWeight)
      {
        u0 = uBin(i) - p.lineEndWeight;
        break;
      }
    }

    // Find second endpoint
    tw = 0;
    for (int i = UHistSize - 1; i >= 0; --i)
    {
      tw += uHistogram[i];
      if (tw >= p.lineEndWeight)
      {
        u1 = uBin(i) + p.lineEndWeight;
        break;
      }
    }
    setEndPoints(u0, u1);
  }

  // Adjust (R,T)
  r(fit.centerX() * fit.secondPrinciaplAxisU() + fit.centerY() * fit.secondPrinciaplAxisV());
  if (ux() * fit.secondPrinciaplAxisU() + uy() * fit.secondPrinciaplAxisV() >= 0)
    setUnitVec(fit.secondPrinciaplAxisU(), fit.secondPrinciaplAxisV());
  else
  {
    setUnitVec(-fit.secondPrinciaplAxisU(), -fit.secondPrinciaplAxisV());
    r(-r());
  }

  return true;
}

// bool HoughLine::equals(const HoughLine& hl) {
//   return (rIndex() == hl.rIndex() && tIndex() == hl.tIndex());
// }

string HoughLine::print() const
{
  return strPrintf("%4d, %02X  %4.0f %4.2f %s -> %s",
                   rIndex(), tIndex(), score(), fitError(),
                   GeoLine::print(true).c_str(), field().print(true).c_str());
}

// *********************
// *                   *
// *  Hough Line List  *
// *                   *
// *********************

void HoughLineList::mapToField(const FieldHomography& h, Field& f)
{
  for (list<HoughLine>::iterator hl = begin(); hl != end(); ++hl)
    hl->setField(h);

  _fx0 = -h.wx0();
  _fy0 = -h.wy0();
}

// **********************************
// *                                *
// *  Goalbox and Corner Detection  *
// *                                *
// **********************************

Corner::Corner(FieldLine* first_, FieldLine* second_, CornerID id_)
  : std::pair<FieldLine*, FieldLine*>(first_, second_), id(id_)
{
  const GeoLine& field1 = (*first)[0].field();
  const GeoLine& field2 = (*second)[0].field();

  field1.intersect(field2, x, y);
}

string Corner::print() const
{
  return strPrintf("C, %4d, %4d, %4d", first->index(), second->index(), (int) id);
}

GoalboxDetector::GoalboxDetector()
  : std::pair<FieldLine*, FieldLine*>(NULL, NULL), 
    parallelThreshold_(15), seperationThreshold_(20), lengthThreshold_(0)
{}

bool GoalboxDetector::find(FieldLineList& list)
{
  // Reset detector
  first = NULL;
  second = NULL;

  // Loop thru field lines
  for (int i = 0; i < list.size(); i++) {
    for (int j = 0; j < list.size(); j++) {
      // Consider each pair once
      if (i <= j) continue;

      // Get pair of lines
      FieldLine& line1 = list[i];
      FieldLine& line2 = list[j];

      // Find goalbox
      // NOTE since there are two hough lines in each field line, we require
      //      finding the goal box in all pairings of hough lines
      bool foundGoalbox = true;
      for (int k = 0; k < 2; k++) {
        for (int l = 0; l < 2; l++) {
          if (!validBox(line1[k], line2[l]))
            foundGoalbox = false;
        }
      }

      // Classify lines if box was detected
      if (foundGoalbox) {
        if (fabs(line1[0].field().r()) < fabs(line2[0].field().r())) {
          first = &line1;
          second = &line2;
        } else {
          first = &line2;
          second = &line1;
        }
        first->id(LineID::TopGoalbox);
        second->id(LineID::Endline);
        return true;
      }
    }
  }
  return false;
}

bool GoalboxDetector::validBox(const HoughLine& line1, const HoughLine& line2) const
{
  // Use world coordinates
  const GeoLine& field1 = line1.field();
  const GeoLine& field2 = line2.field();

  // Goalbox = two field lines that are parallel, seperated according to spec,
  // and sufficiently long (to rule out center circle false positives)

  // (1) Parallel
  // NOTE this check also requires that the robot is not in between the lines 
  //      in order to simplify the classification problem
  double normalizedT1 = (field1.r() > 0 ? field1.t() : field1.t() - M_PI);
  double normalizedT2 = (field2.r() > 0 ? field2.t() : field2.t() - M_PI);
  bool parallel = diffRadians(normalizedT1, normalizedT2) < parallelThreshold()*TO_RAD;

  // (2) Seperated by 60 cm
  double distBetween = fabs(field1.pDist(field2.r()*cos(field2.t()), field2.r()*sin(field2.t())));
  bool seperation = fabs(distBetween - GOALBOX_DEPTH) < seperationThreshold();

  // (3) Both lines are sufficiently long
  bool line1Length = (field1.ep1() - field1.ep0()) > lengthThreshold();
  bool line2Length = (field2.ep1() - field2.ep0()) > lengthThreshold();
  bool length = line1Length && line2Length;

  return parallel && seperation && length;
}

string GoalboxDetector::print() const
{
  return strPrintf("B, %4d, %4d", first->index(), second->index());
}

CornerDetector::CornerDetector(int width_, int height_)
  : width(width_), 
    height(height_), 
    orthogonalThreshold_(40), 
    intersectThreshold_(15), 
    closeThreshold_(15),
    farThreshold_(15), 
    edgeImageThreshold_(0.1),
    lengthThreshold_(0)  // NOTE zero means that the condition is not currently being used
{}

void CornerDetector::findCorners(FieldLineList& list)
{
  // Reset detector
  this->clear();

  // Loop thru field lines
  for (int i = 0; i < list.size(); i++) {
    for (int j = 0; j < list.size(); j++) {
      // Consider each pair once
      if (i <= j) continue;

      // Get pair of lines
      FieldLine& line1 = list[i];
      FieldLine& line2 = list[j];

      // Find corners
      // NOTE since there are two hough lines in each field line, we require
      //      finding valid corners with same id in at least two pairings 
      //      of hough lines
      std::vector<CornerID> ids;
      if (isCorner(line1[0], line2[0]))
          ids.push_back(classify(line1[0], line2[0]));
      if (isCorner(line1[0], line2[1]))
          ids.push_back(classify(line1[0], line2[1]));
      if (isCorner(line1[1], line2[0]))
          ids.push_back(classify(line1[1], line2[0]));
      if (isCorner(line1[1], line2[1]))
          ids.push_back(classify(line1[1], line2[1]));

      CornerID id = CornerID::None;
      for (int i = 0; i < ids.size(); i++) {
          for (int j = 0; j < ids.size(); j++) {
              if (i == j) continue;
              if (ids[i] != CornerID::None && ids[i] == ids[j])
                  id = ids[i];
          }
      }

      // NOTE TFirst and TSecond are temporarily used to specify whether corner.first
      //      or corner.second is the top of the T corner. Before the corner detector finishes 
      //      finding corners, all references to TFirst and TSecond are replaced 
      //      with T and the corner.first is the vertical part of the T. This happens 
      //      below. Thus the client is not aware of TFirst and TSecond.

      // Create corner object and add to field lines
      if (id != CornerID::None) {
        Corner newCorner;
        if (id == CornerID::TSecond)
          newCorner = Corner(&line2, &line1, CornerID::T);
        else if (id == CornerID::TFirst)
          newCorner = Corner(&line1, &line2, CornerID::T);
        else
          newCorner = Corner(&line1, &line2, id);
        line1.addCorner(newCorner);
        line2.addCorner(newCorner);
        this->push_back(newCorner);
      }
    }
  }
}

bool CornerDetector::isCorner(const HoughLine& line1, const HoughLine& line2) const
{
  // (1) Check that lines intersect
  // NOTE done in image coords
  double imageIntersectX;
  double imageIntersectY;

  bool intersects = line1.intersect(line2, imageIntersectX, imageIntersectY);
  if (intersects) {
    double qIntersect1 = line1.qDist(imageIntersectX, imageIntersectY);
    double qIntersect2 = line2.qDist(imageIntersectX, imageIntersectY);
    intersects = (qIntersect1 >= line1.ep0() - intersectThreshold() && 
                  qIntersect1 <= line1.ep1() + intersectThreshold() && 
                  qIntersect2 >= line2.ep0() - intersectThreshold() && 
                  qIntersect2 <= line2.ep1() + intersectThreshold());
  }

  // (2) If intersection is in edge of image, don't classify corner
  double xThreshold = (width / 2) - (width * edgeImageThreshold());
  double yThresholdBottom = (height / 2) - (height * edgeImageThreshold());
  double yThresholdTop = (height / 2) - (height * 0.5*edgeImageThreshold());
  // NOTE in top of image, no classify buffer is half the size

  bool farEnoughFromImageEdge = (imageIntersectX >= -xThreshold &&
                                 imageIntersectX <=  xThreshold &&
                                 imageIntersectY >= -yThresholdBottom &&
                                 imageIntersectY <=  yThresholdTop);

  // (3) Check that lines are close to orthogonal
  // NOTE done in world coords
  const GeoLine& field1 = line1.field();
  const GeoLine& field2 = line2.field();

  double normalizedT1 = (field1.r() > 0 ? field1.t() : field1.t() - M_PI);
  double normalizedT2 = (field2.r() > 0 ? field2.t() : field2.t() - M_PI);
  bool orthogonal = diffRadians(diffRadians(normalizedT1, normalizedT2), (M_PI / 2)) < orthogonalThreshold()*TO_RAD;

  // (4) Both lines are sufficiently long
  bool line1Length = (field1.ep1() - field1.ep0()) > lengthThreshold();
  bool line2Length = (field2.ep1() - field2.ep0()) > lengthThreshold();
  bool length = line1Length && line2Length;

  return intersects && farEnoughFromImageEdge && orthogonal && length;
}

CornerID CornerDetector::classify(const HoughLine& line1, const HoughLine& line2) const
{
  // Use world coordinates
  const GeoLine& field1 = line1.field();
  const GeoLine& field2 = line2.field();

  // Find intersection point
  double worldIntersectX;
  double worldIntersectY;

  bool intersects = field1.intersect(field2, worldIntersectX, worldIntersectY);
  if (!intersects) return CornerID::None; 
  // NOTE should never happen since checked above in image coords

  // (1) Check for T corner
  // Project intersection point onto lines
  double whereOnField1 = field1.qDist(worldIntersectX, worldIntersectY);
  double whereOnField2 = field2.qDist(worldIntersectX, worldIntersectY);

  // If intersection point is between line's endpoints, found T
  if (whereOnField1 > field1.ep0() + farThreshold() && 
      whereOnField1 < field1.ep1() - farThreshold())
    return CornerID::TFirst;
  else if (whereOnField2 > field2.ep0() + farThreshold() && 
           whereOnField2 < field2.ep1() - farThreshold())
    return CornerID::TSecond;

  // (2) Check for convave or convex corner
  // Find endpoints
  double field1EndX[2];
  double field1EndY[2];

  double field2EndX[2];
  double field2EndY[2];

  field1.endPoints(field1EndX[0], field1EndY[0], field1EndX[1], field1EndY[1]);
  field2.endPoints(field2EndX[0], field2EndY[0], field2EndX[1], field2EndY[1]);

  // Calculate distance
  double dist1[2];
  for (int i = 0; i < 2; i++)
    dist1[i] = dist(field1EndX[i], field1EndY[i], worldIntersectX, worldIntersectY);

  double dist2[2];
  for (int i = 0; i < 2; i++)
    dist2[i] = dist(field2EndX[i], field2EndY[i], worldIntersectX, worldIntersectY);

  // Find and classify concave and convex corners
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      // Concave or convex corners have both endpoints on the intersection point
      if (dist1[i] < closeThreshold() && dist2[j] < closeThreshold()) {
        // Found concave or convex
        if (isConcave(field1EndX[!i], field1EndY[!i], field2EndX[!j], 
                      field2EndY[!j], worldIntersectX, worldIntersectY))
          return CornerID::Concave;
        return CornerID::Convex;
      }
    }
  }

  // Return none if no classification found
  // NOTE if closeThreshold == farThreshold, classification will always be made
  return CornerID::None; 
}

bool CornerDetector::isConcave(double end1X, double end1Y, 
                               double end2X, double end2Y,
                               double intersectX, double intersectY) const
{
  // Parallel lines -> convex corner
  if (intersectX == 0 && (end1X - end2X) == 0)
    return false;
  // Check for divide by zero
  else if (intersectX != 0 && (end1X - end2X) != 0) {
    double m1 = (end1Y - end2Y) / (end1X - end2X);
    double m2 = intersectY / intersectX;
    // Parallel lines -> convex corner
    if (fabs(m1 - m2) < 0.001)
      return false;
  }

  // General case
  return (ccw(end1X, end1Y, intersectX, intersectY, 0, 0) !=
          ccw(end2X, end2Y, intersectX, intersectY, 0, 0) &&
          ccw(end1X, end1Y, end2X, end2Y, intersectX, intersectY) !=
          ccw(end1X, end1Y, end2X, end2Y, 0, 0));
}

bool CornerDetector::ccw(double ax, double ay, 
                         double bx, double by, 
                         double cx, double cy) const
{
  return (cy-ay)*(bx-ax) > (by-ay)*(cx-ax);
}

// *******************
// *                 *
// *  Center Circle  *
// *                 *
// *******************
CenterCircleDetector::CenterCircleDetector() 
{
  _on = false;
  set();
}

void CenterCircleDetector::set()
{
  // Set parameters
  minPotentials = 850;
  maxEdgeDistanceSquared = 500 * 500;       // Good practicle distance = 5m
  ccr = CENTER_CIRCLE_RADIUS;               // 75 cm
  minVotesInMaxBin = 0.23;                  // Conservative clustering theshold
  fieldTestDistance = 200;

}

bool CenterCircleDetector::detectCenterCircle(EdgeList& edges, Field& field)
{
  on(findPotentialsAndCluster(edges, _ccx, _ccy));  // Excluding onField test. Needs debugging.
  return (on());
}

// Get potential cc centers and clean edge list
bool CenterCircleDetector::findPotentialsAndCluster(EdgeList& edges, double& x0, double& y0)
{
#ifdef OFFLINE
  _potentials.clear();
  std::cout << "POTENTIAL SIZE: " << _potentials.size() << std::endl;
#endif
  std::vector<Point> vec;
  Point p1, p2;
  int count = 0;
  int xOffset = binCount * binWidth / 2;

  int bcSq = binCount * binCount;
  
  // Fill set of little bins
  int bins[bcSq]; 
  
  std::fill(bins, bins + bcSq, 0);

  AngleBinsIterator<Edge> abi(edges);
  for (Edge* e = *abi; e; e = *++abi) {
    double distance = e->field().x() * e->field().x() + e->field().y() * e->field().y();
    if (e->field().y() >= 0 && distance < maxEdgeDistanceSquared) {
      count += 2;
      p1 = Point(e->field().x() + ccr*sin(e->field().t()), e->field().y() - ccr*cos(e->field().t()));
      p2 = Point(e->field().x() - ccr*sin(e->field().t()), e->field().y() + ccr*cos(e->field().t()));

#ifdef OFFLINE
      _potentials.push_back(p1);
      _potentials.push_back(p2);
#endif
    
      // Add first potential point to one bin if it is within the grid
      int xbin = roundDown((int)p1.first + xOffset) / binWidth;
      int ybin = roundDown((int)p1.second) / binWidth;
      if (xbin < binCount - 1 && ybin < binCount - 1)
        bins[xbin + binCount * ybin] += 1;

      // Add second potential point to one bin if within grid
      xbin = roundDown((int)p2.first + xOffset) / binWidth;
      ybin = ((int)p2.second) / binWidth;
      if (xbin < binCount - 1 && ybin < binCount - 1)
        bins[xbin + binCount * ybin] += 1;

    }
  }

  if (count < minPotentials) {
#ifdef OFFLINE
    std::cerr << std::endl << "Not enough potentials for center circle: " << (double)count << " potentials" << std::endl;
#endif   
    return false;
  }

  // Tally bins
  int winBin, votes = 0;

  for (int i = 0; i < binCount - 1; i++) {
    for (int j = 0; j < binCount - 1; j++) {
      int neighboorTally = bins[i +  j   *binCount] + bins[i+1 +  j   *binCount] +
                           bins[i + (j+1)*binCount] + bins[i+1 + (j+1)*binCount];

      // Look at neighbooring bins for overlapping effect
      if (neighboorTally > votes) {
        votes = neighboorTally;
        winBin = i + j*binCount;
      }
    }
  }

  if (votes > minVotesInMaxBin * count) {
    x0 = (winBin % binCount + 1) * binWidth - xOffset;
    y0 = (winBin / binCount + 1) * binWidth; 
    
#ifdef OFFLINE
    _potentials.push_back(Point(x0, y0));
    std::cerr << std::endl << "Center Circle at (" << x0 << "," << y0 << "). " << 
      (double)votes * 100/count << "\% of the " << count << " potentials in most populated bin" << std::endl;
#endif
    return true;
  } else {
#ifdef OFFLINE
    _potentials.push_back(Point(0.0, 0.0));
#endif
  return false;
  }

}

// TODO: debug and enable
// Project 2 points from CC and check if they are on the field
bool CenterCircleDetector::onField(Field& field)
{
  double y;
  if (field.onField(_ccx + fieldTestDistance, y) && _ccy + fieldTestDistance < y &&
          field.onField(_ccx - fieldTestDistance, y) && _ccy + fieldTestDistance < y) {
    return true;
  } else {
#ifdef OFFLINE
    std::cerr << "FAILING ON FIELD TEST\n";
#endif
    return false; 
  }
 
}

void CenterCircleDetector::adjustCC(double x, double y)
{
  _ccx += x;
  _ccy += y;
  _potentials.push_back(Point(_ccx, _ccy));
}


// **************************
// *                        *
// *  Field Lines and List  *
// *                        *
// **************************

FieldLine::FieldLine(HoughLine& line1, HoughLine& line2, int index, double fx0, double fy0)
  : id_(LineID::Line), corners_(), index_(index), repeat_(false)
{
  double d1 = fabs(line1.field().pDist(fx0, fy0));
  double d2 = fabs(line2.field().pDist(fx0, fy0));
  int i = (int)(d2 < d1);
  _lines[i    ] = &line1;
  _lines[i ^ 1] = &line2;
}

std::string FieldLine::print() const
{
  return strPrintf("%4d, %4d, %4d, %4d",
                   (*this)[0].index(), (*this)[1].index(), id(), index());
}

FieldLineList::FieldLineList()
{
  maxLineAngle(5.0f);
  maxLineSeparation(30.0f);
  maxCalibrateAngle(5.0f);
}

void FieldLineList::find(HoughLineList& houghLines, bool blackStar)
{
  // Check max angle by dot product of unit vectors. Since lines must have opposite
  // polarity, dot product must be below a negative threshold.
  double maxCosAngle = -cos(maxLineAngle() * (M_PI / 180));

  clear();

  for (HoughLineList::iterator hl1 = houghLines.begin(); hl1 != houghLines.end(); ++hl1)
    if (hl1->field().valid() && hl1->onField())
    {
      HoughLineList::iterator hl2 = hl1;
      for (++hl2; hl2 != houghLines.end(); ++hl2)
        // Here is the dot product 
        if (hl2->field().valid() && hl2->onField() &&
            hl1->field().ux() * hl2->field().ux() + hl1->field().uy() * hl2->field().uy() <= maxCosAngle)
        {
          // We use image coordinates to check polarity. Converting to field 
          // coordinates leads to crossed field lines if the homography is poor.
          // Crosses field lines in world coordinates leads to polarity error.
          bool correctPolarity = (hl1->r() + hl2->r() < 0);

          // If we are looking for lines in the black clibration star, check for
          // opposite polarity.
          if (blackStar)
            correctPolarity = !correctPolarity;

          // Separation is sum of the two r values (distance of line to origin).
          // This is well defined and sensible for lines that may not be perfectly
          // parallel. For field lines the polarities are pointing towards each
          // other, which makes the sum of r's negative. A pair of nearly parallel
          // lines with the right separation but with polarities pointing away from
          // each other is not a field line. 
          double separation = fabs(hl1->field().separation(hl2->field()));
          if (correctPolarity && separation <= maxLineSeparation())
          {
            int index = size();
            hl1->fieldLine(index);
            hl2->fieldLine(index);
            push_back(FieldLine(*hl1, *hl2, index, houghLines.fx0(), houghLines.fy0()));
          }
        }
    }
}

void FieldLineList::classify(GoalboxDetector& boxDetector,
                             CornerDetector& cornerDetector,
                             CenterCircleDetector& circleDetector)
{
  bool sideboxFound = false;
  bool midlineFound = false;
  bool sidelineFound = false;

  // If there are no field lines, no classification possible
  if (size() < 1) {
    circleDetector.on(false);
    return;
  }

  // Run goalbox detector
  bool topBoxFound = boxDetector.find(*this);
  bool endlineFound = topBoxFound;

  // Run corner detector
  cornerDetector.findCorners(*this);
  
  // Look for field line close to the center circle
  if (circleDetector.on()) {
    double minDist = 1000;
    FieldLine* midline;
    HoughLine* midHoughInner;

    for (int i = 0; i < size(); ++i) {
      FieldLine& fl = (*this)[i];
      HoughLine& inner = fl[0];
      if (inner.field().r() < 0) {
        inner = fl[1];
      }

      // Project CC center onto line, find distance to line
      double distToLine = inner.field().pDist(circleDetector.x(), circleDetector.y());
      double qD = inner.field().qDist(circleDetector.x(), circleDetector.y());
      bool within = qD > inner.field().ep0() && qD < inner.field().ep1();

      // Check for min distance
      if (within && minDist > fabs(distToLine)) {
          midline = &fl;
          midHoughInner = &inner;
          minDist = distToLine;
      }
    }

    // Set midline and adjust CC, or discard CC if no close line
    if (fabs(minDist) < 40) {
      midline->id(LineID::Midline);
      midlineFound = true;
      circleDetector.adjustCC(-minDist * cos(midHoughInner->field().t()),
                              -minDist * sin(midHoughInner->field().t()));

      // Erase all short field lines near center cirlce
      int startSize = size();
      int i = 0;
      while (i < startSize) {
        FieldLine* fl = &(*this)[i];

        // Loop through lines and it is likely to be a center circle false line, erase it
        if (fl->id() != LineID::Midline && fl->maxLength() < 75 &&
            fabs((*fl)[0].field().pDist(circleDetector.x(), circleDetector.y())) < 100) {
          this->erase(this->begin() + i);
#ifdef OFFLINE
          std::cout << "Discarding fieldline supposedly on center circle." << std::endl;
#endif
        }
        i++;
      }
    } else {
      circleDetector.on(false);
    }
  }

  // If there is only one line, stop here 
  if (size() < 2)
    return;

  // China 2015 hack
  //
  // The code belows attempts to make all possible line classifications from
  // already classified lines. This is a useful bit of functionality, but significant
  // testing is needed before we trust such a system, probably in the form of a 
  // unit test. As we are about to go play robot soccer in China, such a test is
  // not gonna happen.
  //
  // So the current strategy is to just classify lines via the goalbox detector,
  // the circle detector, and the corner loop below the commented out code. This
  // is imo sufficient for good localization accuracy, but in the future it
  // would be better to get the code commented out below tested and running.
  //
  // - Josh

  // Loop over lines until no more classifications possible
  // int i = 0;
  // int numStepsWithoutClassify = -1;
  // int numLines = static_cast<int>(size());
  // while (numStepsWithoutClassify < numLines) {
  //   numStepsWithoutClassify++;
  //   FieldLine& line = (*this)[i];
  //   i = (i + 1) % numLines;
  //   if (line.id() != LineID::Line) continue;
  //   std::vector<Corner> corners = line.corners();

  //   bool oneConcave = false;
  //   bool oneConvex = false;
  //   bool oneTHorizontal = false;
  //   bool oneTVertical = false;

  //   for (int j = 0; j < corners.size(); j++) {
  //     // (1) Classifies lines based on seeing two corners connected to line
  //     // Two concave -> endline
  //     if (corners[j].id == CornerID::Concave) {
  //       if (oneConcave) {
  //         line.id(LineID::Endline);
  //         numStepsWithoutClassify = 0;
  //         endlineFound = true;
  //       } else
  //         oneConcave = true;
  //     // Two convex -> top goalbox
  //     } else if (corners[j].id == CornerID::Convex) {
  //       if (oneConvex) {
  //         line.id(LineID::TopGoalbox);
  //         numStepsWithoutClassify = 0;
  //         topBoxFound = true;
  //       } else {
  //         oneConvex = true;
  //         // One convex, one T (vertical) -> side goalbox
  //         if (oneTVertical) {
  //           line.id(LineID::SideGoalbox);
  //           numStepsWithoutClassify = 0;
  //           sideboxFound = true;
  //         }
  //       }
  //     // Two t (horizontal) -> endline
  //     } else if (corners[j].id == CornerID::T &&
  //         corners[j].first == &line) {
  //       if (oneTHorizontal) {
  //         line.id(LineID::Endline);
  //         numStepsWithoutClassify = 0;
  //         endlineFound = true;
  //       } else
  //         oneTHorizontal = true;
  //     // Two t (vertical) -> midline
  //     } else if (corners[j].id == CornerID::T &&
  //         corners[j].second == &line) {
  //       if (oneTVertical) {
  //         line.id(LineID::Midline);
  //         numStepsWithoutClassify = 0;
  //         midlineFound = true;
  //       } else {
  //         oneTVertical = true;
  //         // One convex, one T (vertical) -> side goalbox
  //         if (oneConvex) {
  //           line.id(LineID::SideGoalbox);
  //           numStepsWithoutClassify = 0;
  //           sideboxFound = true;
  //         }
  //       }
  //     }

  //     // (2) Classifies line based on looking for connections to already classified lines
  //     if (topBoxFound) {
  //       // Top box found and convex corner -> side goalbox
  //       if (corners[j].id == CornerID::Convex) {
  //         line.id(LineID::SideGoalbox); 
  //         numStepsWithoutClassify = 0;
  //         sideboxFound = true;
  //       }
  //     } 
  //     if (endlineFound) {
  //       // Endline found and concave corner -> sideline
  //       if (corners[j].id == CornerID::Concave) {
  //         line.id(LineID::Sideline); 
  //         numStepsWithoutClassify = 0;
  //         sidelineFound = true;
  //       // Endline found, T, and corner.first is endline -> side goalbox
  //       } else if (corners[j].id == CornerID::T) {
  //         if (corners[j].first->id() == LineID::Endline) {
  //           line.id(LineID::SideGoalbox); 
  //           numStepsWithoutClassify = 0;
  //           sideboxFound = true;
  //         }
  //       }
  //     } 
  //     if (midlineFound) {
  //       // Midline found, T, and corner.second is midline -> sideline
  //       if (corners[j].id == CornerID::T) {
  //         if (corners[j].second->id() == LineID::Midline) {
  //           line.id(LineID::Sideline); 
  //           numStepsWithoutClassify = 0;
  //           sidelineFound = true;
  //         }
  //       }
  //     }
  //     // TODO
  //     if (sidelineFound) {
  //       ;
  //     }
  //     // TODO
  //     if (sideboxFound) {
  //       ;
  //     }
  //   }
  // }

  // Classify less specifically
  for (int i = 0; i < size(); i++) {
    FieldLine& line = (*this)[i];
    if (line.id() != LineID::Line) continue;
    std::vector<Corner> corners = line.corners();

    for (int j = 0; j < corners.size(); j++) {
      // Only two lines connected to convex corner
      if (corners[j].id == CornerID::Convex)
        line.id(LineID::TopGoalboxOrSideGoalbox);
      // Only two lines connected to concave corner
      else if (corners[j].id == CornerID::Concave)
        line.id(LineID::EndlineSidelineTopGoalboxOrSideGoalbox);
      // Only four lines connected to T corner
      else if (corners[j].id == CornerID::T) {
        if (corners[j].first == &line)
          line.id(LineID::EndlineOrSideline);
        else
          line.id(LineID::SideGoalboxOrMidline);
      }
    }
  }
}


// *****************
// *               *
// *  Calibration  *
// *               *
// *****************

bool FieldLineList::tiltCalibrate(FieldHomography& h, string* diagnostics)
{
  string s = "\n";
  LineFit tiltStats;

  for (int i = 0; i < (int)size() - 1; ++i)
    for (int j = i + 1; j < (int)size(); ++j)
    {
      const FieldLine& fl1 = at(i);
      const FieldLine& fl2 = at(j);
      if (fabs(fl1[0].field().ux() * fl2[0].field().ux() + fl1[0].field().uy() * fl2[0].field().uy()) < 0.33)
      {
        s += strPrintf("Perpendicular field lines %d and %d:\n", i, j);
        for (int a = 0; a < 2; ++a)
          for (int b = 0; b < 2; ++b)
          {
            double t;
            string diag;
            bool ok = h.visualTiltPerpendicular(fl1[a], fl2[b], t, &diag);
            if (ok)
            {
              t *= 180 / M_PI;
              tiltStats.add(t, 0);
              s += strPrintf("  %4.1f,", t);
            }
            else
              s += "  ----,";
            s += strPrintf(" | %s\n", diag.c_str());
          }
      }
      else if (fabs(fl1[0].field().ux() * fl2[0].field().uy() - fl1[0].field().uy() * fl2[0].field().ux()) < 0.33)
      {
        s += strPrintf("Parallel field lines %d and %d:\n", i, j);
        for (int a = 0; a < 2; ++a)
          for (int b = 0; b < 2; ++b)
          {
            double t;
            string diag;
            bool ok = h.visualTiltParallel(fl1[a], fl2[b], t, &diag);
            if (ok)
            {
              t *= 180 / M_PI;
              tiltStats.add(t, 0);
              s += strPrintf("  %4.1f,", t);
            }
            else
              s += "  ----,";
            s += strPrintf(" | %s\n", diag.c_str());
          }
      }
    }

  s += strPrintf("Mean tilt = %.2f, stDev = %.2f\n",
                 tiltStats.centerX(), tiltStats.firstPrincipalLength() / sqrt(3.0));
  if (diagnostics != 0)
    *diagnostics = s;
  return tiltStats.area() >= 3;
}

// *****************
// *               *
// *  Hough Space  *
// *               *
// *****************

int16_t HoughSpace::sincosTable[sizeof(sincosTable) / sizeof(int16_t)];
bool HoughSpace::tableInit = false;
const char* HoughSpace::timeNames[NumTimes] = { "Clear", "Edges", "Smooth", "Peaks", "Adjust", "Run"};

HoughSpace::HoughSpace(int maxImageWd, int maxImageHt)
{
  if (!tableInit)
  {
    for (int i = 0; i < sizeof(sincosTable) / sizeof(int16_t); ++i)
    {
      double a = (i - AngleSpread - 0.5) * (M_PI / 128);
      sincosTable[i] = (int16_t)min((int)floor(ldexp(sin(a), 15) + 0.5), 0x7FFF);
    }

    tableInit = true;
  }

  double x = maxImageWd - 4;
  double y = maxImageHt - 4;
  _rPitch = ((int)ceil(sqrt(x * x + y * y)) + 15) & ~15;
  _initRadius = _rRadius = max(maxImageWd, maxImageHt) / 2;
  spaceMem = (uint16_t*)alignedAlloc(spaceSize(), 4, spaceBlock);
  _space00 = spaceMem + rPitch() + rPitch() / 2;

  runs = new int32_t[((rWidth() - 4 + 15) & ~15) + 1];

  acceptThreshold(24);
  fast(true);
  adjustSteps(AdjustSet::MaxAdjustSteps);
}

HoughSpace::~HoughSpace()
{
  delete[] spaceBlock;
  delete[] runs;
}

void HoughSpace::clear()
{
  TickTimer timer;

  _rRadius = _initRadius;

  uint16_t* row = spaceUL();
  for (int t = 0; t < TSpan; ++t, row += rPitch())
    for (int r = 0; r < rRadius(); ++r)
      ((uint32_t*)row)[r] = 0;

  times[0] = timer.time32();
}

void HoughSpace::processEdges(EdgeList& edges)
{
  TickTimer timer;

  AngleBinsIterator<Edge> abi(edges);
  for (const Edge* e = *abi; e; e = *++abi)
  {
    int x = e->x();
    int y = e->y();
    int16_t* psin = sincosTable + e->angle();
    int r[2 * (AngleSpread + 1)];
    for (int a = 0; a < 2 * (AngleSpread + 1); ++a, ++psin)
      r[a] = (x * psin[0x40] + y * psin[0]) >> 15;

    int t = e->angle() - AngleSpread;
    for (int a = 0; a < 2 * AngleSpread + 1; ++a, ++t)
    {
      uint16_t* ph = space(r[a], t);
      ++*ph;
      int n = r[a + 1] - r[a];
      while (n > 0)
        ++ph[n--];
      while (n < 0)
        ++ph[n++];
    }
  }

  times[1] = timer.time32();
}

void HoughSpace::wrapAround()
{
  uint16_t* top = spaceUL();
  uint16_t* bot = top + (TSpan - 1) * rPitch();
  for (int r = 0; r < rWidth(); ++r)
  {
    top[r - rPitch()] = bot[r];
    bot[r + rPitch()] = top[r];
  }
}

extern "C" void _houghSmooth(uint16_t* image, int dstWd, int dstHt, int pitch);

void HoughSpace::smooth()
{
  TickTimer timer;

  wrapAround();

  if (fast())
    _houghSmooth(spaceUL() + 1, rWidth() - 2, TSpan, rPitch());
  else
  {
    uint16_t* buf = new uint16_t[2 * rWidth()];
    uint16_t* top = buf;
    uint16_t* mid = buf + rWidth();
    uint16_t* p = spaceUL() - rPitch();
    for (int t = 0; t < 2; ++t, p += rPitch())
      for (int r = 1; r < rWidth() - 1; ++r)
        top[r + t * rWidth()] = p[r - 1] + 2 * p[r] + p[r + 1];

    for (int t = 0; t < TSpan; ++t, p += rPitch())
    {
      for (int r = 1; r < rWidth() - 1; ++r)
      {
        int z = p[r - 1] + 2 * p[r] + p[r + 1];
        p[r - rPitch()] = top[r] + 2 * mid[r] + z;
        top[r] = z;
      }
      uint16_t* tmp = top;
      top = mid;
      mid = tmp;
    }

    delete[] buf;
  }

  rErode(1);


  times[2] = timer.time32();
}

void HoughSpace::peaks(HoughLineList& hlList)
{
  TickTimer timer;

  wrapAround();
  int diag1 = rPitch() + 1;
  int diag2 = rPitch() - 1;
  int accept = acceptThreshold() * 16;

  hlList.clear();

  if (fast())
  {
    uint16_t* row = spaceUL() + 1;
    for (int t = 0; t < TSpan; ++t, row += rPitch())
    {
      _runLengthU16(row, rWidth() - 2, accept - 1, runs);
      int ri = 0;
      for (int r = runs[0]; r >= 0; r = runs[++ri])
      {
        uint16_t* p = row + r;
        int z = *p;
        if (z > p[1       ] && z >= p[-1       ] &&
            z > p[rPitch()] && z >= p[-rPitch()] &&
            z > p[diag1   ] && z >= p[-diag1   ] &&
            z > p[diag2   ] && z >= p[-diag2   ])
            hlList.add(r - rRadius() + 1, t, z);
      }
    }
  }
  else
    for (int t = 0; t < TSpan; ++t)
    {
      uint16_t* p = spaceUL() + t * rPitch() + 1;
      for (int r = -rRadius() + 1; r < rRadius() - 1; ++r, ++p)
      {
        int z = *p;
        if (z >= accept &&
            z > p[1       ] && z >= p[-1       ] &&
            z > p[rPitch()] && z >= p[-rPitch()] &&
            z > p[diag1   ] && z >= p[-diag1   ] &&
            z > p[diag2   ] && z >= p[-diag2   ])
            hlList.add(r, t, z);
      }
    }

  times[3] = timer.time32();
}

void HoughSpace::adjust(EdgeList& edges, EdgeList& rejectedEdges, HoughLineList& hlList)
{
  TickTimer timer;

  AngleBinsIterator<Edge> abi(edges);
  for (Edge* e = *abi; e; e = *++abi)
    e->memberOf(0);

  hlList.sort();

  for (list<HoughLine>::iterator hl = hlList.begin(); hl != hlList.end();)
  {
    bool kill = false;
    for (int a = 0; a < adjustSteps(); ++a)
      if (!hl->adjust(edges, adjustSet().params[a], a == adjustSteps() - 1))
      {
        kill = true;
        break;
      }

    if (kill)
      hl = hlList.erase(hl);
    else
      ++hl;
  }

  // For center circle detection, collect all orphan edges
  rejectedEdges.reset();

  AngleBinsIterator<Edge> rejectABI(edges);
  for (Edge* e = *rejectABI; e; e = *++rejectABI)
    if (e->memberOf() == 0) 
      rejectedEdges.add(e->x(), e->y(), e->mag(), e->angle());
    
  times[4] = timer.time32();
}

void HoughSpace::run(EdgeList& edges, EdgeList& rejectedEdges, HoughLineList& hlList)
{
  TickTimer timer;

  clear();
  processEdges(edges);
  smooth();
  peaks(hlList);
  adjust(edges, rejectedEdges, hlList);

  times[5] = timer.time32();
}


}
}
