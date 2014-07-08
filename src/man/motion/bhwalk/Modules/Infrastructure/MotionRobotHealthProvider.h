/**
* @file Modules/Infrastructure/MotionRobotHealthProvider.h
* This file declares a module that provides information about the robot's health.
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/

#pragma once

#include "Tools/ModuleBH/Module.h"
#include "Tools/RingBufferWithSum.h"
#include "Representations/Infrastructure/RobotHealth.h"

MODULE(MotionRobotHealthProvider)
  PROVIDES(MotionRobotHealthBH)
END_MODULE

/**
* @class MotionRobotHealthProvider
* A module that provides information about the robot's health
*/
class MotionRobotHealthProvider : public MotionRobotHealthProviderBase
{
private:
  /** The main function, called every cycle
  * @param motionRobotHealth The data struct to be filled
  */
  void update(MotionRobotHealthBH& motionRobotHealth);

  RingBufferWithSumBH<unsigned, 30> timeBuffer;        /** Buffered timestamps of previous executions */
  unsigned lastExecutionTime;

public:
  /** Constructor. */
  MotionRobotHealthProvider() : lastExecutionTime(0) {}
};
