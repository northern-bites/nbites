/**
* @file BallPercept.h
*
* Very simple representation of a seen ball
*
* @author <a href="mailto:timlaue@informatik.uni-bremen.de">Tim Laue</a>
*/

#pragma once

#include "Tools/Streams/AutoStreamable.h"
#include "Tools/Math/Vector2.h"

STREAMABLE(BallPerceptBH,
{
public:
  /** Draws the ball*/
  void draw() const,

  (Vector2BH<>) positionInImage,         /**< The position of the ball in the current image */
  (float) radiusInImage,               /**< The radius of the ball in the current image */
  (bool)(false) ballWasSeen,           /**< Indicates, if the ball was seen in the current image. */
  (Vector2BH<>) relativePositionOnField, /**< Ball position relative to the robot. */
});
