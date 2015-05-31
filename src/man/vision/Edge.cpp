// *************************
// *                       *
// *  Edge Detection 2015  *
// *                       *
// *************************

#include "Edge.h"

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
  dstBlock = 0;
  _dstPitch = _dstWd = _dstHt = 0;
  dstAllocated = 0;

  runs = 0;
  runSize = 0;

  gradientThreshold(8);
  edgeThreshold(16);
  greenThreshold(12);
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
  dstBlock = 0;
  _dstPitch = _dstWd = _dstHt = 0;
  dstAllocated = 0;
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
  gradient(short* sourceImage, int width, int height, int pitch,
           uint16_t* gradientImage, int gradientThreshold);

uint32_t EdgeDetector::gradient(short* source, int width, int height, int pitch)
{
  // Compute destination images (gradient and edge) size, pitch, and offset. See the ASM
  // for more information. 
  _dstPitch = ((((long)source >> 1) + width + 7) & ~7) - (((long)source >> 1) & ~7);
  dstOffset = (((long)source >> 1) & 7) + 2;
  _dstWd = width - 2;
  _dstHt = height - 2;

  // If we don't have enough room for the destination images (from previous runs),
  // get more
  int dstSize = dstPitch() * dstHeight();
  if (dstSize > dstAllocated)
  {
    delete[] dstBlock;
    _gradImage = (uint16_t*)alignedAlloc(dstSize * 2, 4, dstBlock);
    dstAllocated = dstSize;
  }

  // Allocate runs for later edge detection
  int runSizeNeeded = ((dstWidth() + 15) & ~15) + 1;
  if (runSizeNeeded > runSize)
  {
    delete[] runs;
    runs = new int32_t[runSizeNeeded];
    runSize = runSizeNeeded;
  }

  TickTimer timer;

  // Run ASM or C++ grdient
  if (fast())
    ::gradient(source, width, height, 2 * pitch, _gradImage, gradientThreshold());
  else
  {
    for (int y = 1; y < height - 1; ++y)
      for (int x = 1; x < width - 1; ++x)
      {
        short* p = source + y * pitch + x;
        int u = (p[-pitch + 1] + 2 * p[+1    ] + p[ pitch + 1]) -
                (p[-pitch - 1] + 2 * p[-1    ] + p[ pitch - 1]);
        int v = (p[ pitch - 1] + 2 * p[ pitch] + p[ pitch + 1]) -
                (p[-pitch - 1] + 2 * p[-pitch] + p[-pitch + 1]);

        int mag, dir;
        cToP(u << 3, v << 3, mag, dir);

        uint16_t grad = (uint16_t)(mag > gradientThreshold() ? (mag << 8) + dir : 0);

        int i = (y - 1) * dstPitch() + x - 1;
        gradientImage()[i] = grad;
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

uint32_t EdgeDetector::edgeDetect(uint8_t* green, int greenPitch, EdgeList& edgeList)
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
    gradNeighbors[i] = 2 * (dstPitch() * dyTab[i] + dxTab[i]);
    int j = (i + 3) & 7;
    greenNeighbors[i] = greenPitch * dyTab[j] + dxTab[j];
  }
  greenNeighbors[8] = greenNeighbors[0];
  greenNeighbors[9] = greenNeighbors[1];

  uint16_t* gradRow = gradientImage();
  if (green)
    green += greenPitch;

  int x0 = dstWidth() / 2;
  int y0 = dstHeight() / 2;

  if (fast())
    for (int y = 1; y < dstHeight() - 1; ++y)
    {
      gradRow += dstPitch();
      if (green)
        green += greenPitch;

      runLengthU16(gradRow + 1, dstWidth() - 2, (edgeThreshold() << 8) | 0xFF, runs);

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
          if (green)
          {
            uint8_t* pg = green + x;
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
    for (int y = 1; y < dstHeight() - 1; ++y)
    {
      gradRow += dstPitch();
      if (green)
        green += greenPitch;

      for (int x = 1; x < dstWidth() - 1; ++x)
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
            if (green)
            {
              uint8_t* pg = green + x;
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
