/**
 * @file BehaviorControlOutput.h
 * Declaration of class BehaviorControlOutputBH
 *
 * @author Max Risler
 */

#pragma once

#include "Representations/MotionControl/ArmMotionRequest.h"
#include "Representations/MotionControl/MotionRequest.h"
#include "Representations/MotionControl/HeadMotionRequest.h"
#include "Representations/Infrastructure/TeamInfo.h"
#include "Representations/Infrastructure/RobotInfo.h"
#include "Representations/Infrastructure/GameInfo.h"
#include "Representations/BehaviorControl/BehaviorStatus.h"
#include "Representations/BehaviorControl/BehaviorLEDRequest.h"
#include "Representations/BehaviorControl/ActivationGraph.h"

/**
 * A class collecting the output from the behavior control module
 */
STREAMABLE(BehaviorControlOutputBH,
{,
  (ArmMotionRequestBH) armMotionRequest,
  (MotionRequestBH) motionRequest,
  (HeadMotionRequestBH) headMotionRequest,
  (OwnTeamInfoBH) ownTeamInfo,
  (RobotInfoBH) robotInfo,
  (GameInfoBH) gameInfo,
  (BehaviorStatus) behaviorStatus,
  (BehaviorLEDRequestBH) behaviorLEDRequest,
  (ActivationGraphBH) executionGraph,
});
