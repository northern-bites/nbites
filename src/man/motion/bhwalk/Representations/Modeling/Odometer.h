/*
 * @file Odometer.h
 *
 * Some additional odometry information
 *
 * @author <a href="mailto:Tim.Laue@dfki.de">Tim Laue</a>
 * @author marcel
 */

#pragma once

#include "Tools/Math/Pose2D.h"
#include "Tools/Streams/AutoStreamable.h"

STREAMABLE(OdometerBH,
{,
  (float)(10000.f) distanceWalked,  /** Total distance walked since start of B-Human software */
  (Pose2DBH) odometryOffset, /** Odometry difference since last frame */
});
