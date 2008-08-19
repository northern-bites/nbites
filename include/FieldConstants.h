#ifndef FieldConstantsDefined__h
#define FieldConstantsDefined__h

#include <cmath>
#include "Common.h"

// Nao field constants
static const double LAB_SCALE = 0; // What is our lab scale?  Shrug
static const double OFFICIAL_SCALE = 1.0;
static const double FIELD_SCALE = OFFICIAL_SCALE;

static const double FIELD_WHITE_WIDTH = 400. * FIELD_SCALE;
static const double FIELD_WHITE_HEIGHT = 600. * FIELD_SCALE;
static const double FIELD_GREEN_WIDTH = 540. * FIELD_SCALE;
static const double FIELD_GREEN_HEIGHT = 740. * FIELD_SCALE;
static const double FIELD_WIDTH = FIELD_GREEN_WIDTH;
static const double FIELD_HEIGHT = FIELD_GREEN_HEIGHT;

static const double CENTER_FIELD_X = FIELD_GREEN_WIDTH / 2;
static const double CENTER_FIELD_Y = FIELD_GREEN_HEIGHT / 2;

static const double FIELD_GREEN_LEFT_SIDELINE_X = 0;
static const double FIELD_GREEN_RIGHT_SIDELINE_X = FIELD_GREEN_WIDTH;
static const double FIELD_GREEN_BOTTOM_SIDELINE_Y = 0;
static const double FIELD_GREEN_TOP_SIDELINE_Y = FIELD_GREEN_HEIGHT;

static const double GREEN_PAD_Y = 70. * FIELD_SCALE;
static const double GREEN_PAD_X = 70. * FIELD_SCALE;

static const double FIELD_WHITE_BOTTOM_SIDELINE_Y = GREEN_PAD_Y;
static const double FIELD_WHITE_TOP_SIDELINE_Y = (FIELD_WHITE_HEIGHT +
                                                  GREEN_PAD_Y);
static const double FIELD_WHITE_LEFT_SIDELINE_X = GREEN_PAD_X;
static const double FIELD_WHITE_RIGHT_SIDELINE_X = (FIELD_WHITE_WIDTH +
                                                    GREEN_PAD_X);

// GOAL CONSTANTS
// my left post is left of goalie defending my goal facing the opponent
static const double LANDMARK_MY_GOAL_LEFT_POST_X = CENTER_FIELD_X - 70;
static const double LANDMARK_MY_GOAL_RIGHT_POST_X = CENTER_FIELD_X + 70;
static const double LANDMARK_OPP_GOAL_LEFT_POST_X = CENTER_FIELD_X - 70;
static const double LANDMARK_OPP_GOAL_RIGHT_POST_X = CENTER_FIELD_X + 70;

// measure to the center of the posts, 5 cm off the line
static const double LANDMARK_MY_GOAL_LEFT_POST_Y =
    FIELD_WHITE_BOTTOM_SIDELINE_Y-5.0;
static const double LANDMARK_MY_GOAL_RIGHT_POST_Y =
    FIELD_WHITE_BOTTOM_SIDELINE_Y-5.0;
static const double LANDMARK_OPP_GOAL_LEFT_POST_Y =
    FIELD_WHITE_TOP_SIDELINE_Y+5.0;
static const double LANDMARK_OPP_GOAL_RIGHT_POST_Y =
    FIELD_WHITE_TOP_SIDELINE_Y+5.0;

static const double CENTER_CIRCLE_RADIUS = 60.; // not scaled

static const double GOALBOX_HEIGHT = 50.; // not scaled
static const double GOALBOX_WIDTH = 300.; // not scaled

static const double MIDFIELD_X = FIELD_GREEN_WIDTH / 2.;
static const double MIDFIELD_Y = FIELD_GREEN_HEIGHT / 2.;

static const double GOALBOX_TOP_Y = GOALBOX_HEIGHT + GREEN_PAD_Y;
static const double GOALBOX_LEFT_X = MIDFIELD_X - GOALBOX_WIDTH / 2.;
static const double GOALBOX_RIGHT_X = MIDFIELD_X + GOALBOX_WIDTH / 2.;

static const double OPP_GOAL_HEADING = 0.;
static const double MY_GOAL_HEADING = 180.;

// my goal box constants relative to (0,0) on my team
static const double MY_GOALBOX_LEFT_X = MIDFIELD_X - GOALBOX_WIDTH / 2.;
static const double MY_GOALBOX_RIGHT_X = MIDFIELD_X + GOALBOX_WIDTH / 2.;
 // bottom as in closest to (0,0)
static const double MY_GOALBOX_BOTTOM_Y = GREEN_PAD_Y;
static const double MY_GOALBOX_TOP_Y = GREEN_PAD_Y + GOALBOX_HEIGHT;

// opp goal box constants relative to (0,0) on my team
static const double OPP_GOALBOX_LEFT_X = MIDFIELD_X - GOALBOX_WIDTH / 2.;
static const double OPP_GOALBOX_RIGHT_X = MIDFIELD_X + GOALBOX_WIDTH / 2.;
static const double OPP_GOALBOX_BOTTOM_Y = (FIELD_GREEN_HEIGHT-
                                            GREEN_PAD_Y-GOALBOX_HEIGHT);
static const double OPP_GOALBOX_TOP_Y = FIELD_GREEN_HEIGHT-GREEN_PAD_Y;

#endif // File
