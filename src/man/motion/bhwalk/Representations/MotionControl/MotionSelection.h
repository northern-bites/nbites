/**
* @file Representations/MotionControl/MotionSelection.h
* This file declares a class that represents the motions actually selected based on the constraints given.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#pragma once

#include "MotionRequest.h"
#include <cstring>

/**
* @class MotionSelectionBH
* A class that represents the motions actually selected based on the constraints given.
*/
STREAMABLE(MotionSelectionBH,
{
public:
  ENUM(ActivationMode,
    deactive,
    active,
    first
  ),

  (MotionRequestBH, Motion)(specialAction) targetMotion, /**< The motion that is the destination of the current interpolation. */
  (ActivationMode)(active) specialActionMode, /**< Whether and how the special action module is active. */
  (float[MotionRequestBH::numOfMotions]) ratios, /**< The current ratio of each motion in the final joint request. */
  (SpecialActionRequest) specialActionRequest, /**< The special action request, if it is an active motion. */

  // Initialization
  memset(ratios, 0, sizeof(ratios));
  ratios[MotionRequestBH::specialAction] = 1;
});
