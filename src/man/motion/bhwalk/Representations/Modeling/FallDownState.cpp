/**
 * @file Representations/Modeling/FallDownStateBH.cpp
 *
 * Implementation of a debug drawing for the FallDownStateBH.
 *
 * @author Carsten Könemann
 */

#include "FallDownState.h"
#include "Tools/Debugging/DebugDrawings.h"

void FallDownStateBH::draw()
{
  DECLARE_DEBUG_DRAWING("representation:FallDownStateBH", "drawingOnImage");
  // text-shadow for better visibility
  DRAWTEXT("representation:FallDownStateBH", 26, 26, 35, ColorClasses::black, "State: " << getName(state));
  DRAWTEXT("representation:FallDownStateBH", 26, 51, 35, ColorClasses::black, "Direction: " << getName(direction));
  DRAWTEXT("representation:FallDownStateBH", 26, 76, 35, ColorClasses::black, "Sidewards: " << getName(sidewards));
  // text
  DRAWTEXT("representation:FallDownStateBH", 25, 25, 35, ColorClasses::white, "State: " << getName(state));
  DRAWTEXT("representation:FallDownStateBH", 25, 50, 35, ColorClasses::white, "Direction: " << getName(direction));
  DRAWTEXT("representation:FallDownStateBH", 25, 75, 35, ColorClasses::white, "Sidewards: " << getName(sidewards));
}
