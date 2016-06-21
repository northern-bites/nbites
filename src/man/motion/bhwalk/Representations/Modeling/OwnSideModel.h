/**
 * @file OwnSideModel.h
 * The file implements a model that states that the robot cannot have left its own
 * side since the last kick-off and how far it can have gotten along the field.
 *
 * @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
 */

#pragma once

#include "Tools/Streams/AutoStreamable.h"

/**
 * @class OwnSideModelBH
 * A model that states that the robot cannot have left its own
 * side since the last kick-off and how far it can have gotten along
 * the field.
 */
STREAMABLE(OwnSideModelBH,
{,
  (bool)(false) stillInOwnSide, /**< The robot must still be in its own side. */
  (float)(100000.f) largestXPossible, /**< The largest x-coordinate that is currently possible. */
  (bool)(false) returnFromGameControllerPenalty, /**< The robot was unpenalized by the GameController and believes it. */
  (bool)(false) returnFromManualPenalty, /**< The robot was unpenalized by the GameController and believes it. */
});
