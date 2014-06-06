/**
 * @file SubPhaseParameters.h
 * This file declares a parameter set to separate a phase in three arbitrary parts
 * @author Colin Graf
 */

#pragma once

#include "Tools/Streams/AutoStreamable.h"

/**
 * A parameter set to separate a phase in three arbitrary parts
 */
STREAMABLE(SubPhaseParameters,
{
public:
  SubPhaseParameters(float s, float d) { start = s; duration = d; }
,
  (float)(0) start, /**< The start position of the second sub phase */
  (float)(1) duration, /**< The length of the second sub phase */
});
