#ifndef FieldConstantsDefined__h
#define FieldConstantsDefined__h

#include <cmath>
#include "Common.h"

// Nao field constants
#ifdef USING_LAB_FIELD
static const float FIELD_WHITE_WIDTH = 330.f;
static const float FIELD_WHITE_HEIGHT = 500.f;
static const float GREEN_PAD_X = 15.f;
static const float GREEN_PAD_Y = 20.f;
#else
static const float FIELD_WHITE_WIDTH = 405.f;
static const float FIELD_WHITE_HEIGHT = 605.f;
static const float GREEN_PAD_X = 67.5f;
static const float GREEN_PAD_Y = 67.5f;
#endif // USING_LAB_FIELD

static const float FIELD_GREEN_WIDTH = FIELD_WHITE_WIDTH + 2.0f * GREEN_PAD_X;
static const float FIELD_GREEN_HEIGHT = FIELD_WHITE_HEIGHT + 2.0f * GREEN_PAD_Y;
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

// GOAL CONSTANTS
// my left post is left of goalie defending my goal facing the opponent
static const float LANDMARK_MY_GOAL_LEFT_POST_X = CENTER_FIELD_X - 70;
static const float LANDMARK_MY_GOAL_RIGHT_POST_X = CENTER_FIELD_X + 70;
static const float LANDMARK_OPP_GOAL_LEFT_POST_X = CENTER_FIELD_X - 70;
static const float LANDMARK_OPP_GOAL_RIGHT_POST_X = CENTER_FIELD_X + 70;

// measure to the center of the posts, 5 cm off the line
static const float LANDMARK_MY_GOAL_LEFT_POST_Y =
    FIELD_WHITE_BOTTOM_SIDELINE_Y-5;
static const float LANDMARK_MY_GOAL_RIGHT_POST_Y =
    FIELD_WHITE_BOTTOM_SIDELINE_Y-5;
static const float LANDMARK_OPP_GOAL_LEFT_POST_Y =
    FIELD_WHITE_TOP_SIDELINE_Y+5;
static const float LANDMARK_OPP_GOAL_RIGHT_POST_Y =
    FIELD_WHITE_TOP_SIDELINE_Y+5;

static const float CENTER_CIRCLE_RADIUS = 62.5f; // Not scaled

#ifdef USING_LAB_FIELD
static const float CENTER_CIRCLE_RADIUS = 62.5f;
static const float GOALBOX_HEIGHT = 60.f;
static const float GOALBOX_WIDTH = 200.f;
#else
static const float GOALBOX_HEIGHT = 65.f;
static const float GOALBOX_WIDTH = 305.f;
#endif // USING_LAB_FIELD
static const float MIDFIELD_X = FIELD_GREEN_WIDTH * .5f;
static const float MIDFIELD_Y = FIELD_GREEN_HEIGHT * .5f;

static const float GOALBOX_TOP_Y = GOALBOX_HEIGHT + GREEN_PAD_Y;
static const float GOALBOX_LEFT_X = MIDFIELD_X - GOALBOX_WIDTH * .5f;
static const float GOALBOX_RIGHT_X = MIDFIELD_X + GOALBOX_WIDTH * .5f;

static const float OPP_GOAL_HEADING = 0;
static const float MY_GOAL_HEADING = 180;

// my goal box constants relative to (0,0) on my team
static const float MY_GOALBOX_LEFT_X = MIDFIELD_X - GOALBOX_WIDTH * .5f;
static const float MY_GOALBOX_RIGHT_X = MIDFIELD_X + GOALBOX_WIDTH * .5f;
 // bottom as in closest to (0,0)
static const float MY_GOALBOX_BOTTOM_Y = GREEN_PAD_Y;
static const float MY_GOALBOX_TOP_Y = GREEN_PAD_Y + GOALBOX_HEIGHT;

// opp goal box constants relative to (0,0) on my team
static const float OPP_GOALBOX_LEFT_X = MIDFIELD_X - GOALBOX_WIDTH * .5f;
static const float OPP_GOALBOX_RIGHT_X = MIDFIELD_X + GOALBOX_WIDTH * .5f;
static const float OPP_GOALBOX_BOTTOM_Y = (FIELD_GREEN_HEIGHT-
                                            GREEN_PAD_Y-GOALBOX_HEIGHT);
static const float OPP_GOALBOX_TOP_Y = FIELD_GREEN_HEIGHT-GREEN_PAD_Y;

// Other Field object dimensions
static const float GOAL_POST_CM_HEIGHT = 80.0f;
static const float GOAL_POST_CM_WIDTH = 10.0f;
static const float CROSSBAR_CM_WIDTH = 140.f;
static const float CROSSBAR_CM_HEIGHT = 5.0f;
#endif // File
