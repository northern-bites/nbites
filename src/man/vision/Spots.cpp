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
  filterGain(0.5f);
  filterThreshold(40);
  greenThreshold(64);
}

SpotDetector::~SpotDetector()
{
  delete[] innerColumns;
  delete[] outerColumns;
  delete[] filteredImageMemory;
}

bool SpotDetector::alloc(const ImageLiteBase& src)
{
  std::cout<<"Spots Alloc Enter\n";
  if(!src.hasProperDimensions()) {
    std::cout<<"SRC Does not have proper dimensions\n";
    delete[] innerColumns;
    delete[] outerColumns;
    delete[] filteredImageMemory;
    _spots.clear();
    return false;
  }

  if (src.width() >= _filteredImage.width())
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
  int maxHeightNeeded = src.height() - initialOuterDiam() + 2;  // one extra for peak detect

  if(maxHeightNeeded <= 0) {
    _spots.clear();
    return false;
  }

  size_t sizeNeeded = pitchNeeded * maxHeightNeeded; //is sizeNeeded good?
  if (sizeNeeded >= filteredSize)
  {
    delete[] filteredImageMemory;
    filteredPixels = (uint8_t*)alignedAlloc(sizeNeeded, 4, filteredImageMemory);
    filteredSize = sizeNeeded;
  }
  
  int width = src.width();
  int height = maxHeightNeeded;
  if(width > 0 && height > 0) {
    _filteredImage = ImageLiteU8(src.x0() + ((initialOuterDiam() + 1) & 1), src.y0() - initialOuterDiam() + 1,
                               width, height, pitchNeeded, filteredPixels);
    return true;
  } else {
    return false;
  }
}

bool SpotDetector::spotDetect(const ImageLiteU8* green)
{
  if(!green->hasProperDimensions()) {
    _spots.clear();
    return false;
  }
  _spots.clear();
  int p = filteredImage().pitch();
  int spotThreshold = (int)(filterThreshold() * filterGain()) - 1;

  // Scan filtered image, look for peaks, reject those too green
  for (int y = 1; y < filteredImage().height(); ++y)
  {
    uint8_t* row = filteredImage().pixelAddr(0, y);
    int x0 = (row[0] >> 1);
    _runLengthU8(row + x0, filteredImage().width() - row[0] + 1, spotThreshold, outerColumns);

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
          int greenX0 = (green->x0() - filteredImage().x0() - diam + 1) >> 1;
          int greenY0 = (green->y0() - filteredImage().y0() - diam + 1) >> 1;
          int greenSum = 0;
          for (int gy = 0; gy < diam; ++gy)
          {
            const uint8_t* gp = green->pixelAddr(x + greenX0, y + greenY0 + gy);
            for (int gx = 0; gx < diam; ++gx)
              greenSum += *gp++;
          }
          if (greenSum > greenThreshold() * diam * diam)
            continue;
          greenScore = greenSum / (diam * diam);
        }
        Spot spot;
        spot.x = (x << 1) - filteredImage().x0();
        spot.y = (y << 1) - filteredImage().y0();
        spot.filterOutput = z;
        spot.green = greenScore;
        spot.outerDiam = row[0];
        spot.innerDiam = row[1];
        spots().push_back(spot);
      }
    }
  }

  // Reject weaker overlapping peaks. This is technically O(n^2), but we take
  // advantage of the natural sorting of the list to keep it fast.
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
  return true;
}

}
}
