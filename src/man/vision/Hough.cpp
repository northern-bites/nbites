// ***************************************************
// *                                                 *
// *  Hough Transform and Field Line Detection 2015  *
// *                                                 *
// ***************************************************

#include "Hough.h"
#include "FrontEnd.h"
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

void HoughLine::set(int rIndex, int tIndex, double r, double t, double score)
{
  GeoLine::set(r, t);
  _rIndex = rIndex;
  _tIndex = tIndex;
  _score = score;
  _fitError = -1;
  members = 0;
  fieldLine(-1);
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
  return strPrintf("%4d, %02X (%6.1f, %02X) %4.0f %4.2f [%4.0f ..%4.0f]",
                   rIndex(), tIndex(), r(), binaryAngle(), score(),
                   fitError(), ep0(), ep1());
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

// **************************
// *                        *
// *  Field Lines and List  *
// *                        *
// **************************

FieldLine::FieldLine(HoughLine& line1, HoughLine& line2, double fx0, double fy0)
{
  double d1 = fabs(line1.field().pDist(fx0, fy0));
  double d2 = fabs(line2.field().pDist(fx0, fy0));
  int i = (int)(d2 < d1);
  _lines[i    ] = &line1;
  _lines[i ^ 1] = &line2;
}

FieldLineList::FieldLineList()
{
  maxLineAngle(10.0f);
  maxLineSeparation(20.0f);
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
        double separation = -(hl1->field().r() + hl2->field().r());
        if (0.0 < separation && separation <= maxLineSeparation())
        {
          hl1->fieldLine((int)size());
          hl2->fieldLine((int)size());
          push_back(FieldLine(*hl1, *hl2, houghLines.fx0(), houghLines.fy0()));
        }
      }
  }
}

// ****************
// *               
// *  Calibration  
// *               
// ****************

bool FieldLineList::TiltCalibrate(FieldHomography& h, string* s)
{
#if 0
  if (s)
    s = "\n";
  //Stats tiltStats = new Stats();

  for (int i = 0; i < size() - 1; ++i)
    for (int j = i + 1; j < size(); ++j)
    {
      if (fsbs(lines[i].Ca * lines[j].Ca + lines[i].Sa * lines[j].Sa) < 0.33)
      {
        s += string.Format("Perpendicular field lines {0} and {1}:\n", i, j);
        for (int a = 0; a < 2; ++a)
          for (int b = 0; b < 2; ++b)
          {
            double t;
            string diag;
            bool ok = VisualTiltPerpendicular(lines[i].Lines(a), lines[j].Lines(b), out t, out diag);
            if (ok)
            {
              t *= 180 / Math.PI;
              tiltStats.Add(t);
              s += string.Format("  {0,4:f1},", t);
            }
            else
              s += "  ----,";
            s += string.Format(" | {0}\n", diag);
          }
      }
      else if (Math.Abs(lines[i].Ca * lines[j].Sa - lines[i].Sa * lines[j].Ca) < 0.33)
      {
        s += string.Format("Parallel field lines {0} and {1}:\n", i, j);
        for (int a = 0; a < 2; ++a)
          for (int b = 0; b < 2; ++b)
          {
            double t;
            string diag;
            bool ok = VisualTiltParallel(lines[i].Lines(a), lines[j].Lines(b), out t, out diag);
            if (ok)
            {
              t *= 180 / Math.PI;
              tiltStats.Add(t);
              s += string.Format("  {0,4:f1},", t);
            }
            else
              s += "  ----,";
            s += string.Format(" | {0}\n", diag);
          }
      }
    }

  s += string.Format("Mean tilt = {0:f2}, stDev = {1:f2}\n", tiltStats.Mean, tiltStats.StDev);
  return s;
#endif

  return false;
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
