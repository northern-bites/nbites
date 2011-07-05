#ifndef FieldConstantsDefined__h
#define FieldConstantsDefined__h

#include <cmath>
#include "Common.h"
#include "manconfig.h"

// Nao field constants

#ifdef USING_LAB_FIELD
static const float FIELD_WHITE_WIDTH = 500.f;
static const float FIELD_WHITE_HEIGHT = 330.f;
static const float GREEN_PAD_X = 20.f;
static const float GREEN_PAD_Y = 15.f;
#else
static const float FIELD_WHITE_WIDTH = 600.f;
static const float FIELD_WHITE_HEIGHT = 400.f;
static const float GREEN_PAD_X = 71.0f;
static const float GREEN_PAD_Y = 71.0f;
#endif // USING_LAB_FIELD

static const float FIELD_GREEN_WIDTH = FIELD_WHITE_WIDTH + 2.0f * GREEN_PAD_Y;
static const float FIELD_GREEN_HEIGHT = FIELD_WHITE_HEIGHT + 2.0f * GREEN_PAD_X;
static const float FIELD_WIDTH = FIELD_GREEN_WIDTH;
static const float FIELD_HEIGHT = FIELD_GREEN_HEIGHT;

static const float CENTER_FIELD_X = FIELD_GREEN_WIDTH * .5f;
static const float CENTER_FIELD_Y = FIELD_GREEN_HEIGHT * .5f;

static const float FIELD_GREEN_LEFT_SIDELINE_X = 0;
static const float FIELD_GREEN_RIGHT_SIDELINE_X = FIELD_GREEN_WIDTH;
static const float FIELD_GREEN_BOTTOM_SIDELINE_Y = 0;
static const float FIELD_GREEN_TOP_SIDELINE_Y = FIELD_GREEN_HEIGHT;

static const float FIELD_WHITE_BOTTOM_SIDELINE_Y = GREEN_PAD_Y;
static const float FIELD_WHITE_TOP_SIDELINE_Y = (FIELD_WHITE_HEIGHT +
                                                  GREEN_PAD_Y);
static const float FIELD_WHITE_LEFT_SIDELINE_X = GREEN_PAD_X;
static const float FIELD_WHITE_RIGHT_SIDELINE_X = (FIELD_WHITE_WIDTH +
                                                    GREEN_PAD_X);

static const float MIDFIELD_X = FIELD_GREEN_WIDTH * .5f;
static const float MIDFIELD_Y = FIELD_GREEN_HEIGHT * .5f;

// Other Field object dimensions
static const float GOAL_POST_CM_HEIGHT = 80.0f;
static const float GOAL_POST_CM_WIDTH = 10.0f;
static const float CROSSBAR_CM_WIDTH = 140.f;
static const float CROSSBAR_CM_HEIGHT = 5.0f;
static const float GOAL_POST_RADIUS = GOAL_POST_CM_WIDTH / 2.0f;

// GOAL CONSTANTS
static const float LANDMARK_BLUE_GOAL_BOTTOM_POST_X =
    FIELD_WHITE_LEFT_SIDELINE_X + GOAL_POST_RADIUS;
static const float LANDMARK_BLUE_GOAL_TOP_POST_X =
    FIELD_WHITE_LEFT_SIDELINE_X + GOAL_POST_RADIUS;
static const float LANDMARK_YELLOW_GOAL_BOTTOM_POST_X =
    FIELD_WHITE_RIGHT_SIDELINE_X - GOAL_POST_RADIUS;
static const float LANDMARK_YELLOW_GOAL_TOP_POST_X =
    FIELD_WHITE_RIGHT_SIDELINE_X - GOAL_POST_RADIUS;

static const float LANDMARK_BLUE_GOAL_BOTTOM_POST_Y =
    CENTER_FIELD_Y - CROSSBAR_CM_WIDTH / 2.0f;
static const float LANDMARK_BLUE_GOAL_TOP_POST_Y =
    CENTER_FIELD_Y + CROSSBAR_CM_WIDTH / 2.0f;
static const float LANDMARK_YELLOW_GOAL_BOTTOM_POST_Y =
    CENTER_FIELD_Y - CROSSBAR_CM_WIDTH / 2.0f;
static const float LANDMARK_YELLOW_GOAL_TOP_POST_Y =
    CENTER_FIELD_Y + CROSSBAR_CM_WIDTH / 2.0f;

static const float CENTER_CIRCLE_RADIUS = 60.0f; // Not scaled

#ifdef USING_LAB_FIELD
static const float GOALBOX_DEPTH = 60.f;
static const float GOALBOX_WIDTH = 200.f;
#else
static const float GOALBOX_DEPTH = 60.f;
static const float GOALBOX_WIDTH = 220.f;
#endif // USING_LAB_FIELD


// These are used by the vision system
// The distance the goalie box extends out past each goal post
static const float GOALBOX_OVERAGE = 40.0f;
// The distance from any goal post to the goalie box corner nearest it
static const float POST_CORNER = sqrtf(GOALBOX_DEPTH * GOALBOX_DEPTH +
                                       GOALBOX_OVERAGE * GOALBOX_OVERAGE);

static const float YELLOW_GOAL_HEADING = 0;
static const float BLUE_GOAL_HEADING = 180;

// my goal box constants relative to (0,0) on my team
static const float BLUE_GOALBOX_TOP_Y = MIDFIELD_Y + GOALBOX_WIDTH * .5f;
static const float BLUE_GOALBOX_BOTTOM_Y = MIDFIELD_Y - GOALBOX_WIDTH * .5f;
 // bottom as in closest to (0,0)
static const float BLUE_GOALBOX_LEFT_X = GREEN_PAD_X;
static const float BLUE_GOALBOX_RIGHT_X = GREEN_PAD_X + GOALBOX_DEPTH;

// opp goal box constants relative to (0,0) on my team
static const float YELLOW_GOALBOX_BOTTOM_Y = MIDFIELD_Y - GOALBOX_WIDTH * .5f;
static const float YELLOW_GOALBOX_TOP_Y = MIDFIELD_Y + GOALBOX_WIDTH * .5f;
static const float YELLOW_GOALBOX_LEFT_X =
    FIELD_WHITE_RIGHT_SIDELINE_X - GOALBOX_DEPTH;
static const float YELLOW_GOALBOX_RIGHT_X = FIELD_WHITE_RIGHT_SIDELINE_X;

// Crosses on the Field
#ifdef USING_LAB_FIELD
static const float LINE_CROSS_OFFSET = 130;
#else
static const float LINE_CROSS_OFFSET = 180;
#endif // USING_LAB_FIELD

static const float LANDMARK_BLUE_GOAL_CROSS_X = FIELD_WHITE_LEFT_SIDELINE_X +
    LINE_CROSS_OFFSET;
static const float LANDMARK_BLUE_GOAL_CROSS_Y = MIDFIELD_Y;
static const float LANDMARK_YELLOW_GOAL_CROSS_X = FIELD_WHITE_RIGHT_SIDELINE_X -
    LINE_CROSS_OFFSET;
static const float LANDMARK_YELLOW_GOAL_CROSS_Y = MIDFIELD_Y;

static const float CC_LINE_CROSS_X = MIDFIELD_X;
static const float CC_LINE_CROSS_Y = MIDFIELD_Y;

static const float TOP_CC_Y = CENTER_FIELD_Y + CENTER_CIRCLE_RADIUS;
static const float TOP_CC_X = CENTER_FIELD_X;
static const float BOTTOM_CC_Y = CENTER_FIELD_Y - CENTER_CIRCLE_RADIUS;
static const float BOTTOM_CC_X = CENTER_FIELD_X;

static const float LINE_CROSS_LENGTH = 10.0f; // length of each cross in cm

#endif // File
