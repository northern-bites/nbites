/**
* @file GroundContactState.h
* Declaration of class GroundContactStateBH.
* @author Colin Graf
*/

#pragma once

#include "Tools/Streams/AutoStreamable.h"

/**
* @class GroundContactStateBH
* Describes whether we got contact with ground or not.
*/
STREAMABLE(GroundContactStateBH,
{,
  (bool)(true) contact, /**< a foot of the robot touches the ground */
});
