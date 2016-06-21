#pragma once

#include "Tools/Module/Module.h"
#include "Representations/Sensing/RobotModel.h"
#include "Representations/Infrastructure/SensorData.h"
#include "Representations/Sensing/FsrData.h"

#include "Tools/RingBufferWithSum.h"

MODULE(FsrDataProvider)
REQUIRES(RobotModelBH)
REQUIRES(SensorDataBH)
PROVIDES_WITH_MODIFY(FsrDataBH)
DEFINES_PARAMETER(float, fsrWeightOffset, 2.0f)
END_MODULE

/**
 * @class ExpGroundContactDetector
 * A module for sensor data filtering.
 */
class FsrDataProvider : public FsrDataProviderBase
{
public:
  /** Default constructor. */
  FsrDataProvider();

private:
  Vector2BH<> LFsrFL;
  Vector2BH<> LFsrFR;
  Vector2BH<> LFsrRL;
  Vector2BH<> LFsrRR;
  Vector2BH<> RFsrFL;
  Vector2BH<> RFsrFR;
  Vector2BH<> RFsrRL;
  Vector2BH<> RFsrRR;

  RingBufferWithSumBH<float, 10> fsrL;
  RingBufferWithSumBH<float, 10> fsrR;

  void update(FsrDataBH& fsrData);
};
