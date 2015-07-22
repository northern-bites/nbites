// *************************
// *                       *
// *  Edge Detection 2015  *
// *                       *
// *************************

#include "Edge.h"

namespace man {
namespace vision {

// ******************************************************
// *                                                    *
// *  Hash Table of Objects Indexed by 8-bit Direction  *
// *                                                    *
// ******************************************************
float AngleElement::cosTable[256];

static EdgeInit ei;

void AngleElement::init()
{
  for (int i = 0; i < 256; ++i)
    cosTable[i] = (float)::cos(i * M_PI / 128);
}

Edge& Edge::operator=(const Edge& e)
{
  AngleElement::operator=(e);
  _x = e.x();
  _y = e.y();
  _mag = e.mag();
  _memberOf = e.memberOf();
  _nextMember = 0;
  return *this;
}

// **********
// *        *
// *  Edge  *
// *        *
// **********

void Edge::setField(const FieldHomography& h)
{
  double wx, wy, dwx, dwy;
  h.fieldCoords(_x, _y, wx, wy);
  h.fieldVector(0, 0, sin(), -cos(), dwx, dwy);
  _field = FieldEdge(wx, wy, atan2(dwy, dwx));
}


// **************
// *            *
// *  EdgeList  *
// *            *
// **************

void EdgeList::mapToField(const FieldHomography& h)
{
  AngleBinsIterator<Edge> abi(*this);
  for (Edge* e = *abi; e; e = *++abi)
    e->setField(h);

  _fx0 = -h.wx0();
  _fy0 = -h.wy0();
}


// **************
// *            *
// *  Gradient  *
// *            *
// **************

short EdgeDetector::cosTab[5];
short EdgeDetector::sinTab[5];

void EdgeDetector::init()
{
  for (int i = 0; i < 5; ++i)
  {
    double rad = (1 << (4 - i)) * M_PI / 128;
    cosTab[i] = (short)(ldexp(cos(rad), 15) + 0.5);
    sinTab[i] = (short)(ldexp(sin(rad), 15) + 0.5);
  }
}

EdgeDetector::EdgeDetector()
{
  gradMem = 0;
  gradAllocated = 0;

  runs = 0;
  runSize = 0;

  gradientThreshold(8);
  edgeThreshold(16);
  greenThreshold(220);
  correctEdgeDirection(false);
  fast(true);
}

EdgeDetector& EdgeDetector::operator= (const EdgeDetector& ed)
{
  gradientThreshold(ed.gradientThreshold());
  edgeThreshold(ed.edgeThreshold());
  greenThreshold(ed.greenThreshold());
  correctEdgeDirection(ed.correctEdgeDirection());
  fast(ed.fast());
  return *this;
}

EdgeDetector::EdgeDetector(const EdgeDetector& ed)
{
  gradMem = 0;
  gradAllocated = 0;

  runs = 0;
  runSize = 0;

  *this = ed;
}


// Fixed point cartesian to polar conversion. Match SIMD version in ASM. See comments
// in ASM code for more information.
void EdgeDetector::cToP(int x, int y, int& mag, int& dir)
{
  int d = 0x3F;

  // Half-plane
  if (y < 0)
  {
    y = -y;
    d ^= 0x1FF;
  }

  // Quadrant
  if (x < 0)
  {
    x = -x;
    d ^= 0xFF;
  }

  // Octant
  if (x < y)
  {
    int tmp = x;
    x = y;
    y = tmp;
    d ^= 0x7F;
  }

  // Next 5 bits by rotating (x, y) and folding when y < 0
  int bits = 0x3F;
  for (int i = 0; i < 5; ++i, bits >>= 1)
  {
    int t;
    t = ((x * cosTab[i] + 0x4000) >> 15) + ((y * sinTab[i] + 0x4000) >> 15);
    y = ((y * cosTab[i] + 0x4000) >> 15) - ((x * sinTab[i] + 0x4000) >> 15);
    x = t;
    if (y < 0)
    {
      y = -y;
      d ^= bits;
    }
  }

  // 9ty direction bit for rounding off to 8
  if (82 * y < x)
    d ^= 1;

  // Return magnitude and direction
  dir = ((d + 1) >> 1) & 0xFF;
  mag = (x + 0x40) >> 7;
}

extern "C" uint32_t
  _gradient(uint16_t* sourceImage, int width, int height, int pitch,
            uint16_t* gradientImage, int gradientThreshold);

uint32_t EdgeDetector::gradient(const ImageLiteU16& source)
{
  // Compute destination images (gradient and edge) size, pitch, and offset. See the ASM
  // for more information. 
  long addr = (long)(source.pixelAddr()) >> 1;
  int pitch = ((addr + source.width() + 7) & ~7) - (addr & ~7);
  int offset = (addr & 7) + 2;

  // If we don't have enough room for the destination images (from previous runs),
  // get more
  int size = pitch * (source.height() - 2);
  if (size > gradAllocated)
  {
    delete[] gradMem;
    gradPixels = (uint16_t*)alignedAlloc(size * 2, 4, gradMem);
    gradAllocated = size;
  }

  _gradImage = ImageLiteU16(source, 2, pitch, gradPixels + offset);

  // Allocate runs for later edge detection
  int runSizeNeeded = ((_gradImage.width() + 15) & ~15) + 1;
  if (runSizeNeeded > runSize)
  {
    delete[] runs;
    runs = new int32_t[runSizeNeeded];
    runSize = runSizeNeeded;
  }

  TickTimer timer;

  // Run ASM or C++ grdient
  if (fast())
    _gradient(source.pixelAddr(), source.width(), source.height(), 
                           2 * source.pitch(), gradPixels, gradientThreshold());
  else
  {
    int pitch = source.pitch();
    for (int y = 1; y < source.height() - 1; ++y)
      for (int x = 1; x < source.width() - 1; ++x)
      {
        uint16_t* p = source.pixelAddr(x, y);
        int u = (p[-pitch + 1] + 2 * p[+1    ] + p[ pitch + 1]) -
                (p[-pitch - 1] + 2 * p[-1    ] + p[ pitch - 1]);
        int v = (p[ pitch - 1] + 2 * p[ pitch] + p[ pitch + 1]) -
                (p[-pitch - 1] + 2 * p[-pitch] + p[-pitch + 1]);

        int mag, dir;
        cToP(u << 3, v << 3, mag, dir);

        uint16_t grad = (uint16_t)(mag > gradientThreshold() ? (mag << 8) + dir : 0);

        *gradientImage().pixelAddr(x - 1, y - 1) = grad;
      }
  }

  _gradTime = (uint32_t)timer.time();
  return gradientTime();
}

// ********************
// *                  *
// *  Edge Detection  *
// *                  *
// ********************

uint32_t EdgeDetector::edgeDetect(const ImageLiteU8& green, EdgeList& edgeList)
{
  static int8_t edgeDirCorrection[64] =
  {  0,  0,  0,  0,  0,  0,  0,  0,    0,  1,  1,  1,  1,  1,  1,  1,
     1,  1,  1,  1,  1,  1,  1,  1,    0,  0,  0,  0,  0,  0,  0,  0,

     0,  0,  0,  0,  0,  0,  0,  0,   -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1,  0,    0,  0,  0,  0,  0,  0,  0,  0,
  };

  TickTimer timer;

  edgeList.reset();

  static int dxTab[8] = { 1,  1,  0, -1, -1, -1,  0,  1};
  static int dyTab[8] = { 0,  1,  1,  1,  0, -1, -1, -1};
  int gradNeighbors[8];
  int greenNeighbors[10];
  for (int i = 0; i < 8; ++i)
  {
    gradNeighbors[i] = 2 * (gradientImage().pitch() * dyTab[i] + dxTab[i]);
    int j = (i + 3) & 7;
    greenNeighbors[i] = green.pitch() * dyTab[j] + dxTab[j];
  }
  greenNeighbors[8] = greenNeighbors[0];
  greenNeighbors[9] = greenNeighbors[1];

  uint16_t* gradRow = gradientImage().pixelAddr();
  uint8_t* greenRow = green.pixelAddr();
  if (greenRow)
    greenRow += green.pitch() + 1;

  //int x0 = dstWidth() / 2;
  //int y0 = dstHeight() / 2;
  int x0 = gradientImage().x0() >> 1;
  int y0 = gradientImage().y0() >> 1;

  if (fast())
    for (int y = 1; y < gradientImage().height() - 1; ++y)
    {
      gradRow += gradientImage().pitch();
      if (greenRow)
        greenRow += green.pitch();

      man::vision::_runLengthU16(gradRow + 1, gradientImage().width() - 2, 
                                 (edgeThreshold() << 8) | 0xFF, runs);

      int ri = 0;
      for (int x = runs[0]; x >= 0; x = runs[++ri])
      {
        ++x;
        uint8_t* pmag = (uint8_t*)(gradRow + x) + 1;
        int mag = *pmag;
        int dir = pmag[-1];
        int octant = ((dir + 0x10) >> 5) & 7;
        int nbr = gradNeighbors[octant];
        if (mag > pmag[nbr] && mag >= pmag[-nbr])
        {
          if (greenRow)
          {
            uint8_t* pg = greenRow + x;
            nbr = greenNeighbors[octant];
            int g = pg[nbr] - pg[-nbr];
            nbr = greenNeighbors[octant + 1];
            g += pg[nbr] - pg[-nbr];
            nbr = greenNeighbors[octant + 2];
            g += pg[nbr] - pg[-nbr];
            if (g < greenThreshold())
              goto noGreen;
          }

          if (correctEdgeDirection())
            dir += edgeDirCorrection[dir & 0x3F];
          edgeList.add(x - x0, y0 - y, mag, -dir);
          noGreen: ;
        }
      }
    }
  else
    for (int y = 1; y < gradientImage().height() - 1; ++y)
    {
      gradRow += gradientImage().pitch();
      if (greenRow)
        greenRow += green.pitch();

      for (int x = 1; x < gradientImage().width() - 1; ++x)
      {
        uint8_t* pmag = (uint8_t*)(gradRow + x) + 1;
        int mag = *pmag;
        if (mag > edgeThreshold())
        {
          int dir = pmag[-1];
          int octant = ((dir + 0x10) >> 5) & 7;
          int nbr = gradNeighbors[octant];
          if (mag > pmag[nbr] && mag >= pmag[-nbr])
          {
            if (greenRow)
            {
              uint8_t* pg = greenRow + x;
              nbr = greenNeighbors[octant];
              int g = pg[nbr] - pg[-nbr];
              nbr = greenNeighbors[octant + 1];
              g += pg[nbr] - pg[-nbr];
              nbr = greenNeighbors[octant + 2];
              g += pg[nbr] - pg[-nbr];
              if (g < greenThreshold())
                goto noGreen2;
            }

            if (correctEdgeDirection())
              dir += edgeDirCorrection[dir & 0x3F];
            edgeList.add(x - x0, y0 - y, mag, -dir);
            noGreen2: ;
          }
        }
      }
    }

  _edgeTime = (uint32_t)timer.time();
  return edgeTime();
}

}
}
