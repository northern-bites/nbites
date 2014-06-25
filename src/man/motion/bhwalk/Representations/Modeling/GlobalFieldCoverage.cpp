/*
 * GlobalFieldCoverageBH.cpp
 *
 * Implementation of GlobalFieldCoverageBH methods.
 *
 * Author: Felix Wenk
 */

#include <cstring>
#include <algorithm>
#include "GlobalFieldCoverage.h"
#include "Platform/BHAssert.h"
#include "Tools/Math/Common.h"

unsigned char GlobalFieldCoverageBH::Grid::coverage(int index, unsigned time) const
{
  int sub = (time - cells[index]) / FieldCoverageBH::GridInterval::tick;
  if(sub < 0) sub = 0;
  return sub >= FieldCoverageBH::GridInterval::maxCoverage ? 0 : (unsigned char) (FieldCoverageBH::GridInterval::maxCoverage - sub);
}

void GlobalFieldCoverageBH::Grid::setCoverage(int index, unsigned time, unsigned char coverage)
{
  ASSERT(coverage <= FieldCoverageBH::GridInterval::maxCoverage);
  ASSERT(index < FieldCoverageBH::GridInterval::xSteps * FieldCoverageBH::GridInterval::ySteps);
  cells[index] = std::max(0, static_cast<int>(time - FieldCoverageBH::GridInterval::tick * (FieldCoverageBH::GridInterval::maxCoverage - coverage)));
}
