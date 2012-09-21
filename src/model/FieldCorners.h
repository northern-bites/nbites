/*
 * Defines concrete field corners.
 */

#pragma once

#include <Eigen/Dense>
#include "FieldConstants.h"

typedef Eigen::Vector3f FieldCorner;

// What these vectors mean
static const int X_VALUE = 0;
static const int Y_VALUE = 1;
static const int Z_VALUE = 2;

// About corners
static const int NUM_GREEN_CORNERS = 4;
static const int NUM_FIELD_CORNERS = 14;
static const int NUM_CORNERS = 18;

// Green outer corners
static const FieldCorner BOTTOM_LEFT_GREEN(GREEN_LEFT_EDGE_X,
                                           GREEN_BOTTOM_Y,
                                           0);

static const FieldCorner TOP_LEFT_GREEN(GREEN_LEFT_EDGE_X,
                                        GREEN_TOP_Y,
                                        0);

static const FieldCorner BOTTOM_RIGHT_GREEN(GREEN_RIGHT_EDGE_X,
                                            GREEN_BOTTOM_Y,
                                            0);

static const FieldCorner TOP_RIGHT_GREEN(GREEN_RIGHT_EDGE_X,
                                         GREEN_TOP_Y,
                                         0);

// Outer field corners
static const FieldCorner BOTTOM_LEFT_FIELD_L(FIELD_LEFT_EDGE_X,
                                             FIELD_BOTTOM_Y,
                                             0);

static const FieldCorner TOP_LEFT_FIELD_L(FIELD_LEFT_EDGE_X,
                                          FIELD_TOP_Y,
                                          0);

static const FieldCorner BOTTOM_RIGHT_FIELD_L(FIELD_RIGHT_EDGE_X,
                                              FIELD_BOTTOM_Y,
                                              0);

static const FieldCorner TOP_RIGHT_FIELD_L(FIELD_RIGHT_EDGE_X,
                                           FIELD_TOP_Y,
                                           0);

// Goalbox T corners
static const FieldCorner BOTTOM_LEFT_GOAL_T(FIELD_LEFT_EDGE_X,
                                            GOALBOX_BOTTOM_Y,
                                            0);

static const FieldCorner TOP_LEFT_GOAL_T(FIELD_LEFT_EDGE_X,
                                         GOALBOX_TOP_Y,
                                         0);

static const FieldCorner BOTTOM_RIGHT_GOAL_T(FIELD_RIGHT_EDGE_X,
                                             GOALBOX_BOTTOM_Y,
                                             0);

static const FieldCorner TOP_RIGHT_GOAL_T(FIELD_RIGHT_EDGE_X,
                                          GOALBOX_TOP_Y,
                                          0);

// Goalbox L corners
static const FieldCorner BOTTOM_LEFT_GOALBOX_L(LEFT_GOALBOX_FRONT_X,
                                               GOALBOX_BOTTOM_Y,
                                               0);

static const FieldCorner TOP_LEFT_GOALBOX_L(LEFT_GOALBOX_FRONT_X,
                                            GOALBOX_TOP_Y,
                                            0);

static const FieldCorner BOTTOM_RIGHT_GOALBOX_L(RIGHT_GOALBOX_FRONT_X,
                                                GOALBOX_BOTTOM_Y,
                                                0);

static const FieldCorner TOP_RIGHT_GOALBOX_L(RIGHT_GOALBOX_FRONT_X,
                                             GOALBOX_TOP_Y,
                                             0);

// Center line T corners
static const FieldCorner TOP_CENTER_T(HALF_FIELD_X,
                                      FIELD_TOP_Y,
                                      0);

static const FieldCorner BOTTOM_CENTER_T(HALF_FIELD_X,
                                         FIELD_BOTTOM_Y,
                                         0);

// Arrays of all the corners
static const FieldCorner GREEN_CORNERS[NUM_GREEN_CORNERS] =
{ BOTTOM_LEFT_GREEN,
  TOP_LEFT_GREEN,
  BOTTOM_RIGHT_GREEN,
  TOP_RIGHT_GREEN };

static const FieldCorner FIELD_CORNERS[NUM_FIELD_CORNERS] =
{ BOTTOM_LEFT_FIELD_L,
  TOP_LEFT_FIELD_L,
  BOTTOM_RIGHT_FIELD_L,
  TOP_RIGHT_FIELD_L,
  BOTTOM_LEFT_GOAL_T,
  TOP_LEFT_GOAL_T,
  BOTTOM_RIGHT_GOAL_T,
  TOP_RIGHT_GOAL_T,
  BOTTOM_LEFT_GOALBOX_L,
  TOP_LEFT_GOALBOX_L,
  BOTTOM_RIGHT_GOALBOX_L,
  TOP_RIGHT_GOALBOX_L,
  TOP_CENTER_T,
  BOTTOM_CENTER_T};

static const FieldCorner ALL_CORNERS[NUM_CORNERS] =
{ BOTTOM_LEFT_GREEN,
  TOP_LEFT_GREEN,
  BOTTOM_RIGHT_GREEN,
  TOP_RIGHT_GREEN,
  BOTTOM_LEFT_FIELD_L,
  TOP_LEFT_FIELD_L,
  BOTTOM_RIGHT_FIELD_L,
  TOP_RIGHT_FIELD_L,
  BOTTOM_LEFT_GOAL_T,
  TOP_LEFT_GOAL_T,
  BOTTOM_RIGHT_GOAL_T,
  TOP_RIGHT_GOAL_T,
  BOTTOM_LEFT_GOALBOX_L,
  TOP_LEFT_GOALBOX_L,
  BOTTOM_RIGHT_GOALBOX_L,
  TOP_RIGHT_GOALBOX_L,
  TOP_CENTER_T,
  BOTTOM_CENTER_T};
