/**
* @file Modules/MotionControl/MotionSelector.cpp
* This file implements a module that is responsible for controlling the motion.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
* @author <A href="mailto:allli@tzi.de">Alexander Härtl</A>
*/

#include <algorithm>
#include "MotionSelector.h"
#include "Tools/Debugging/DebugDrawings.h"

MAKE_MODULE(MotionSelector, Motion Control)

PROCESS_WIDE_STORAGE(MotionSelector) MotionSelector::theInstance = 0;

void MotionSelector::stand()
{
  if(theInstance)
  {
#ifdef TARGET_ROBOT
    theInstance->forceStand = true;
#endif
  }
}

void MotionSelector::update(MotionSelectionBH& motionSelection)
{
  static int interpolationTimes[MotionRequestBH::numOfMotions];
  interpolationTimes[MotionRequestBH::walk] = 300;
  interpolationTimes[MotionRequestBH::kick] = 300;
  interpolationTimes[MotionRequestBH::specialAction] = 10;
  interpolationTimes[MotionRequestBH::stand] = 300;
  interpolationTimes[MotionRequestBH::getUp] = 600;
  static const int playDeadDelay(2000);

  if(lastExecution)
  {
    MotionRequestBH::Motion requestedMotion = theMotionRequestBH.motion;
    if(theMotionRequestBH.motion == MotionRequestBH::walk && !theGroundContactStateBH.contact)
      requestedMotion = MotionRequestBH::stand;

    if(forceStand && (lastMotion == MotionRequestBH::walk || lastMotion == MotionRequestBH::stand))
    {
      requestedMotion = MotionRequestBH::stand;
      forceStand = false;
    }

    // check if the target motion can be the requested motion (mainly if leaving is possible)
    if((lastMotion == MotionRequestBH::walk && (!&theWalkingEngineOutputBH || theWalkingEngineOutputBH.isLeavingPossible || !theGroundContactStateBH.contact)) ||
       lastMotion == MotionRequestBH::stand || // stand can always be left
       (lastMotion == MotionRequestBH::specialAction) ||
       (lastMotion == MotionRequestBH::getUp && (!&theGetUpEngineOutputBH || theGetUpEngineOutputBH.isLeavingPossible)) ||
       (lastMotion == MotionRequestBH::kick && theKickEngineOutput.isLeavingPossible)
        )
    {
      motionSelection.targetMotion = requestedMotion;
    }

    if(requestedMotion == MotionRequestBH::specialAction)
    {
      motionSelection.specialActionRequest = theMotionRequestBH.specialActionRequest;
    }
    else
    {
      motionSelection.specialActionRequest = SpecialActionRequest();
      if(motionSelection.targetMotion == MotionRequestBH::specialAction)
        motionSelection.specialActionRequest.specialAction = SpecialActionRequest::numOfSpecialActionIDs;
    }

    // increase / decrease all ratios according to target motion
    const unsigned deltaTime(theFrameInfoBH.getTimeSince(lastExecution));
    const int interpolationTime = prevMotion == MotionRequestBH::specialAction && lastActiveSpecialAction == SpecialActionRequest::playDead ? playDeadDelay : interpolationTimes[motionSelection.targetMotion];
    float delta((float)deltaTime / interpolationTime);
    ASSERT(SystemCall::getMode() == SystemCall::logfileReplay || delta > 0.00001f);
    float sum(0);
    for(int i = 0; i < MotionRequestBH::numOfMotions; i++)
    {
        if(i == motionSelection.targetMotion) {
            motionSelection.ratios[i] += delta;
        }
      else
        motionSelection.ratios[i] -= delta;
      motionSelection.ratios[i] = std::max(motionSelection.ratios[i], 0.0f); // clip ratios
      sum += motionSelection.ratios[i];
    }
    ASSERT(sum != 0);
    // normalizeBH ratios
    for(int i = 0; i < MotionRequestBH::numOfMotions; i++)
    {
      motionSelection.ratios[i] /= sum;
      if(std::abs(motionSelection.ratios[i] - 1.f) < 0.00001f) {
        motionSelection.ratios[i] = 1.f; // this should fix a "motionSelection.ratios[motionSelection.targetMotion] remains smaller than 1.f" bug
      }
    }

    if(motionSelection.ratios[MotionRequestBH::specialAction] < 1.f)
    {
      if(motionSelection.targetMotion == MotionRequestBH::specialAction)
        motionSelection.specialActionMode = MotionSelectionBH::first;
      else
        motionSelection.specialActionMode = MotionSelectionBH::deactive;
    }
    else
      motionSelection.specialActionMode = MotionSelectionBH::active;

    if(motionSelection.specialActionMode == MotionSelectionBH::active && motionSelection.specialActionRequest.specialAction != SpecialActionRequest::numOfSpecialActionIDs)
      lastActiveSpecialAction = motionSelection.specialActionRequest.specialAction;
  }
  lastExecution = theFrameInfoBH.time;
  if(lastMotion != motionSelection.targetMotion)
    prevMotion = lastMotion;
  lastMotion = motionSelection.targetMotion;

  PLOT("module:MotionSelector:ratios:walk", motionSelection.ratios[MotionRequestBH::walk]);
  PLOT("module:MotionSelector:ratios:stand", motionSelection.ratios[MotionRequestBH::stand]);
  PLOT("module:MotionSelector:ratios:specialAction", motionSelection.ratios[MotionRequestBH::specialAction]);
  PLOT("module:MotionSelector:lastMotion", lastMotion);
  PLOT("module:MotionSelector:prevMotion", prevMotion);
  PLOT("module:MotionSelector:targetMotion", motionSelection.targetMotion);
}
