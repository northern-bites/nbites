/**
* @file Representations/MotionControl/RobotBalance.h
* This file declares a class that represents information about the robot balance.
* @author Felix Wenk
*/

#pragma once

#include "Tools/Math/Vector2.h"
#include "Tools/Streams/AutoStreamable.h"

/**
* @class RobotBalanceBH
* A class that represents information about the robot balance.
*/
STREAMABLE(RobotBalanceBH,
{
public:
  enum {maxZmpPreview = 100}, /**< Maximum number of previewed ZMPs sent in this representation. */

  (int)(1) numZmpPreview, /**< Number of actually previewed ZMPs. */
  (Vector2BH<>[maxZmpPreview]) zmpPreview, /**< Previewed ZMPs. zmpPreview[0] is the currently demanded ZMP. */
  (Vector2BH<>) zmp, /**< The currently estimated zero-moment point. */
  (bool)(false) leftSupport, /**< True of the robot stands on its left foot. */
});
