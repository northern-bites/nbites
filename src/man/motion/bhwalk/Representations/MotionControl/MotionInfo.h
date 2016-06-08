/**
* @file MotionInfo.h
* Definition of class MotionInfoBH.
* @author Martin Lötzsch
*/

#pragma once

#include "MotionRequest.h"

/**
* @class MotionInfoBH
* The executed motion request and additional information about the motions which are executed by the Motion process.
*/
STREAMABLE_WITH_BASE(MotionInfoBH, MotionRequestBH,
{,
  (bool)(false) isMotionStable, /**< If true, the motion is stable, leading to a valid torso / camera matrix. */
  (Pose2DBH) upcomingOdometryOffset, /**< The remaining odometry offset for the currently executed motion. */
});
