#ifndef FieldConstantsDefined__h
#define FieldConstantsDefined__h

#include <cmath>
#include "Common.h"

// Just copied and changed to C++ stuff from Constants.py
#if ROBOT(AIBO)
static const double LAB_SCALE = 0.72;
static const double OFFICIAL_SCALE = 1.0;
static const double FIELD_SCALE = OFFICIAL_SCALE;

static const double FIELD_WHITE_WIDTH = 460. * FIELD_SCALE;
static const double FIELD_WHITE_HEIGHT = 690. * FIELD_SCALE;
static const double FIELD_GREEN_WIDTH = 500. * FIELD_SCALE;
static const double FIELD_GREEN_HEIGHT = 750. * FIELD_SCALE;
 
static const double FIELD_WIDTH = FIELD_GREEN_WIDTH;
static const double FIELD_HEIGHT = FIELD_GREEN_HEIGHT;

static const double CENTER_FIELD_X = FIELD_GREEN_WIDTH / 2;
static const double CENTER_FIELD_Y = FIELD_GREEN_HEIGHT / 2;

static const double LAB_CENTER_FIELD_X = CENTER_FIELD_X;
static const double LAB_CENTER_FIELD_Y = CENTER_FIELD_Y;

static const double FIELD_GREEN_LEFT_SIDELINE_X = 0;
static const double FIELD_GREEN_RIGHT_SIDELINE_X = FIELD_GREEN_WIDTH;
static const double FIELD_GREEN_BOTTOM_SIDELINE_Y = 0;
static const double FIELD_GREEN_TOP_SIDELINE_Y = FIELD_GREEN_HEIGHT;

static const double GREEN_PAD_Y = 30. * FIELD_SCALE;
static const double GREEN_PAD_X = 20. * FIELD_SCALE;

static const double FIELD_WHITE_BOTTOM_SIDELINE_Y = GREEN_PAD_Y;
static const double FIELD_WHITE_TOP_SIDELINE_Y = FIELD_WHITE_HEIGHT + GREEN_PAD_Y;
static const double FIELD_WHITE_LEFT_SIDELINE_X = GREEN_PAD_X;
static const double FIELD_WHITE_RIGHT_SIDELINE_X = FIELD_WHITE_WIDTH + GREEN_PAD_X;


static const double LANDMARK_RIGHT_BEACON_X = FIELD_GREEN_WIDTH - 5.;
static const double LANDMARK_RIGHT_BEACON_Y = FIELD_GREEN_HEIGHT / 2.;
static const double LANDMARK_LEFT_BEACON_X = 5.;
static const double LANDMARK_LEFT_BEACON_Y = FIELD_GREEN_HEIGHT / 2.;

  // Goals >= 2007
  // my left post is left of goalie defending my goal facing the opponent
static const double GOAL_WIDTH = 90.;
static const double LANDMARK_MY_GOAL_LEFT_POST_X = CENTER_FIELD_X - (GOAL_WIDTH / 2.);
static const double LANDMARK_MY_GOAL_RIGHT_POST_X = CENTER_FIELD_X + (GOAL_WIDTH / 2.);
static const double LANDMARK_OPP_GOAL_LEFT_POST_X = CENTER_FIELD_X - (GOAL_WIDTH / 2.);
static const double LANDMARK_OPP_GOAL_RIGHT_POST_X = CENTER_FIELD_X + (GOAL_WIDTH / 2.);

  // measure to the center of the posts, 5 cm off the line
static const double LANDMARK_MY_GOAL_LEFT_POST_Y = FIELD_WHITE_BOTTOM_SIDELINE_Y-5.0;
static const double LANDMARK_MY_GOAL_RIGHT_POST_Y = FIELD_WHITE_BOTTOM_SIDELINE_Y-5.0;
static const double LANDMARK_OPP_GOAL_LEFT_POST_Y = FIELD_WHITE_TOP_SIDELINE_Y+5.0;
static const double LANDMARK_OPP_GOAL_RIGHT_POST_Y = FIELD_WHITE_TOP_SIDELINE_Y+5.0;

  // For the backstops
static const double LANDMARK_OPP_GOAL_X = FIELD_WIDTH / 2.;
static const double LANDMARK_OPP_GOAL_Y = FIELD_HEIGHT;
static const double LANDMARK_MY_GOAL_X = FIELD_WIDTH / 2.;
static const double LANDMARK_MY_GOAL_Y = 0.;

static const double CENTER_CIRCLE_RADIUS = 18.; //# not scaled
static const double FIELD_ARC_RADIUS = 40.; //# Not to be scaled

static const double FIELD_ARC_CENTER_DIST = FIELD_ARC_RADIUS / sqrt(2.0);

static const double MY_YELLOW_ARC_X = FIELD_WHITE_LEFT_SIDELINE_X + FIELD_ARC_CENTER_DIST;
static const double MY_YELLOW_ARC_Y = FIELD_WHITE_BOTTOM_SIDELINE_Y + FIELD_ARC_CENTER_DIST;
static const double MY_BLUE_ARC_X = FIELD_WHITE_RIGHT_SIDELINE_X - FIELD_ARC_CENTER_DIST;
static const double MY_BLUE_ARC_Y = MY_YELLOW_ARC_Y;

static const double OPP_BLUE_ARC_X = MY_YELLOW_ARC_X;
static const double OPP_BLUE_ARC_Y = FIELD_WHITE_TOP_SIDELINE_Y - FIELD_ARC_CENTER_DIST;
static const double OPP_YELLOW_ARC_X = MY_BLUE_ARC_X;
static const double OPP_YELLOW_ARC_Y = OPP_BLUE_ARC_Y;

static const double GOALBOX_HEIGHT = 45.;
static const double GOALBOX_WIDTH = 130.; //# not scaled

static const double MIDFIELD_X = FIELD_GREEN_WIDTH / 2.;
static const double MIDFIELD_Y = FIELD_GREEN_HEIGHT / 2.;

static const double GOALBOX_TOP_Y = GOALBOX_HEIGHT + GREEN_PAD_Y;
static const double GOALBOX_LEFT_X = MIDFIELD_X - GOALBOX_WIDTH / 2.;
static const double GOALBOX_RIGHT_X = MIDFIELD_X + GOALBOX_WIDTH / 2.;

//# my goal box constants relative to (0,0) on my team
static const double MY_GOALBOX_LEFT_X = MIDFIELD_X - GOALBOX_WIDTH / 2.;
static const double MY_GOALBOX_RIGHT_X = MIDFIELD_X + GOALBOX_WIDTH / 2.;
static const double MY_GOALBOX_BOTTOM_Y = GREEN_PAD_Y; // bottom as in closest to (0,0)
static const double MY_GOALBOX_TOP_Y = GREEN_PAD_Y + GOALBOX_HEIGHT;

  //# opp goal box constants relative to (0,0) on my team
static const double OPP_GOALBOX_LEFT_X = MIDFIELD_X - GOALBOX_WIDTH / 2.;
static const double OPP_GOALBOX_RIGHT_X = MIDFIELD_X + GOALBOX_WIDTH / 2.;
static const double OPP_GOALBOX_BOTTOM_Y = FIELD_GREEN_HEIGHT-GREEN_PAD_Y-GOALBOX_HEIGHT;
static const double OPP_GOALBOX_TOP_Y = FIELD_GREEN_HEIGHT-GREEN_PAD_Y;



#elif ROBOT(NAO)
static const double LAB_SCALE = 0; // What is our lab scale?  Shrug
static const double OFFICIAL_SCALE = 1.0;
static const double FIELD_SCALE = OFFICIAL_SCALE;

static const double FIELD_WHITE_WIDTH = 400. * FIELD_SCALE;
static const double FIELD_WHITE_HEIGHT = 600. * FIELD_SCALE;
static const double FIELD_GREEN_WIDTH = 440. * FIELD_SCALE;
static const double FIELD_GREEN_HEIGHT = 680. * FIELD_SCALE;
static const double FIELD_WIDTH = FIELD_GREEN_WIDTH;
static const double FIELD_HEIGHT = FIELD_GREEN_HEIGHT;

static const double CENTER_FIELD_X = FIELD_GREEN_WIDTH / 2;
static const double CENTER_FIELD_Y = FIELD_GREEN_HEIGHT / 2;

static const double FIELD_GREEN_LEFT_SIDELINE_X = 0;
static const double FIELD_GREEN_RIGHT_SIDELINE_X = FIELD_GREEN_WIDTH;
static const double FIELD_GREEN_BOTTOM_SIDELINE_Y = 0;
static const double FIELD_GREEN_TOP_SIDELINE_Y = FIELD_GREEN_HEIGHT;

static const double GREEN_PAD_Y = 40. * FIELD_SCALE;
static const double GREEN_PAD_X = 20. * FIELD_SCALE;

static const double FIELD_WHITE_BOTTOM_SIDELINE_Y = GREEN_PAD_Y;
static const double FIELD_WHITE_TOP_SIDELINE_Y = FIELD_WHITE_HEIGHT + GREEN_PAD_Y;
static const double FIELD_WHITE_LEFT_SIDELINE_X = GREEN_PAD_X;
static const double FIELD_WHITE_RIGHT_SIDELINE_X = FIELD_WHITE_WIDTH + GREEN_PAD_X;


  // BEACON CONSTANTS
static const double LANDMARK_RIGHT_BEACON_X = FIELD_GREEN_WIDTH - 5.;
static const double LANDMARK_RIGHT_BEACON_Y = FIELD_GREEN_HEIGHT / 2.;
static const double LANDMARK_LEFT_BEACON_X = 5.;
static const double LANDMARK_LEFT_BEACON_Y = FIELD_GREEN_HEIGHT / 2.;

  // GOAL CONSTANTS
  // my left post is left of goalie defending my goal facing the opponent
static const double LANDMARK_MY_GOAL_LEFT_POST_X = CENTER_FIELD_X - 70;
static const double LANDMARK_MY_GOAL_RIGHT_POST_X = CENTER_FIELD_X + 70;
static const double LANDMARK_OPP_GOAL_LEFT_POST_X = CENTER_FIELD_X - 70;
static const double LANDMARK_OPP_GOAL_RIGHT_POST_X = CENTER_FIELD_X + 70;

// measure to the center of the posts, 5 cm off the line
static const double LANDMARK_MY_GOAL_LEFT_POST_Y = FIELD_WHITE_BOTTOM_SIDELINE_Y-5.0;
static const double LANDMARK_MY_GOAL_RIGHT_POST_Y = FIELD_WHITE_BOTTOM_SIDELINE_Y-5.0;
static const double LANDMARK_OPP_GOAL_LEFT_POST_Y = FIELD_WHITE_TOP_SIDELINE_Y+5.0;
static const double LANDMARK_OPP_GOAL_RIGHT_POST_Y = FIELD_WHITE_TOP_SIDELINE_Y+5.0;

static const double CENTER_CIRCLE_RADIUS = 65.; // not scaled

static const double GOALBOX_HEIGHT = 60.; // not scaled
static const double GOALBOX_WIDTH = 200.; // not scaled

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
static const double MY_GOALBOX_BOTTOM_Y = GREEN_PAD_Y; // bottom as in closest to (0,0)
static const double MY_GOALBOX_TOP_Y = GREEN_PAD_Y + GOALBOX_HEIGHT;

// opp goal box constants relative to (0,0) on my team
static const double OPP_GOALBOX_LEFT_X = MIDFIELD_X - GOALBOX_WIDTH / 2.;
static const double OPP_GOALBOX_RIGHT_X = MIDFIELD_X + GOALBOX_WIDTH / 2.;
static const double OPP_GOALBOX_BOTTOM_Y = FIELD_GREEN_HEIGHT-GREEN_PAD_Y-GOALBOX_HEIGHT;
static const double OPP_GOALBOX_TOP_Y = FIELD_GREEN_HEIGHT-GREEN_PAD_Y;


#endif // Nao constants

#endif // File
