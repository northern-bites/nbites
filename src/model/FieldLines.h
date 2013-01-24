/*
 * Defines concrete field lines.
 */

#pragma once

#include "FieldCorners.h"

struct FieldLine {
    const FieldCorner corner1;
    const FieldCorner corner2;
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



