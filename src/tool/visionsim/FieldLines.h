/*
 * Defines concrete field lines.
 */

#pragma once

#include "FieldCorners.h"

namespace tool{
namespace visionsim{

struct FieldLine
{
    FieldCorner corner1;
    FieldCorner corner2;
};

// About lines
static const int NUM_GREEN_LINES = 4;
static const int NUM_FIELD_LINES = 11;
static const int NUM_LINES = 15;

// Green field edges
static const FieldLine BOTTOM_GREEN = {BOTTOM_LEFT_GREEN,
                                       BOTTOM_RIGHT_GREEN};

static const FieldLine TOP_GREEN = {TOP_LEFT_GREEN,
                                    TOP_RIGHT_GREEN};

static const FieldLine LEFT_GREEN = {BOTTOM_LEFT_GREEN,
                                     TOP_LEFT_GREEN};

static const FieldLine RIGHT_GREEN = {BOTTOM_RIGHT_GREEN,
                                     TOP_RIGHT_GREEN};

// White field lines
static const FieldLine TOP_SIDELINE = {TOP_LEFT_FIELD_L,
                                       TOP_RIGHT_FIELD_L};

static const FieldLine BOTTOM_SIDELINE = {BOTTOM_LEFT_FIELD_L,
                                          BOTTOM_RIGHT_FIELD_L};

static const FieldLine LEFT_ENDLINE = {TOP_LEFT_FIELD_L,
                                       BOTTOM_LEFT_FIELD_L};

static const FieldLine RIGHT_ENDLINE = {TOP_RIGHT_FIELD_L,
                                       BOTTOM_RIGHT_FIELD_L};

static const FieldLine LEFT_BOX_TOP = {TOP_LEFT_GOAL_T,
                                       TOP_LEFT_GOALBOX_L};

static const FieldLine LEFT_BOX_BOTTOM = {BOTTOM_LEFT_GOAL_T,
                                          BOTTOM_LEFT_GOALBOX_L};

static const FieldLine LEFT_BOX_FRONT = {TOP_LEFT_GOALBOX_L,
                                         BOTTOM_LEFT_GOALBOX_L};

static const FieldLine RIGHT_BOX_TOP = {TOP_RIGHT_GOAL_T,
                                       TOP_RIGHT_GOALBOX_L};

static const FieldLine RIGHT_BOX_BOTTOM = {BOTTOM_RIGHT_GOAL_T,
                                          BOTTOM_RIGHT_GOALBOX_L};

static const FieldLine RIGHT_BOX_FRONT = {TOP_RIGHT_GOALBOX_L,
                                         BOTTOM_RIGHT_GOALBOX_L};

static const FieldLine CENTER_LINE = {TOP_CENTER_T,
                                      BOTTOM_CENTER_T};

// Helpful arrays
static const FieldLine GREEN_LINES[NUM_GREEN_LINES] =
{ BOTTOM_GREEN,
  TOP_GREEN,
  LEFT_GREEN,
  RIGHT_GREEN };

static const FieldLine FIELD_LINES[NUM_FIELD_LINES] =
{ TOP_SIDELINE,
  BOTTOM_SIDELINE,
  LEFT_ENDLINE,
  RIGHT_ENDLINE,
  LEFT_BOX_TOP,
  LEFT_BOX_BOTTOM,
  LEFT_BOX_FRONT,
  RIGHT_BOX_TOP,
  RIGHT_BOX_BOTTOM,
  RIGHT_BOX_FRONT,
  CENTER_LINE };

static const FieldLine ALL_LINES[NUM_LINES] =
{ BOTTOM_GREEN,
  TOP_GREEN,
  LEFT_GREEN,
  RIGHT_GREEN,
  TOP_SIDELINE,
  BOTTOM_SIDELINE,
  LEFT_ENDLINE,
  RIGHT_ENDLINE,
  LEFT_BOX_TOP,
  LEFT_BOX_BOTTOM,
  LEFT_BOX_FRONT,
  RIGHT_BOX_TOP,
  RIGHT_BOX_BOTTOM,
  RIGHT_BOX_FRONT,
  CENTER_LINE };

}
}
