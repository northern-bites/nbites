/**
 * @file Representations/Modeling/FreePartOfOpponentGoalModelBH.cpp
 * Implementation of a debug drawing of the free part of opponent goal model
 * @author Colin Graf
 */

#include "FreePartOfOpponentGoalModel.h"
#include "Tools/Debugging/DebugDrawings.h"

void FreePartOfOpponentGoalModelBH::draw() const
{
  DECLARE_DEBUG_DRAWING("representation:FreePartOfOpponentGoalModelBH", "drawingOnField");
  LINE("representation:FreePartOfOpponentGoalModelBH", leftEnd.x, leftEnd.y, rightEnd.x, rightEnd.y, 50, Drawings::ps_solid, ColorRGBA(255, 255, 255));
}
