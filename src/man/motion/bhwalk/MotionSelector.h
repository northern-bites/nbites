/**
* @file Modules/MotionControl/MotionSelector.h
* This file declares a module that is responsible for controlling the motion.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
* @author <A href="mailto:allli@tzi.de">Alexander Härtl</A>
*/

#pragma once

#include "Tools/Module/Module.h"
#include "Representations/Infrastructure/FrameInfo.h"
#include "Representations/MotionControl/SpecialActionsOutput.h"
#include "Representations/MotionControl/WalkingEngineOutput.h"
#include "Representations/MotionControl/KickEngineOutput.h"
#include "Representations/MotionControl/GetUpEngineOutput.h"
#include "Representations/MotionControl/MotionRequest.h"
#include "Representations/MotionControl/MotionSelection.h"
#include "Representations/Sensing/GroundContactState.h"

MODULE(MotionSelector)
  USES(SpecialActionsOutputBH)
  USES(WalkingEngineOutputBH)
  USES(KickEngineOutput)
  USES(GetUpEngineOutputBH)
  REQUIRES(FrameInfoBH)
  REQUIRES(MotionRequestBH)
  REQUIRES(GroundContactStateBH)
  PROVIDES_WITH_MODIFY(MotionSelectionBH)
END_MODULE

class MotionSelector : public MotionSelectorBase
{
public:
  static PROCESS_WIDE_STORAGE(MotionSelector) theInstance; /**< The only instance of this module. */

  bool forceStand;
  MotionRequestBH::Motion lastMotion;
  MotionRequestBH::Motion prevMotion;
  unsigned lastExecution;
  SpecialActionRequest::SpecialActionID lastActiveSpecialAction;
  void update(MotionSelectionBH& motionSelection);

  /**
  * Can be used to overwrite all other motion requests with a stand request.
  * Must be called again in every frame a stand is desired.
  */
  static void stand();

  /**
  * Default constructor.
  */
  MotionSelector() : lastMotion(MotionRequestBH::specialAction), prevMotion(MotionRequestBH::specialAction),
    lastActiveSpecialAction(SpecialActionRequest::playDead)
  {
    theInstance = this;
  }
};
