/**
* @file Representations/MotionControl/GetUpEngineOutput.h
* @author <A href="mailto:judy@tzi.de">Judith Müller</A>
*/

#pragma once

#include "Representations/Infrastructure/JointData.h"
#include "Tools/Math/Pose2D.h"

/**
* @class BikeEngineOutputBH
* A class that represents the output of the walking engine.
*/
STREAMABLE_WITH_BASE(GetUpEngineOutputBH, JointRequestBH,
{,
  (bool)(true) isLeavingPossible, /**< Is leaving the motion module possible now? */
  (Pose2DBH) odometryOffset,
});
