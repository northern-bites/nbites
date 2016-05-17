// ********************************
// *                              *
// *  Spot Filter, Ball detector  *
// *                              *
// ********************************


#include "Stdafx.h"
#include "Spots.h"

#include <stdlib.h>

namespace man {
namespace vision {

// *******************
// *                 *
// *  Spot Detector  *
// *                 *
// *******************

SpotDetector::SpotDetector()
  : innerColumns(NULL), outerColumns(NULL), filteredImageMemory(NULL), filteredSize(0)
{
  initialInnerDiam(2);
  initialOuterDiam(4);
  darkSpot(true);
  innerDiamCm(3.f);
  innerGrowRows(50.f);
  outerGrowRows(30.f);
  filterGain(2.f);
  filterThreshold(40);
  greenThreshold(64);
}

SpotDetector::~SpotDetector()
{
  delete[] innerColumns;
  delete[] outerColumns;
  delete[] filteredImageMemory;
}

void SpotDetector::alloc(const ImageLiteBase& src)
{
  if (src.width() > _filteredImage.width())
  {
    delete[] innerColumns;
    delete[] outerColumns;
    int n = (src.width() + 15 ) & ~15;
    outerColumns = new int[n];
    innerColumns = new int[n];
  }

  enum
  {
    rowAlignBits = 4,
    rowAlignMask = (1 << rowAlignBits) - 1
  };
  int pitchNeeded = (src.width() + rowAlignMask) & ~rowAlignMask;
  int maxHeightNeeded = src.height() - initialOuterDiam() + 1;
  size_t sizeNeeded = pitchNeeded * maxHeightNeeded;
  if (sizeNeeded > filteredSize)
  {
    delete[] filteredImageMemory;
    filteredPixels = (uint8_t*)alignedAlloc(sizeNeeded, 4, filteredImageMemory);
    filteredSize = sizeNeeded;
  }
  _filteredImage = ImageLiteU8(src.x0() + ((initialOuterDiam() + 1) & 1), src.y0() - initialOuterDiam() + 1,
                               src.width(), maxHeightNeeded, pitchNeeded, filteredPixels);
}

void SpotDetector::spotDetect(int y0, const ImageLiteU8* green)
{
  _spots.clear();
  int p = filteredImage().pitch();
  int spotThreshold = (int)(filterThreshold() * filterGain()) - 1;

  // Scan filtered image, look for peaks, reject those too green
  for (int y = 1; y < filteredImage().height() - 1; ++y)
  {
    uint8_t* row = filteredImage().pixelAddr(0, y);
    int x0 = row[0] >> 1;
    _runLengthU8(row + x0, filteredImage().width() - row[0], spotThreshold, outerColumns);

    for (int i = 0; outerColumns[i] >= 0; ++i)
    {
      int x = outerColumns[i] + x0;
      uint8_t* c = row + x;
      int z = c[0];
      if (z >= c[1] && z > c[-1] && z >= c[p] && z > c[-p] &&
          z >= c[p + 1] && z > c[-1 - p] && z >= c[p - 1] && z > c[1 - p])
      {
        int greenScore = 0;
        if (green)
        {
          int diam = row[1];
          int org = diam >> 1;
          int greenSum = 0;
          for (int gy = 0; gy < diam; ++gy)
          {
            const uint8_t* gp = green->pixelAddr(x - org, y + y0 + gy - org);
            for (int gx = 0; gx < diam; ++gx)
              greenSum += *gp++;
          }
          if (greenSum >= greenThreshold() * diam * diam)
            continue;
          greenScore = greenSum / (diam * diam);
        }
        Spot spot;
        spot.x = x;
        spot.y = y + y0;
        spot.filterOutput = z;
        spot.green = greenScore;
        spot.outerDiam = row[0];
        spot.innerDiam = row[1];
        spots().push_back(spot);
      }
    }
  }

  // Reject weaker overlapping peaks
  if (spots().size() >= 2)
  {
    SpotIterator last = spots().end();
    --last;
    for (SpotIterator i = spots().begin(); i != last; ++i)
    {
      int xLo = i->xLo() - 1;
      int xHi = i->xHi() + 1;
      int yLo = i->yLo() - 1;
      int yHi = i->yHi() + 1;
      SpotIterator next = i;
      ++next;
      for (SpotIterator j = next; j != spots().end(); ++j)
      {
        if (j->yLo() > yHi)
          break;

        if (xLo > j->xHi() || xHi < j->xLo())
          continue;

        if (i->filterOutput > j->filterOutput)
          j->outerDiam = 0;
        else
          i->outerDiam = 0;
      }
    }
    for (SpotIterator i = spots().begin(); i != spots().end();)
      if (i->outerDiam == 0)
        i = spots().erase(i);
      else
        ++i;
  }
}

}
}
