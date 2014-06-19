/**
* @file OrientationData.h
* Declaration of class OrientationDataBH.
* @author Colin Graf
*/

#pragma once

#include "Tools/Math/RotationMatrix.h"
#include "Tools/Streams/AutoStreamable.h"

/**
* @class OrientationDataBH
* Encapsulates the orientation and velocity of the torso.
*/
STREAMABLE(OrientationDataBH,
{,
  (RotationMatrixBH) rotation, /**< The rotation of the torso. */
  (Vector3BH<>) velocity, /**< The velocity along the x-, y- and z-axis relative to the torso. (in m/s) */
});

class GroundTruthOrientationDataBH  : public OrientationDataBH {};
