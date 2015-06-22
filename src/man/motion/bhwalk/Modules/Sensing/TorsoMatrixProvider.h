/**
* @file TorsoMatrixProvider.h
* Declaration of module TorsoMatrixProvider.
* @author Colin Graf
*/

#pragma once

#include "Tools/Module/Module.h"
#include "Representations/Configuration/RobotDimensions.h"
#include "Representations/Sensing/OrientationData.h"
#include "Representations/Sensing/RobotModel.h"
#include "Representations/Sensing/TorsoMatrix.h"
#include "Representations/Sensing/GroundContactState.h"
#include "Representations/MotionControl/OdometryData.h"

MODULE(TorsoMatrixProvider)
  REQUIRES(OrientationDataBH)
  REQUIRES(RobotModelBH)
  REQUIRES(RobotDimensionsBH)
  REQUIRES(GroundContactStateBH)
  PROVIDES_WITH_MODIFY(TorsoMatrixBH)
  USES(TorsoMatrixBH)
  PROVIDES_WITH_MODIFY_AND_OUTPUT(OdometryDataBH)
END_MODULE

/**
* @class TorsoMatrixProvider
* A module that provides the (estimated) position and velocity of the inertia board.
*/
class TorsoMatrixProvider : public TorsoMatrixProviderBase
{
public:
  static PROCESS_WIDE_STORAGE(TorsoMatrixProvider) theInstance;

  /** Updates the TorsoMatrixBH representation.
  * @param torsoMatrix The inertia matrix representation which is updated by this module.
  */
  void update(TorsoMatrixBH& torsoMatrix);

  /** Updates the OdometryDataBH representation.
  * @param odometryData The odometry data representation which is updated by this module.
  */
  void update(OdometryDataBH& odometryData);

private:
  float lastLeftFootZRotation; /**< The last z-rotation of the left foot. */
  float lastRightFootZRotation; /**< The last z-rotation of the right foot. */

  Vector3BH<> lastFootSpan; /**< The last span between both feet. */
  Pose3DBH lastTorsoMatrix; /**< The last torso matrix for calculating the odometry offset. */
};
