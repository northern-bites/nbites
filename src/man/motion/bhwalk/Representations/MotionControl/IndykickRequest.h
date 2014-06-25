/**
* @file Representations/MotionControl/IndykickRequest.h
* @author Felix Wenk
*/

#pragma once

#include "Tools/Enum.h"
#include "Tools/Math/Vector2.h"
#include "Tools/Streams/AutoStreamable.h"

STREAMABLE(IndykickRequest,
{
public:
  /** Motions supported by the kick engine. */
  ENUM(MotionId,
    bsplineKick,
    none
  );

  ENUM(SupportLeg,
    left,
    right,
    unspecified
  ),

  (MotionId)(none) motion,
  (SupportLeg)(unspecified) supportLeg,
  (Vector2BH<>) ballPosition,
  (Vector2BH<>) kickDirection,
  (unsigned)(750) kickDuration, /**< Time duration of the kick trajectory. */
});
