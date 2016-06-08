/**
* @file Pose2DBH.cpp
* Contains class Pose2DBH
* @author <a href="mailto:martin.kallnik@gmx.de">Martin Kallnik</a>
* @author Max Risler
*/

#include "Pose2D.h"
#include "Tools/Range.h"
#include "Tools/Math/Random.h"

Pose2DBH Pose2DBH::random(const RangeBH<float>& x,
                      const RangeBH<float>& y,
                      const RangeBH<float>& angle)
{
  // angle should even work in wrap around case!
  return Pose2DBH(float(::randomFloat() * (angle.max - angle.min) + angle.min),
                Vector2BH<>(float(::randomFloat() * (x.max - x.min) + x.min),
                          float(::randomFloat() * (y.max - y.min) + y.min)));
}
