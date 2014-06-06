/**
* @file FallDownStateDetector.h
*
* This file declares a module that provides information about the current state of the robot's body.
*
* @author <a href="mailto:maring@informatik.uni-bremen.de">Martin Ring</a>
*/

#pragma once

#include "Representations/Infrastructure/SensorData.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/MotionControl/MotionInfo.h"
#include "Representations/Sensing/FallDownState.h"
#include "Representations/Sensing/InertiaSensorData.h"
#include "Tools/Module/Module.h"
#include "Tools/RingBufferWithSum.h"


MODULE(FallDownStateDetector)
  REQUIRES(FilteredSensorDataBH)
  REQUIRES(InertiaSensorDataBH)
  USES(MotionInfoBH)
  REQUIRES(FrameInfoBH)
  PROVIDES_WITH_MODIFY_AND_DRAW(FallDownStateBH)
  DEFINES_PARAMETER(int, fallTime, 1000) /**< The time (in ms) to remain in state 'falling' after a detected fall */
  DEFINES_PARAMETER(float, staggeringAngleX, 40) /**< The threshold angle which is used to detect the robot is staggering to the back or front*/
  DEFINES_PARAMETER(float, staggeringAngleY, 30) /**< The threshold angle which is used to detect the robot is staggering sidewards*/
  DEFINES_PARAMETER(float, fallDownAngleY, 45) /**< The threshold angle which is used to detect a fall to the back or front*/
  DEFINES_PARAMETER(float, fallDownAngleX, 55) /**< The threshold angle which is used to detect a sidewards fall */
  DEFINES_PARAMETER(float, onGroundAngle, 75) /**< The threshold angle which is used to detect the robot lying on the ground */
END_MODULE


/**
* @class FallDownStateDetector
*
* A module for computing the current body state from sensor data
*/
class FallDownStateDetector: public FallDownStateDetectorBase
{
private:
  bool isFalling();
  bool isStaggering();
  bool isCalibrated();
  bool specialSpecialAction();
  bool isUprightOrStaggering(FallDownStateBH& fallDownState);
  FallDownStateBH::Direction directionOf(float angleX, float angleY);
  FallDownStateBH::Sidestate sidewardsOf(FallDownStateBH::Direction dir);

  unsigned lastFallDetected;

  /** Indices for buffers of sensor data */
  ENUM(BufferEntry, accX, accY, accZ);

  /** Buffers for averaging sensor data */
  RingBufferWithSumBH<float, 15> buffers[numOfBufferEntrys];

public:
  static PROCESS_WIDE_STORAGE(FallDownStateDetector) theInstance;
  /**
  * Default constructor.
  */
  FallDownStateDetector();

  /** Executes this module
  * @param fallDownState The data structure that is filled by this module
  */
  void update(FallDownStateBH& fallDownState);
};
