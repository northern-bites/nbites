/**
* @file TorsoMatrix.h
* Declaration of class TorsoMatrixBH.
* @author Colin Graf
*/

#pragma once

#include "Tools/Math/Pose3D.h"
#include "Tools/Streams/AutoStreamable.h"

/**
* @class TorsoMatrixBH
* Matrix describing the transformation from ground to the robot torso.
*/
STREAMABLE_WITH_BASE(TorsoMatrixBH, Pose3DBH,
{,
  (Pose3DBH) offset, /**< The estimated offset (including odometry) from last torso matrix to this one. (relative to the torso) */
  (bool)(false) isValid, /**< Matrix is only valid if robot is on ground. */
  (bool) leftSupportFoot, /**< Guess whether the left foot is the support foot. */
});
