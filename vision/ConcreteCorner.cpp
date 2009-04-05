#include "ConcreteCorner.h"
using namespace std;

ConcreteCorner::ConcreteCorner(const float _fieldX, const float _fieldY,
                               const cornerID _id)
    : ConcreteLandmark(_fieldX, _fieldY), id(_id) {
    cornerType = inferCornerType(_id);
}
ConcreteCorner::ConcreteCorner(const float _fieldX, const float _fieldY,
                               const ConcreteLine _l1, const ConcreteLine _l2,
                               const cornerID _id)
    : ConcreteLandmark(_fieldX, _fieldY), id(_id) , line1(&_l1), line2(&_l2) {
    cornerType = inferCornerType(_id);
}

// Copy constructor - just pairwise copy the elements
ConcreteCorner::ConcreteCorner(const ConcreteCorner& other)
    : ConcreteLandmark(other), id(other.id), cornerType(other.cornerType) {
}

// No memory to free
ConcreteCorner::~ConcreteCorner() {}

///////////////////////////////////////////////////////////////////////////////
// Constants for absolute coordinates on the field of the specific corners.
// (0,0) is the lower left corner of the field when BLUE goal is at the
// bottom
///////////////////////////////////////////////////////////////////////////////
const ConcreteCorner ConcreteCorner::
blue_corner_top_l(FIELD_WHITE_LEFT_SIDELINE_X,
                  FIELD_WHITE_TOP_SIDELINE_Y,
                  BLUE_CORNER_TOP_L);

const ConcreteCorner ConcreteCorner::
blue_corner_bottom_l(FIELD_WHITE_LEFT_SIDELINE_X,
                     FIELD_WHITE_BOTTOM_SIDELINE_Y,
                     BLUE_CORNER_BOTTOM_L);

const ConcreteCorner ConcreteCorner::
blue_goal_left_t(BLUE_GOALBOX_LEFT_X,
                 BLUE_GOALBOX_TOP_Y,
                 BLUE_GOAL_LEFT_T);

const ConcreteCorner ConcreteCorner::
blue_goal_right_t(BLUE_GOALBOX_LEFT_X,
                  BLUE_GOALBOX_BOTTOM_Y,
                  BLUE_GOAL_RIGHT_T);

const ConcreteCorner ConcreteCorner::
blue_goal_left_l(BLUE_GOALBOX_RIGHT_X,
                 BLUE_GOALBOX_TOP_Y,
                 BLUE_GOAL_LEFT_L);

const ConcreteCorner ConcreteCorner::
blue_goal_right_l(BLUE_GOALBOX_RIGHT_X,
                  BLUE_GOALBOX_BOTTOM_Y,
                  BLUE_GOAL_RIGHT_L);

const ConcreteCorner ConcreteCorner::
yellow_corner_bottom_l(FIELD_WHITE_RIGHT_SIDELINE_X,
                       FIELD_WHITE_BOTTOM_SIDELINE_Y,
                       YELLOW_CORNER_BOTTOM_L);

const ConcreteCorner ConcreteCorner::
yellow_corner_top_l(FIELD_WHITE_RIGHT_SIDELINE_X,
                    FIELD_WHITE_TOP_SIDELINE_Y,
                    YELLOW_CORNER_TOP_L);

const ConcreteCorner ConcreteCorner::
yellow_goal_left_t(YELLOW_GOALBOX_RIGHT_X,
                   YELLOW_GOALBOX_BOTTOM_Y,
                   YELLOW_GOAL_LEFT_T);

const ConcreteCorner ConcreteCorner::
yellow_goal_right_t(YELLOW_GOALBOX_RIGHT_X,
                    YELLOW_GOALBOX_TOP_Y,
                    YELLOW_GOAL_RIGHT_T);

const ConcreteCorner ConcreteCorner::
yellow_goal_left_l(YELLOW_GOALBOX_LEFT_X,
                   YELLOW_GOALBOX_BOTTOM_Y,
                   YELLOW_GOAL_LEFT_L);

const ConcreteCorner ConcreteCorner::
yellow_goal_right_l(YELLOW_GOALBOX_LEFT_X,
                    YELLOW_GOALBOX_TOP_Y,
                    YELLOW_GOAL_RIGHT_L);

const ConcreteCorner ConcreteCorner::
center_circle(MIDFIELD_X,
              MIDFIELD_Y,
              CENTER_CIRCLE);

const ConcreteCorner ConcreteCorner::
center_bottom_t(MIDFIELD_X,
                FIELD_WHITE_BOTTOM_SIDELINE_Y,
                CENTER_BOTTOM_T);

const ConcreteCorner ConcreteCorner::
center_top_t(MIDFIELD_X,
             FIELD_WHITE_TOP_SIDELINE_Y,
             CENTER_TOP_T);

const ConcreteCorner* ConcreteCorner::concreteCornerList[NUM_CORNERS] =
{&blue_corner_top_l,
 &blue_corner_bottom_l,
 &blue_goal_left_t,
 &blue_goal_right_t,
 &blue_goal_left_l,
 &blue_goal_right_l,
 &center_top_t,
 &center_bottom_t,
 &yellow_corner_bottom_l,
 &yellow_corner_top_l,
 &yellow_goal_left_t,
 &yellow_goal_right_t,
 &yellow_goal_left_l,
 &yellow_goal_right_l,
 &center_circle
};


const ConcreteCorner* ConcreteCorner::
L_CORNERS[NUM_L_CORNERS] = {
    &blue_corner_top_l,
    &blue_corner_bottom_l,
    &blue_goal_left_l,
    &blue_goal_right_l,
    &yellow_corner_bottom_l,
    &yellow_corner_top_l,
    &yellow_goal_left_l,
    &yellow_goal_right_l
};

const ConcreteCorner* ConcreteCorner::T_CORNERS[NUM_T_CORNERS] = {
    &blue_goal_left_t,
    &blue_goal_right_t,
    &yellow_goal_left_t,
    &yellow_goal_right_t,
    &center_top_t,
    &center_bottom_t
};

const ConcreteCorner* ConcreteCorner::CC_CORNERS[NUM_CC_CORNERS] = {
    &center_circle
};


const ConcreteCorner* ConcreteCorner::YELLOW_GOAL_CORNERS[
    NUM_YELLOW_GOAL_CORNERS] = {
    &yellow_goal_left_l,
    &yellow_goal_right_l,
    &yellow_goal_left_t,
    &yellow_goal_right_t
};
const ConcreteCorner* ConcreteCorner::BLUE_GOAL_CORNERS[
    NUM_BLUE_GOAL_CORNERS] = {
    &blue_goal_left_l,
    &blue_goal_right_l,
    &blue_goal_left_t,
    &blue_goal_right_t
};

const ConcreteCorner* ConcreteCorner::YELLOW_GOAL_T_CORNERS[
    NUM_YELLOW_GOAL_T_CORNERS] =
{
    &yellow_goal_left_t,
    &yellow_goal_right_t
};
const ConcreteCorner* ConcreteCorner::BLUE_GOAL_T_CORNERS[
    NUM_BLUE_GOAL_T_CORNERS] =
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
                                  &ConcreteCorner::BLUE_GOAL_CORNERS[
                                      NUM_BLUE_GOAL_CORNERS] );

const list <const ConcreteCorner*> ConcreteCorner::yellowGoalCorners =
    list <const ConcreteCorner*>(ConcreteCorner::YELLOW_GOAL_CORNERS,
                                 &ConcreteCorner::YELLOW_GOAL_CORNERS[
                                     NUM_YELLOW_GOAL_CORNERS] );

const list <const ConcreteCorner*> ConcreteCorner::blueGoalTCorners =
    list <const ConcreteCorner*>(ConcreteCorner::BLUE_GOAL_T_CORNERS,
                                 &ConcreteCorner::BLUE_GOAL_T_CORNERS[
                                     NUM_BLUE_GOAL_T_CORNERS] );

const list <const ConcreteCorner*> ConcreteCorner::yellowGoalTCorners =
    list <const ConcreteCorner*>(ConcreteCorner::YELLOW_GOAL_T_CORNERS,
                                 &ConcreteCorner::YELLOW_GOAL_T_CORNERS[
                                     NUM_YELLOW_GOAL_T_CORNERS] );


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
    case BLUE_CORNER_TOP_L:
        return "Blue Corner Top L";
    case BLUE_CORNER_BOTTOM_L:
        return "Blue Corner Bottom L";
    case BLUE_GOAL_LEFT_T:
        return "Blue Goal Left T";
    case BLUE_GOAL_RIGHT_T:
        return "Blue Goal Right T";
    case BLUE_GOAL_LEFT_L:
        return "Blue Goal Left L";
    case BLUE_GOAL_RIGHT_L:
        return "Blue Goal Right L";

        // Yellow ones
    case YELLOW_CORNER_BOTTOM_L:
        return "Yellow Corner Bottom L";
    case YELLOW_CORNER_TOP_L:
        return "Yellow Corner Top L";
    case YELLOW_GOAL_RIGHT_T:
        return "Yellow Goal Right T";
    case YELLOW_GOAL_LEFT_T:
        return "Yellow Goal Left T";
    case YELLOW_GOAL_RIGHT_L:
        return "Yellow Goal Right L";
    case YELLOW_GOAL_LEFT_L:
        return "Yellow Goal Left L";

        // Center ones
    case CENTER_BOTTOM_T:
        return "Center Bottom T";
    case CENTER_TOP_T:
        return "Center Top T";
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
    case BLUE_CORNER_TOP_L:
    case BLUE_CORNER_BOTTOM_L:
    case BLUE_GOAL_LEFT_L:
    case BLUE_GOAL_RIGHT_L:
    case YELLOW_CORNER_TOP_L:
    case YELLOW_CORNER_BOTTOM_L:
    case YELLOW_GOAL_LEFT_L:
    case YELLOW_GOAL_RIGHT_L:
        return INNER_L;

    case T_CORNER:
    case BLUE_GOAL_T:
    case YELLOW_GOAL_T:
    case YELLOW_GOAL_LEFT_T:
    case YELLOW_GOAL_RIGHT_T:
    case CENTER_TOP_T:
    case CENTER_BOTTOM_T:
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
