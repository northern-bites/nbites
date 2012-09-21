/**
 * Constants specifying field size, object size, robot motion constraints.
 */

#pragma once

// Ball constants
static const float BALL_DIAMETER = 6.5f;
static const float BALL_RADIUS   = 0.5f * BALL_DIAMETER;

// Robot head constants
// From NAO documentation
static const float MAX_HEAD_PITCH =   29.5f;
static const float MIN_HEAD_PITCH =  -38.5f;
static const float MAX_HEAD_YAW   =  119.5f;
static const float MIN_HEAD_YAW   = -119.5f;

// For using most vectors
static const int X_VALUE = 0;
static const int Y_VALUE = 1;
static const int Z_VALUE = 2;

// Field dimensions
static const float FIELD_WIDTH           = 600.f;
static const float FIELD_HEIGHT          = 400.f;
static const float EDGE_WIDTH            = 70.f;

static const float GREEN_WIDTH           = FIELD_WIDTH + 2.0f * EDGE_WIDTH;
static const float GREEN_HEIGHT          = FIELD_HEIGHT + 2.0f * EDGE_WIDTH;

static const float GOALBOX_WIDTH         = 60.f;
static const float GOALBOX_HEIGHT        = 220.f;

static const float GOAL_WIDTH            = 140.f;

static const float CENTER_CIRCLE_DIAM    = 120.f;
static const float CENTER_CIRCLE_RAD     = CENTER_CIRCLE_DIAM * 0.5f;

static const float CROSS_WIDTH           = 10.f;

static const float GOALPOST_WIDTH        = 10.f;
static const float GOAL_DEPTH            = 40.0f;

// useful x and y values
static const float GREEN_BOTTOM_Y        = 0;
static const float GREEN_TOP_Y           = GREEN_HEIGHT;

static const float FIELD_BOTTOM_Y        = EDGE_WIDTH;
static const float FIELD_TOP_Y           = EDGE_WIDTH + FIELD_HEIGHT;

static const float GOALBOX_BOTTOM_Y      = 0.5f *
                                          (GREEN_HEIGHT - GOALBOX_HEIGHT);
static const float GOALBOX_TOP_Y         = 0.5f *
                                          (GREEN_HEIGHT + GOALBOX_HEIGHT);

static const float BOTTOM_GOALPOST_Y     = 0.5f * (GREEN_HEIGHT - GOAL_WIDTH);
static const float TOP_GOALPOST_Y        = 0.5f * (GREEN_HEIGHT + GOAL_WIDTH);

static const float CROSS_Y               = 0.5f * GREEN_HEIGHT;

static const float GREEN_LEFT_EDGE_X     = 0;
static const float GREEN_RIGHT_EDGE_X    = GREEN_WIDTH;

static const float FIELD_LEFT_EDGE_X     = EDGE_WIDTH;
static const float FIELD_RIGHT_EDGE_X    = EDGE_WIDTH + FIELD_WIDTH;

static const float LEFT_GOAL_X           = FIELD_LEFT_EDGE_X;
static const float RIGHT_GOAL_X          = FIELD_RIGHT_EDGE_X;

static const float LEFT_GOAL_BACK_X      = FIELD_LEFT_EDGE_X - GOAL_DEPTH;
static const float RIGHT_GOAL_BACK_X     = FIELD_RIGHT_EDGE_X + GOAL_DEPTH;

static const float LEFT_GOALBOX_FRONT_X  = FIELD_LEFT_EDGE_X + GOALBOX_WIDTH;
static const float RIGHT_GOALBOX_FRONT_X = FIELD_RIGHT_EDGE_X - GOALBOX_WIDTH;

static const float HALF_FIELD_X          = 0.5f * GREEN_WIDTH;


