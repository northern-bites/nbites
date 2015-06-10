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

AdjustSet::AdjustSet()
{
  params[1].angleThr = FuzzyThr(0.08f, 0.12f);
  params[1].distanceThr = FuzzyThr(0.7f, 2.0f);
  params[1].fitThresold = 0.6;
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

string HoughLine::print() const
{
  return strPrintf("%4d, %02X, (%6.1f, %02X), %4.0f, %4.2f, [%4.0f ..%4.0f], %4d, %4d",
                   rIndex(), tIndex(), r(), binaryAngle(), score(),
                   fitError(), ep0(), ep1(), fieldLine(), index());
}

// *********************
// *                   *
// *  Hough Line List  *
// *                   *
// *********************

void HoughLineList::mapToField(const FieldHomography& h)
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
{}

string Corner::print() const
{
  return strPrintf("C, %4d, %4d, %4d", first->index(), second->index(), (int) id);
}

GoalboxDetector::GoalboxDetector()
  : std::pair<FieldLine*, FieldLine*>(NULL, NULL), parallelThreshold_(15), seperationThreshold_(20)
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

// TODO add length condition?
bool GoalboxDetector::validBox(HoughLine& line1, HoughLine& line2) const
{
  // Use world coordinates
  const GeoLine& field1 = line1.field();
  const GeoLine& field2 = line2.field();

  // Goalbox = two field lines that are (1) parallel and (2) seperated by 60 cm
  // Parallel
  double normalizedT1 = (field1.r() > 0 ? field1.t() : field1.t() - M_PI);
  double normalizedT2 = (field2.r() > 0 ? field2.t() : field2.t() - M_PI);
  bool parallel = diffRadians(normalizedT1, normalizedT2) < parallelThreshold()*TO_RAD;

  // Seperated by 60 cm
  double distBetween = fabs(field1.pDist(field2.r()*cos(field2.t()), field2.r()*sin(field2.t())));
  bool seperation = fabs(distBetween - GOALBOX_DEPTH) < seperationThreshold();

  return parallel && seperation;
}

string GoalboxDetector::print() const
{
  return strPrintf("B, %4d, %4d", first->index(), second->index());
}

CornerDetector::CornerDetector(int width_, int height_)
  : width(width_), 
    height(height_), 
    orthogonalThreshold_(40), 
    closeThreshold_(30), 
    farThreshold_(100), 
    edgeImageThreshold_(0.05)
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
      //      finding the same corner in all pairings of hough lines
      CornerID firstId = classify(line1[0], line2[0]);
      bool foundCorner = !(firstId == CornerID::None);
      for (int k = 0; k < 2; k++) {
        for (int l = 0; l < 2; l++) {
          if (k == 0 && l == 0) continue;
          CornerID newId = classify(line1[k], line2[l]);
          if (firstId != newId)
            foundCorner = false;
        }
      }

      // NOTE TFirst and TSecond are temporarily used to specify whether corner.first
      //      or corner.second is the top of the T corner. If TFirst, then corner.first
      //      is the vertical part of the T, otherwise, corner.second is 
      //      vertical part of the T. Before the corner detector finishes 
      //      finding corners, all references to TFirst and TSecond are replaced 
      //      with T and the corner.first is the vertical part of the T. This happens 
      //      below. Thus the client is not aware of TFirst and TSecond.

      // Create corner object and add to field lines
      if (foundCorner) {
        Corner newCorner;
        if (firstId == CornerID::TSecond)
          newCorner = Corner(&line2, &line1, CornerID::T);
        else if (firstId == CornerID::TFirst)
          newCorner = Corner(&line1, &line2, CornerID::T);
        else
          newCorner = Corner(&line1, &line2, firstId);
        line1.addCorner(newCorner);
        line2.addCorner(newCorner);
        this->push_back(newCorner);
      }
    }
  }
}

CornerID CornerDetector::classify(HoughLine& line1, HoughLine& line2) const
{
  // If intersection is in edge of image, don't classify corner
  double imageIntersectX;
  double imageIntersectY;

  bool intersects = line1.intersect(line2, imageIntersectX, imageIntersectY);
  if (!intersects) return CornerID::None;

  double xThreshold = (width / 2) - (width * edgeImageThreshold());
  double yThreshold = (height / 2) - (height * edgeImageThreshold());

  bool farEnoughFromImageEdge = (imageIntersectX >= -xThreshold &&
                                 imageIntersectX <=  xThreshold &&
                                 imageIntersectY >= -yThreshold &&
                                 imageIntersectY <=  yThreshold);
  if (!farEnoughFromImageEdge) return CornerID::None;

  // Use world coordinates
  const GeoLine& field1 = line1.field();
  const GeoLine& field2 = line2.field();

  // Check that lines are close to orthogonal
  double normalizedT1 = (field1.r() > 0 ? field1.t() : field1.t() - M_PI);
  double normalizedT2 = (field2.r() > 0 ? field2.t() : field2.t() - M_PI);
  bool orthogonal = diffRadians(normalizedT1, normalizedT2) - (M_PI / 2) < orthogonalThreshold()*TO_RAD;
  if (!orthogonal) return CornerID::None;

  // Find intersection point
  double worldIntersectX;
  double worldIntersectY;

  intersects = field1.intersect(field2, worldIntersectX, worldIntersectY);
  if (!intersects) return CornerID::None; 
  // NOTE should never happen since checked above in image coords

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
        if (field1.ep0() < 0 && field1.ep1() > 0 && field2.ep0() < 0 && field2.ep1() > 0)
          return CornerID::Concave;
        return CornerID::Convex;
      }
    }
  }
  // Find and classify t corners
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      // Found t
      // TODO for T corners, both endpoints outside of far threshold?
      if ((dist1[i] < closeThreshold() && dist2[j] > farThreshold()))
        return CornerID::TSecond;
      else if (dist2[j] < closeThreshold() && dist1[i] > farThreshold())
        return CornerID::TFirst;
    }
  }
}

// **************************
// *                        *
// *  Field Lines and List  *
// *                        *
// **************************

FieldLine::FieldLine(HoughLine& line1, HoughLine& line2, int index, double fx0, double fy0)
  : id_(LineID::Line), index_(index), corners_()
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

void FieldLineList::find(HoughLineList& houghLines)
{
  // Check max angle by dot product of unit vectors. Since lines must have opposite
  // polarity, dot product must be below a negative threshold.
  double maxCosAngle = -cos(maxLineAngle() * (M_PI / 180));

  clear();

  for (HoughLineList::iterator hl1 = houghLines.begin(); hl1 != houghLines.end(); ++hl1)
  {
    HoughLineList::iterator hl2 = hl1;
    for (++hl2; hl2 != houghLines.end(); ++hl2)
      // Here is the dot product 
      if (hl1->field().ux() * hl2->field().ux() + hl1->field().uy() * hl2->field().uy() <= maxCosAngle)
      {
        // Separation is sum of the two r values (distance of line to origin).
        // This is well defined and sensible for lines that may not be perfectly
        // parallel. For field lines the polarities are pointing towards each
        // other, which makes the sum of r's negative. A pair of nearly parallel
        // lines with the right separation but with polarities pointing away from
        // each other is not a field line. 
        double separation = hl1->field().separation(hl2->field());
        if (0.0 < separation && separation <= maxLineSeparation())
        {
          int index = size();
          hl1->fieldLine(index);
          hl2->fieldLine(index);
          push_back(FieldLine(*hl1, *hl2, index, houghLines.fx0(), houghLines.fy0()));
        }
      }
  }
}

// TODO goalie and the goalbox
// TODO convex and T -> classify as side goalbox?
// TODO midline classification? (cross detection, lots of green detection, etc.)
// TODO find and use field edge?
void FieldLineList::classify(GoalboxDetector& boxDetector, CornerDetector& cornerDetector)
{
  // Run goalbox detector
  bool boxFound = boxDetector.find(*this);
  bool topBoxFound = boxFound;
  bool endlineFound = boxFound;
  bool midlineFound = false;

  // Run corner detector
  cornerDetector.findCorners(*this);

  // Look for lines with two corners and classify
  for (int i = 0; i < size(); i++) {
    FieldLine& line = (*this)[i];
    if (line.id() != LineID::Line) continue;
    std::vector<Corner> corners = line.corners();

    bool oneConcave = false;
    bool oneConvex = false;
    bool oneTHorizontal = false;
    bool oneTVertical = false;

    for (int j = 0; j < corners.size(); j++) {
      // Concave
      if (corners[j].id == CornerID::Concave) {
        if (oneConcave) {
          line.id(LineID::Endline);
          endlineFound = true;
        } else
          oneConcave = true;
      }
      // Convex
      if (corners[j].id == CornerID::Convex) {
        if (oneConvex) {
          line.id(LineID::TopGoalbox);
          topBoxFound = true;
        } else
          oneConvex = true;
      }
      // T, horizontal part
      if (corners[j].id == CornerID::T &&
          corners[j].first == &line) {
        if (oneTHorizontal) {
          line.id(LineID::Endline);
          endlineFound = true;
        } else
          oneTHorizontal = true;
      }
      // T, vertical part
      if (corners[j].id == CornerID::T &&
          corners[j].second == &line) {
        if (oneTVertical) {
          line.id(LineID::Midline);
          midlineFound = true;
        } else
          oneTVertical = true;
      }
    }
  }

  // If we have found either top goalbox, endline, or midline, classification is simpler
  if (topBoxFound) { 
    for (int i = 0; i < size(); i++) {
      FieldLine& line = (*this)[i];
      if (line.id() != LineID::Line) continue;
      std::vector<Corner> corners = line.corners();

      for (int j = 0; j < corners.size(); j++) {
        // Convex corner -> line must be attached to top goalbox
        if (corners[j].id == CornerID::Convex)
          line.id(LineID::SideGoalbox); 
      }
    }
  }

  if (endlineFound) {
    for (int i = 0; i < size(); i++) {
      FieldLine& line = (*this)[i];
      if (line.id() != LineID::Line) continue;
      std::vector<Corner> corners = line.corners();

      for (int j = 0; j < corners.size(); j++) {
        // Concave corner -> line must be attached to endline -> sideline
        if (corners[j].id == CornerID::Concave)
          line.id(LineID::Sideline); 
      }
      for (int j = 0; j < corners.size(); j++) {
        // Concave corner -> line must be attached to endline -> sideline
        if (corners[j].id == CornerID::Concave)
          line.id(LineID::Sideline);
        // T corner + corner.first is endline -> line must be the side goalbox
        else if (corners[j].id == CornerID::T) {
          if (corners[j].first->id() == LineID::Endline)
            line.id(LineID::SideGoalbox); 
        }
      }
    }
  }

  // TODO more logic possible?
  if (midlineFound) { 
    for (int i = 0; i < size(); i++) {
      FieldLine& line = (*this)[i];
      if (line.id() != LineID::Line) continue;
      std::vector<Corner> corners = line.corners();

      for (int j = 0; j < corners.size(); j++) {
        // T corner + corner.second is midline -> line must be the sideline
        if (corners[j].id == CornerID::T) {
          if (corners[j].second->id() == LineID::Midline)
            line.id(LineID::Sideline); 
        }
      }
    }
  }

  // Classify less specifically, note that changing classification only
  // takes affect if new class is more specific than old class
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
        line.id(LineID::EndlineOrSideline);
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

void HoughSpace::adjust(EdgeList& edges, HoughLineList& hlList)
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

  times[4] = timer.time32();
}

void HoughSpace::run(EdgeList& edges, HoughLineList& hlList)
{
  TickTimer timer;

  clear();
  processEdges(edges);
  smooth();
  peaks(hlList);
  adjust(edges, hlList);

  times[5] = timer.time32();
}

}
}
