// ********************************
// *                              *
// *  Fast Connectivity Analysis  *
// *                              *
// ********************************

#include "FastBlob.h"
#include "Stdafx.h"
#include "Vision.h"

#include <cstring>

using namespace std;

namespace man {
namespace vision {

string Blob::print() const
{
  return strPrintf("%4d %6.1f %6.1f %6.1f\n", areaAboveFirst(), area(),
                   centerX(), centerY());
}

Connectivity::Connectivity()
{
  fifo = new Point[FifoSize];
  firstThreshold(192);
  secondThreshold(96);
  minWeight(8);
  minAreaAboveFirst(3);
  fast(true);
  weightThr(96, 192);
  runs = 0;
  runSize = 0;
}

extern "C" void _connectMark(uint8_t* image, int width, int height, int pitch, int t2);

void slowMark(uint8_t* image, int width, int height, int pitch, int t2)
{
  for (int y = 0; y < height; ++y)
  {
    uint8_t* p = image + y * pitch;
    for (int x = 0; x < width; ++x)
    {
      int z = *p & ~1;
      z |= (int)(z > t2);
      *p++ = (uint8_t)z;
    }
  }
}

void Connectivity::connect(int x, int y, uint8_t* p, int pitch)
{
  int readP = 0, writeP = 0;

  // Write seed to FIFO
  *p &= ~1;
  fifoWrite(writeP, p, x + (y << 16));

  // Create new blob
  Blob blob(weightThrRange());

  // Find all connected pixels
  while (readP != writeP)
  {
    uint8_t* q;
    uint32_t xy;
    fifoRead(readP, q, xy);

    // Update blob statistics
    double u = xy & 0xFFFF;
    double v = xy >> 16;
    blob.add(min(max(*q - weightThr0(), 0), weightThrRange()),
             u, v, *q > firstThreshold());

    // East neighbor
    int n = q[1];
    if ((n & 1) != 0)
    {
      q[1] = (uint8_t)(n & ~1);
      fifoWrite(writeP, q + 1, xy + 1);
    }

    // West neighbor
    n = q[-1];
    if ((n & 1) != 0)
    {
      q[-1] = (uint8_t)(n & ~1);
      fifoWrite(writeP, q - 1, xy - 1);
    }

    // South neighbor
    n = q[pitch];
    if ((n & 1) != 0)
    {
      q[pitch] = (uint8_t)(n & ~1);
      fifoWrite(writeP, q + pitch, xy + 0x10000);
    }

    // North neighbor
    n = q[-pitch];
    if ((n & 1) != 0)
    {
      q[-pitch] = (uint8_t)(n & ~1);
      fifoWrite(writeP, q - pitch, xy - 0x10000);
    }
  }

  // Add new blob to results if it's big enough
  if (blob.area() >= minWeight() && blob.areaAboveFirst() >= minAreaAboveFirst())
    blobs.push_back(blob);

}

void Connectivity::run(uint8_t* image, int width, int height, int pitch)
{
  TickTimer timer;

  // Mark pixels using LSB
  //   0   pixel <= second threshold or already included in a blob
  //   1   pixel > second threshold and not already included in a blob
  // The first and last rows and columns are marked as already included.

  // Mark first and last columns
  for (int y = 1; y < height - 1; ++y)
  {
    image[y * pitch] &= ~1;
    image[y * pitch + width - 1] &= ~1;
  }

  // Look for new blobs
  blobs.clear();

  // Set LSB of first threshold to match marked pixels
  int t1 = firstThreshold() | 1;

  if (fast())
  {
    // ASM pixel marking
    _connectMark(image + pitch + 1, width - 2, height - 2, pitch, secondThreshold());
    _connectMark(image, width, 2, pitch * (height - 1), 255);

    // Mark first and last columns. This could be outside the conditional and shared
    // between fast and slow, but cache performance is much better here. Not sure why.
    for (int y = 1; y < height - 1; ++y)
    {
      image[y * pitch] &= ~1;
      image[y * pitch + width - 1] &= ~1;
    }

    // Allocate runs for later edge detection
    int runSizeNeeded = ((width - 2 + 15) & ~15) + 1;
    if (runSizeNeeded > runSize)
    {
      delete[] runs;
      runs = new int32_t[runSizeNeeded];
      runSize = runSizeNeeded;
    }

    // fast loop
    for (int y = 1; y < height - 1; ++y)
    {
      uint8_t* row = image + y * pitch + 1;
      _runLengthU8(row, width - 2, t1, runs);
      int ri = 0;
      for (int x = runs[0]; x >= 0; x = runs[++ri])
      {
        uint8_t* p = row + x++;
        int z = *p;
        if ((z & 1) != 0)
          connect(x, y, p, pitch);
      }
    }
  }
  else
  {
    // C++ pixel marking
    slowMark(image + pitch + 1, width - 2, height - 2, pitch, secondThreshold());
    slowMark(image, width, 2, pitch * (height - 1), 255);

    // Mark first and last columns
    for (int y = 1; y < height - 1; ++y)
    {
      image[y * pitch] &= ~1;
      image[y * pitch + width - 1] &= ~1;
    }

    // slow loop
    for (int y = 1; y < height - 1; ++y)
    {
      uint8_t* p = image + y * pitch + 1;
      for (int x = 1; x < width - 1; ++x, ++p)
      {
        int z = *p;
        if (z > t1 && (z & 1) != 0)
          connect(x, y, p, pitch);
      }
    }
  }

  _ticks = (uint32_t)timer.time();
  _alignment = (int)image & 0xF;
}

bool Connectivity::test(string& message)
{
  struct PaintSpot
  {
    float x0, y0;
    float r;
    float bright;
  };

  const int width = 320, height = 240;
  const float edgeSigma = 0.4f;
  message.clear();

  PaintSpot paintList[] =
  {
    {  80,  60,  40,  0.90f},
    {  80,  60,  30, -0.80f},
    {  80,  60,  10,  0.80f},
    { 320, 240,  15,  0.85f},
    { 250, 180,  50,  0.50f},
    { 250, 180,   3,  0.35f},
    { 100, 170,  20,  0.20f}
  };

  uint8_t* image = new uint8_t[width * height];

  uint8_t* p = image;
  for (int y = 0; y < height; ++y)
    for (int x = 0; x < width; ++x, ++p)
    {
      double z = 0;
      for (int b = 0; b < sizeof(paintList) / sizeof(PaintSpot); ++b)
      {
        const PaintSpot& spot = paintList[b];
        double r = (sqrt((x - spot.x0) * (x - spot.x0) + (y - spot.y0) * (y - spot.y0)) - spot.r) / edgeSigma;
        double a;
        if (r > 10)
          a = 0;
        else if (r < -10)
          a = 1;
        else
          a = 1 / (1 + exp(r));
        z += a * spot.bright;
      }

      *p = (uint8_t)min(max((int)(255 * z), 0), 255);
    }

  // Test pixel marking
  uint8_t* imageCopy = new uint8_t[width * height];
  memcpy(imageCopy, image, width * height);
  _connectMark(image     + 5, width - 10, height, width, secondThreshold());
  slowMark   (imageCopy + 5, width - 10, height, width, secondThreshold());
  int err = 0;
  for (int i = 0; i < width * height; ++i)
    if (image[i] != imageCopy[i] && ++err < 12)
      message += strPrintf("Mark at (%3d,%3d) C++ says %02X, ASM says %02X\n",
                           i % width, i / width, imageCopy[i], image[i]);
  delete[] imageCopy;
  if (err != 0)
  {
    message += strPrintf("%d pixel marking errors\n", err);
    delete[] image;
    return false;
  }

  run(image, width, height, width);
  delete[] image;

  if (blobs.size() != 4)
  {
    message += strPrintf("Expected 4 blobs, found %d\n", blobs.size());
    return false;
  }

  // The following expected results came from running the code, and seem correct
  // based on the geometry of what is rendered. The centers of mass of the
  // first three blobs, which are wholly contained in the image, are clearly
  // correct, and the areas are reasonable from the circle area formula. It
  // would be possible to verity the areas more precisely from the rendering
  // parameters, but I have not done so. The fourth blob runs up to the
  // image border to make sure that works, but it's less clear what the
  // correct result should be. The results are with firstThreshold = 192,
  // secondThreshold = 96. Using these results for this test confirms that
  // the code works the same as it did when the original run was made.
  struct ExpectedResult
  {
    int areaAboveFirst;
    double area;
    double cmx, cmy;
  }
  expected[] =
  {
    { 1908, 2111.7,   80.0,   60.0},
    {  277,  304.2,   80.0,   60.0},
    {   21, 2392.9,  250.0,  180.0},
    {  117,  127.9,  313.1,  233.1}
  };

  const double epsilon = 0.1;
  list<Blob>::const_iterator bi = blobs.begin();
  bool ok = true;
  for (int i = 0; i < (int)blobs.size(); ++i, ++bi)
  {
    const Blob& b = *bi;
    const ExpectedResult& e = expected[i];
    if (b.areaAboveFirst() != e.areaAboveFirst  ||
        fabs(b.area   () - e.area) > epsilon ||
        fabs(b.centerX() - e.cmx ) > epsilon ||
        fabs(b.centerY() - e.cmy ) > epsilon)
    {
      // TODO make compile
      // message += strPrintf("Unexpected blob result for\n  %s", b.print());
      ok = false;
    }
  }
  
  if (ok)
    message += "Blob test passed\n";
  return ok;
}

string Connectivity::print() const
{
  string s;
  for (list<Blob>::const_iterator it = blobs.begin(); it != blobs.end(); ++it)
    s += it->print();
  s += strPrintf("%d ticks = %.1f us; alignment = %d\n", ticks(), ticks() / 1600.0, alignment());
  return s;
}

}
}
