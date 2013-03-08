/*
 * Contains all constants for Python. Replaces NogginConstants.py so that
 * python constants will update when C++ constants are updated. Passed through
 * to python via the noggin_constants boost module in PyConstants.*. Can be
 * accessed by using namespace noggin::py_constants so as not to clutter
 * global namespace. Many of these constants rely on others from man/include.
 * Those with PY_ in front of the name are called without the PY_ from python.
 * This was added to fix conflicts arising from having them #define(d) in
 * other constants files. Note that if you change these constants, they will
 * also change in python, but if you add one, you also need to wrap it!
 */

#ifndef PyNogginConstants_h_DEFINED
#define PyNogginConstants_h_DEFINED

#include "VisionDef.h"
#include "FieldConstants.h"
#include <vector>

namespace noggin {
namespace py_constants {

///// SWITCH BOARD /////
static const bool LOG_LOC = false;

///// WALK TIME CONSTANTS /////
static const float TIME_STEP = 40.0;
static const float TIME_PER_STEP = TIME_STEP/float(1000.0);

///// TEAM STUFF ////
static const int LENGTH_OF_HALF = 600; //seconds

// Setup colors
static const int NUM_GAME_TEAM_COLORS = 2;

enum teamColor{
    PY_TEAM_BLUE = 0,
    PY_TEAM_RED
};

///// VISION CONNECTION /////
static const int CAMERA_FPS = 15;

static const int IMAGE_CENTER_X = int(IMAGE_WIDTH/2.0);

static const float IMAGE_ANGLE_X = IMAGE_WIDTH/FOV_X_DEG;
static const float IMAGE_ANGLE_Y = IMAGE_WIDTH/FOV_Y_DEG;

///// BALL /////
static const int NUM_TOTAL_BALL_VALUES = 27;

///// LOCALIZATION CONSTANTS /////
static const float OPP_GOAL_HEADING = 0.0;
static const float MY_GOAL_HEADING = 180.0;

// my left post is the left of goalie defending my goal, facing opponent
static const float LANDMARK_MY_GOAL_LEFT_POST_X =
    FIELD_WHITE_LEFT_SIDELINE_X + GOAL_POST_RADIUS;
static const float LANDMARK_MY_GOAL_RIGHT_POST_X =
    FIELD_WHITE_LEFT_SIDELINE_X + GOAL_POST_RADIUS;
static const float LANDMARK_OPP_GOAL_LEFT_POST_X =
    FIELD_WHITE_RIGHT_SIDELINE_X - GOAL_POST_RADIUS;
static const float LANDMARK_OPP_GOAL_RIGHT_POST_X =
    FIELD_WHITE_RIGHT_SIDELINE_X - GOAL_POST_RADIUS;

//measure to the center of the posts, 5 cm off the line
static const float LANDMARK_MY_GOAL_LEFT_POST_Y =
    CENTER_FIELD_Y + CROSSBAR_CM_WIDTH / 2.0;
static const float LANDMARK_MY_GOAL_RIGHT_POST_Y =
    CENTER_FIELD_Y - CROSSBAR_CM_WIDTH / 2.0;
static const float LANDMARK_OPP_GOAL_RIGHT_POST_Y =
    CENTER_FIELD_Y - CROSSBAR_CM_WIDTH / 2.0;
static const float LANDMARK_OPP_GOAL_LEFT_POST_Y =
    CENTER_FIELD_Y + CROSSBAR_CM_WIDTH / 2.0;

static const float GOAL_WIDTH = LANDMARK_MY_GOAL_LEFT_POST_Y -
    LANDMARK_MY_GOAL_RIGHT_POST_Y;

static const float OUTSIDE_GOALBOX_Y = MIDFIELD_Y - GOALBOX_WIDTH/2.0;

// my goal box constants relative to (0,0) on my team
static const float MY_GOALBOX_LEFT_X = GREEN_PAD_X;
static const float MY_GOALBOX_RIGHT_X = GREEN_PAD_X + GOALBOX_DEPTH;
static const float MY_GOALBOX_BOTTOM_Y = MIDFIELD_Y -
    GOALBOX_WIDTH / 2.0;
static const float MY_GOALBOX_TOP_Y = MIDFIELD_Y + GOALBOX_WIDTH / 2.0;
static const float MY_GOALBOX_MIDDLE_Y =
    (MY_GOALBOX_BOTTOM_Y + MY_GOALBOX_TOP_Y) / 2.0;

//opp goal box constants relative to (0,0) on my team
static const float OPP_GOALBOX_LEFT_X = FIELD_WHITE_RIGHT_SIDELINE_X -
    GOALBOX_DEPTH;
static const float OPP_GOALBOX_RIGHT_X = FIELD_WHITE_RIGHT_SIDELINE_X;
static const float OPP_GOALBOX_BOTTOM_Y = MIDFIELD_Y -
    GOALBOX_WIDTH / 2.0;
static const float OPP_GOALBOX_TOP_Y = MIDFIELD_Y + GOALBOX_WIDTH / 2.0;
static const float OPP_GOALBOX_MIDDLE_Y = (OPP_GOALBOX_BOTTOM_Y +
                                           OPP_GOALBOX_TOP_Y) / 2.0;
static const float OPP_GOAL_MIDPOINT = (OPP_GOALBOX_RIGHT_X,
                                        OPP_GOALBOX_MIDDLE_Y);

///// LANDMARKS //////
static const int NUM_LANDMARKS = 19;

enum landmarkID {
    LANDMARK_MY_GOAL_LEFT_POST_ID = 0,
    LANDMARK_MY_GOAL_RIGHT_POST_ID,
    LANDMARK_OPP_GOAL_LEFT_POST_ID,
    LANDMARK_OPP_GOAL_RIGHT_POST_ID,
    LANDMARK_BALL_ID,
    LANDMARK_MY_CORNER_LEFT_L_ID,
    LANDMARK_MY_CORNER_RIGHT_L_ID,
    LANDMARK_MY_GOAL_LEFT_T_ID,
    LANDMARK_MY_GOAL_RIGHT_T_ID,
    LANDMARK_MY_GOAL_LEFT_L_ID,
    LANDMARK_MY_GOAL_RIGHT_L_ID,
    LANDMARK_CENTER_LEFT_T_ID,
    LANDMARK_CENTER_RIGHT_T_ID,
    LANDMARK_OPP_CORNER_LEFT_L_ID,
    LANDMARK_OPP_CORNER_RIGHT_L_ID,
    LANDMARK_OPP_GOAL_LEFT_T_ID,
    LANDMARK_OPP_GOAL_RIGHT_T_ID,
    LANDMARK_OPP_GOAL_LEFT_L_ID,
    LANDMARK_OPP_GOAL_RIGHT_L_ID
};

static const int LANDMARK_MY_GOAL_LEFT_POST[3] =
{(int) LANDMARK_MY_GOAL_LEFT_POST_X,
 (int) LANDMARK_MY_GOAL_LEFT_POST_Y,
 LANDMARK_MY_GOAL_LEFT_POST_ID};

static const int LANDMARK_MY_GOAL_RIGHT_POST[3] =
{(int) LANDMARK_MY_GOAL_RIGHT_POST_X,
 (int) LANDMARK_MY_GOAL_RIGHT_POST_Y,
 LANDMARK_MY_GOAL_RIGHT_POST_ID};

static const int LANDMARK_OPP_GOAL_LEFT_POST[3] =
{(int) LANDMARK_OPP_GOAL_LEFT_POST_X,
 (int) LANDMARK_OPP_GOAL_LEFT_POST_Y,
 LANDMARK_OPP_GOAL_LEFT_POST_ID};

static const int LANDMARK_OPP_GOAL_RIGHT_POST[3] =
{(int) LANDMARK_OPP_GOAL_RIGHT_POST_X,
 (int) LANDMARK_OPP_GOAL_RIGHT_POST_Y,
 LANDMARK_OPP_GOAL_RIGHT_POST_ID};

static const int NUM_VIS_LANDMARKS = 6;

enum vis_landmark {
    DEFAULT = 0,
    VISION_YGLP,
    VISION_YGRP,
    VISION_BGLP,
    VISION_BGRP,
    VISION_BG_CROSSBAR,
    VISION_YG_CROSSBAR
};


static const int LANDMARK_MY_FIELD_CROSS[2] =
{(int) (FIELD_WHITE_LEFT_SIDELINE_X + LINE_CROSS_OFFSET),
 (int) MIDFIELD_Y};

static const int LANDMARK_OPP_FIELD_CROSS[2] =
{(int) (FIELD_WHITE_RIGHT_SIDELINE_X - LINE_CROSS_OFFSET),
 (int) MIDFIELD_Y};

static const int NUM_LOC_SCORES = 3;

enum locScore{
    BAD_LOC = 0,
    OK_LOC,
    GOOD_LOC
};

static const int GOOD_LOC_XY_UNCERT_THRESH = 50;
static const int GOOD_LOC_THETA_UNCERT_THRESH = 20;

static const int OK_LOC_XY_UNCERT_THRESH = 90;
static const int OK_LOC_THETA_UNCERT_THRESH = 30;

static const int BAD_LOC_XY_UNCERT_THRESH = 200;
static const int BAD_LOC_THETA_UNCERT_THRESH = 40;

static const int TOP_OPP_CORNER_SLOPE = -1;
static const int BOTTOM_OPP_CORNER_SLOPE = 1;

static const float OPP_CORNER_LEFT_X = FIELD_WIDTH - 100.0;
static const float TOP_OPP_CORNER_Y = FIELD_HEIGHT;

static const float FACING_SIDELINE_ANGLE = 30.0;

///// MESSAGES /////
static const int GAME_STATE_IN = 0;
static const int FILTERED_BALL_IN = 1;
// This should always be the last index
// Uses NUM_PLAYERS_PER_TEAM indices in a row
static const int WORLD_MODEL_IN = 2;

///// ROBOTS /////
static const int NUM_POSSIBLE_ROBOTS = 6;

enum robotID{
    RED_1 = 0,
    RED_2,
    RED_3,
    BLUE_1,
    BLUE_2,
    BLUE_3
};
}
}

#endif //PyNogginConstants_h_DEFINED
