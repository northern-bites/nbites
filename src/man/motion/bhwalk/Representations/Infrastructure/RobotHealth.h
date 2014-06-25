/**
* @file RobotHealth.h
* The file declares two classes to transport information about the current robot health
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/

#pragma once

#include "Tools/Streams/AutoStreamable.h"
#include "Tools/Debugging/DebugDrawings.h"

/**
* @class MotionRobotHealthBH
* All information collected within motion process.
*/
STREAMABLE(MotionRobotHealthBH,
{,
  (float)(0) motionFrameRate, /*< Frames per second within process "Motion" */
  (float) avgMotionTime, /**< average execution time */
  (float) maxMotionTime, /**< Maximum execution time */
  (float) minMotionTime, /**< Minimum execution time */
});

/**
* @class RobotHealthBH
* Full information about the robot.
*/
STREAMABLE_WITH_BASE(RobotHealthBH, MotionRobotHealthBH,
{
public:
  /** Assigning MotionRobotHealthBH
  * @param motionRobotHealth Information from the motion process
  */
  void operator=(const MotionRobotHealthBH& motionRobotHealth)
  {
    motionFrameRate = motionRobotHealth.motionFrameRate;
    minMotionTime = motionRobotHealth.minMotionTime;
    maxMotionTime = motionRobotHealth.maxMotionTime;
    avgMotionTime = motionRobotHealth.avgMotionTime;
  }

  void draw() const
  {
    DECLARE_PLOT("representation:RobotHealthBH:batteryLevel");
    PLOT("representation:RobotHealthBH:batteryLevel", batteryLevel);
    DECLARE_PLOT("representation:RobotHealthBH:maxJointTemperature");
    PLOT("representation:RobotHealthBH:maxJointTemperature", maxJointTemperature);
    DECLARE_PLOT("representation:RobotHealthBH:totalCurrent");
    PLOT("representation:RobotHealthBH:totalCurrent", totalCurrent);
  },

  (float)(0) cognitionFrameRate, /*< Frames per second within process "Cognition" */
  (unsigned char)(0) batteryLevel, /*< Current batteryLevel of robot battery in percent */
  (float)(0) totalCurrent, /*< Sum of all motor currents ( as a measure for the robot's current load) */
  (unsigned char)(0) maxJointTemperature, /*< Highest temperature of a robot actuator */
  (unsigned char)(0) cpuTemperature, /**< The temperature of the cpu */
  (unsigned char)(0) boardTemperature, /**< The temperature of the mainboard or northbridge, dunno */
  (unsigned char[3]) load, /*< load averages */
  (unsigned char) memoryUsage, /*< Percentage of used memory */
  (std::string) robotName, /*< For fancier drawing :-) */
  (unsigned int) ballPercepts, /**< A ball percept counter used to determine ball percepts per hour */
  (unsigned int) linePercepts, /**< A line percept counter used to determine line percepts per hour */
  (unsigned int) goalPercepts, /**< A goal percept counter used to determine goal percepts per hour */
  (bool) wlan, /**< Status of the wlan hardware. true: wlan hardware is ok. false: wlan hardware is (probably physically) broken. */

  // Initialization
  load[0] = load[1] = load[2] = 0;
});
