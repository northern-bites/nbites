#pragma once

#include "Tools/Module/Module.h"
#include "Representations/Sensing/RobotModel.h"
#include "Representations/Infrastructure/SensorData.h"
#include "Representations/Sensing/FsrZmp.h"

#include "Tools/RingBufferWithSum.h"

MODULE(FsrZmpProvider)
REQUIRES(RobotModelBH)
REQUIRES(SensorDataBH)
REQUIRES(RobotDimensionsBH)
PROVIDES_WITH_MODIFY(FsrZmpBH)
END_MODULE

/**
 * @class FsrZmpProvider
 * Computes the center of pressure for the left foot, right foot and whole robot
 * using a simple weighted sum.
 */
class FsrZmpProvider : public FsrZmpProviderBase
{
public:
  /** Default constructor. */
  FsrZmpProvider();

private:
  Vector3BH<> fsrBasePositions[8];/**< Coordinates of the fsr sensors relative to the ankles. Lfs are relative to left ankle, Rfs to right ankle*/
  void update(FsrZmpBH& fsrData);
  
  /**Calculate the current fsr positions for the given limb and store it in positions starting at startIndex*/
  void calculateFsrPosition(MassCalibrationBH::Limb limb, int startIndex, Vector3BH<> (&positions)[8]) const;
};
