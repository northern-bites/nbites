/**
* @file Representations/MotionControl/MotionSelection.cpp
* This file implements a class that represents the motions actually selected based on the constraints given.
* @author <A href="mailto:Thomas.Roefer@dfki.de">Thomas Röfer</A>
*/

#include <cstring>

#include "MotionSelection.h"

MotionSelection::MotionSelection() : targetMotion(MotionRequest::specialAction), specialActionMode(active)
{
  memset(ratios, 0, sizeof(ratios));
  ratios[MotionRequest::specialAction] = 1;
}
