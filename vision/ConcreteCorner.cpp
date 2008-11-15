#include "ConcreteCorner.h"


ConcreteCorner::ConcreteCorner(const float _fieldX, const float _fieldY,
                               const cornerID _id)
  : ConcreteLandmark(_fieldX, _fieldY) {
  id = _id;

  cornerType = inferCornerType(_id);
}

// Copy constructor - just pairwise copy the elements
ConcreteCorner::ConcreteCorner(const ConcreteCorner& other)
  : ConcreteLandmark(other) {
  id = other.id;
  cornerType = other.cornerType;
}

// No memory to free
ConcreteCorner::~ConcreteCorner() {}

///////////////////////////////////////////////////////////////////////////////
// Constants for absolute coordinates on the field of the specific corners.
// (0,0) is the lower left corner of the field when BLUE goal is at the
// bottom
///////////////////////////////////////////////////////////////////////////////
const ConcreteCorner ConcreteCorner::
blue_corner_left_l(FIELD_WHITE_LEFT_SIDELINE_X,
                   FIELD_WHITE_BOTTOM_SIDELINE_Y,
                   BLUE_CORNER_LEFT_L);

const ConcreteCorner ConcreteCorner::
blue_corner_right_l(FIELD_WHITE_RIGHT_SIDELINE_X,
                    FIELD_WHITE_BOTTOM_SIDELINE_Y,
                    BLUE_CORNER_RIGHT_L);

const ConcreteCorner ConcreteCorner::
blue_goal_left_t(GOALBOX_LEFT_X,
                 FIELD_WHITE_BOTTOM_SIDELINE_Y,
                 BLUE_GOAL_LEFT_T);

const ConcreteCorner ConcreteCorner::
blue_goal_right_t(GOALBOX_RIGHT_X,
                  FIELD_WHITE_BOTTOM_SIDELINE_Y,
                  BLUE_GOAL_RIGHT_T);

const ConcreteCorner ConcreteCorner::
blue_goal_left_l(GOALBOX_LEFT_X,
                 GOALBOX_HEIGHT + FIELD_WHITE_BOTTOM_SIDELINE_Y,
                 BLUE_GOAL_LEFT_L);

const ConcreteCorner ConcreteCorner::
blue_goal_right_l(GOALBOX_RIGHT_X,
                  GOALBOX_HEIGHT+FIELD_WHITE_BOTTOM_SIDELINE_Y,
                  BLUE_GOAL_RIGHT_L);

const ConcreteCorner ConcreteCorner::
yellow_corner_left_l(FIELD_WHITE_RIGHT_SIDELINE_X,
                     FIELD_WHITE_TOP_SIDELINE_Y,
                     YELLOW_CORNER_LEFT_L);

const ConcreteCorner ConcreteCorner::
yellow_corner_right_l(FIELD_WHITE_LEFT_SIDELINE_X,
                      FIELD_WHITE_TOP_SIDELINE_Y,
                      YELLOW_CORNER_RIGHT_L);

const ConcreteCorner ConcreteCorner::
yellow_goal_left_t(GOALBOX_RIGHT_X,
                   FIELD_WHITE_TOP_SIDELINE_Y,
                   YELLOW_GOAL_LEFT_T);

const ConcreteCorner ConcreteCorner::
yellow_goal_right_t(GOALBOX_LEFT_X,
                    FIELD_WHITE_TOP_SIDELINE_Y,
                    YELLOW_GOAL_RIGHT_T);

const ConcreteCorner ConcreteCorner::
yellow_goal_left_l(GOALBOX_RIGHT_X,
                   FIELD_WHITE_TOP_SIDELINE_Y - GOALBOX_HEIGHT,
                   YELLOW_GOAL_LEFT_L);

const ConcreteCorner ConcreteCorner::
yellow_goal_right_l(GOALBOX_LEFT_X,
                    FIELD_WHITE_TOP_SIDELINE_Y - GOALBOX_HEIGHT,
                    YELLOW_GOAL_RIGHT_L);

const ConcreteCorner ConcreteCorner::
center_circle(MIDFIELD_X,
              MIDFIELD_Y,
              CENTER_CIRCLE);

const ConcreteCorner ConcreteCorner::
center_by_t(FIELD_WHITE_RIGHT_SIDELINE_X,
            MIDFIELD_Y,
            CENTER_BY_T);

const ConcreteCorner ConcreteCorner::
center_yb_t(FIELD_WHITE_LEFT_SIDELINE_X,
            MIDFIELD_Y,
            CENTER_YB_T);

const ConcreteCorner* ConcreteCorner::concreteCornerList[NUM_CORNERS] =
  {&blue_corner_left_l,
   &blue_corner_right_l,
   &blue_goal_left_t,
   &blue_goal_right_t,
   &blue_goal_left_l,
   &blue_goal_right_l,
   &center_by_t,
   &center_yb_t,
   &yellow_corner_left_l,
   &yellow_corner_right_l,
   &yellow_goal_left_t,
   &yellow_goal_right_t,
   &yellow_goal_left_l,
   &yellow_goal_right_l,
   &center_circle
  };


const ConcreteCorner* ConcreteCorner::
L_CORNERS[NUM_L_CORNERS] = {
  &blue_corner_left_l,
  &blue_corner_right_l,
  &blue_goal_left_l,
  &blue_goal_right_l,
  &yellow_corner_left_l,
  &yellow_corner_right_l,
  &yellow_goal_left_l,
  &yellow_goal_right_l
};

const ConcreteCorner* ConcreteCorner::T_CORNERS[NUM_T_CORNERS] = {
  &blue_goal_left_t,
  &blue_goal_right_t,
  &yellow_goal_left_t,
  &yellow_goal_right_t,
  &center_by_t,
  &center_yb_t
};

const ConcreteCorner* ConcreteCorner::CC_CORNERS[NUM_CC_CORNERS] = {
  &center_circle
};


const ConcreteCorner* ConcreteCorner::YELLOW_GOAL_CORNERS[NUM_YELLOW_GOAL_CORNERS] = {
  &yellow_goal_left_l,
  &yellow_goal_right_l,
  &yellow_goal_left_t,
  &yellow_goal_right_t
};
const ConcreteCorner* ConcreteCorner::BLUE_GOAL_CORNERS[NUM_BLUE_GOAL_CORNERS] = {
  &blue_goal_left_l,
  &blue_goal_right_l,
  &blue_goal_left_t,
  &blue_goal_right_t
};
// Yellow arcs are to the left of the goals
const ConcreteCorner* ConcreteCorner::YELLOW_ARC_CORNERS[NUM_YELLOW_ARC_CORNERS] = {
  &yellow_corner_left_l,
  &blue_corner_left_l
};
// Blue arcs to the right
const ConcreteCorner* ConcreteCorner::BLUE_ARC_CORNERS[NUM_BLUE_ARC_CORNERS] = {
  &yellow_corner_right_l,
  &blue_corner_right_l
};

const ConcreteCorner* ConcreteCorner::YELLOW_GOAL_T_CORNERS[NUM_YELLOW_GOAL_T_CORNERS] =
  {
    &yellow_goal_left_t,
    &yellow_goal_right_t
  };
const ConcreteCorner* ConcreteCorner::BLUE_GOAL_T_CORNERS[NUM_BLUE_GOAL_T_CORNERS] =
  {
    &blue_goal_left_t,
    &blue_goal_right_t
  };


const list <const ConcreteCorner*> ConcreteCorner::lCorners =
  list <const ConcreteCorner*>( ConcreteCorner::L_CORNERS,
                                &ConcreteCorner::L_CORNERS[NUM_L_CORNERS] );

const list <const ConcreteCorner*> ConcreteCorner::tCorners =
  list <const ConcreteCorner*>( ConcreteCorner::T_CORNERS,
				  &ConcreteCorner::T_CORNERS[NUM_T_CORNERS] );

const list <const ConcreteCorner*> ConcreteCorner::ccCorners =
  list <const ConcreteCorner*>( ConcreteCorner::CC_CORNERS,
				  &ConcreteCorner::CC_CORNERS[NUM_CC_CORNERS] );

const list <const ConcreteCorner*> ConcreteCorner::blueGoalCorners =
  list <const ConcreteCorner*>( ConcreteCorner::BLUE_GOAL_CORNERS,
				  &ConcreteCorner::BLUE_GOAL_CORNERS[NUM_BLUE_GOAL_CORNERS] );

const list <const ConcreteCorner*> ConcreteCorner::yellowGoalCorners =
  list <const ConcreteCorner*>(ConcreteCorner::YELLOW_GOAL_CORNERS,
                               &ConcreteCorner::YELLOW_GOAL_CORNERS[NUM_YELLOW_GOAL_CORNERS] );

const list <const ConcreteCorner*> ConcreteCorner::yellowArcCorners =
  list <const ConcreteCorner*>(ConcreteCorner::YELLOW_ARC_CORNERS,
                               &ConcreteCorner::YELLOW_ARC_CORNERS[NUM_YELLOW_ARC_CORNERS] );

const list <const ConcreteCorner*> ConcreteCorner::blueArcCorners =
  list <const ConcreteCorner*>(ConcreteCorner::BLUE_ARC_CORNERS,
                               &ConcreteCorner::BLUE_ARC_CORNERS[NUM_BLUE_ARC_CORNERS] );


const list <const ConcreteCorner*> ConcreteCorner::blueGoalTCorners =
  list <const ConcreteCorner*>(ConcreteCorner::BLUE_GOAL_T_CORNERS,
                               &ConcreteCorner::BLUE_GOAL_T_CORNERS[NUM_BLUE_GOAL_T_CORNERS] );

const list <const ConcreteCorner*> ConcreteCorner::yellowGoalTCorners =
  list <const ConcreteCorner*>(ConcreteCorner::YELLOW_GOAL_T_CORNERS,
                               &ConcreteCorner::YELLOW_GOAL_T_CORNERS[NUM_YELLOW_GOAL_T_CORNERS] );


const string ConcreteCorner::getShapeString(shape s) {
  switch (s) {
  case INNER_L:
    return "Inner L";
  case OUTER_L:
    return "Outer L";
  case T:
    return "T";
  case CIRCLE:
    return "Circle";
  case UNKNOWN:
    return "Unknown";
  default:
    throw "Error, invalid shape";
  }

}

const string ConcreteCorner::toString() const {
  switch (id) {
    //Blue ones
  case BLUE_CORNER_LEFT_L:
    return "Blue Corner Left L";
  case BLUE_CORNER_RIGHT_L:
    return "Blue Corner Right L";
  case BLUE_GOAL_LEFT_T:
    return "Blue Goal Left T";
  case BLUE_GOAL_RIGHT_T:
    return "Blue Goal Right T";
  case BLUE_GOAL_LEFT_L:
    return "Blue Goal Left L";
  case BLUE_GOAL_RIGHT_L:
    return "Blue Goal Right L";

    // Yellow ones
  case YELLOW_CORNER_RIGHT_L:
    return "Yellow Corner Right L";
  case YELLOW_CORNER_LEFT_L:
    return "Yellow Corner Left L";
  case YELLOW_GOAL_RIGHT_T:
    return "Yellow Goal Right T";
  case YELLOW_GOAL_LEFT_T:
    return "Yellow Goal Left T";
  case YELLOW_GOAL_RIGHT_L:
    return "Yellow Goal Right L";
  case YELLOW_GOAL_LEFT_L:
    return "Yellow Goal Left L";

    // Center ones
  case CENTER_YB_T:
    return "Center YB T";
  case CENTER_BY_T:
    return "Center BY T";
  case CENTER_CIRCLE:
    return "Center Circle Corner";

  default:
    return "Invalid Corner Id";
  }

}

const shape ConcreteCorner::inferCornerType(const cornerID id) {
  switch (id) {
  case L_INNER_CORNER:
  case L_OUTER_CORNER:
  case BLUE_GOAL_RIGHT_L_OR_YELLOW_GOAL_LEFT_L:
  case BLUE_GOAL_LEFT_L_OR_YELLOW_GOAL_RIGHT_L:
  case CORNER_INNER_L:
  case GOAL_BOX_INNER_L:
    //  case CORNER_OR_GOAL_INNER_L:
  case BLUE_GOAL_OUTER_L:
  case YELLOW_GOAL_OUTER_L:
  case BLUE_CORNER_LEFT_L:
  case BLUE_CORNER_RIGHT_L:
  case BLUE_GOAL_LEFT_L:
  case BLUE_GOAL_RIGHT_L:
  case YELLOW_CORNER_LEFT_L:
  case YELLOW_CORNER_RIGHT_L:
  case YELLOW_GOAL_LEFT_L:
  case YELLOW_GOAL_RIGHT_L:
    return INNER_L;

  case T_CORNER:
  case BLUE_GOAL_T:
  case YELLOW_GOAL_T:
  case YELLOW_GOAL_LEFT_T:
  case YELLOW_GOAL_RIGHT_T:
  case CENTER_BY_T:
  case CENTER_YB_T:
  case BLUE_GOAL_LEFT_T:
  case BLUE_GOAL_RIGHT_T:
  case CENTER_T:
    return T;
  case CENTER_CIRCLE:
    return CIRCLE;

  default:
    // Should never be reached. Some IDs must have been missed.
    throw -1;
  }
}

/*
 * Given a shape, returns a list of all the concrete corners that this shaped
 * corner could possibly be.
 */
const list <const ConcreteCorner*> ConcreteCorner::
getPossibleCorners(shape corner_type) {
  list <const ConcreteCorner*> possible;
  switch (corner_type) {
  case INNER_L:
    // intentional fall through
  case OUTER_L:
    return lCorners;
  case T:
    return tCorners;
  case CIRCLE:
    return ccCorners;
  default:
    // Should never be reached
    throw -1;
  }
}
