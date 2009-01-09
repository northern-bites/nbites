#include "ConcreteFieldObject.h"

// Constructors
ConcreteFieldObject::ConcreteFieldObject(const float _fieldX,
                                         const float _fieldY,
                                         const fieldObjectID _id)
    : ConcreteLandmark(_fieldX, _fieldY), id(_id) {
}

// Copy constructor - just pairwise copy the elements
ConcreteFieldObject::ConcreteFieldObject(const ConcreteFieldObject& other)
    : ConcreteLandmark(other), id(other.id) {
}
// No memory to free
ConcreteFieldObject::~ConcreteFieldObject() {}

///////////////////////////////////////////////////////////////////////////////
// Constants for absolute coordinates on the field of the specific fieldObject.
// (0,0) is the lower left fieldObject of the field when BLUE goal is at the
// bottom
///////////////////////////////////////////////////////////////////////////////

const ConcreteFieldObject ConcreteFieldObject::blue_goal_left_post(
    LANDMARK_MY_GOAL_LEFT_POST_X, LANDMARK_MY_GOAL_LEFT_POST_Y,
    BLUE_GOAL_LEFT_POST);

const ConcreteFieldObject ConcreteFieldObject::blue_goal_right_post(
    LANDMARK_MY_GOAL_RIGHT_POST_X, LANDMARK_MY_GOAL_RIGHT_POST_Y,
    BLUE_GOAL_RIGHT_POST);

const ConcreteFieldObject ConcreteFieldObject::yellow_goal_left_post(
    LANDMARK_OPP_GOAL_LEFT_POST_X, LANDMARK_OPP_GOAL_LEFT_POST_Y,
    YELLOW_GOAL_LEFT_POST);

const ConcreteFieldObject ConcreteFieldObject::yellow_goal_right_post(
    LANDMARK_OPP_GOAL_RIGHT_POST_X, LANDMARK_OPP_GOAL_RIGHT_POST_Y,
    YELLOW_GOAL_RIGHT_POST);

const ConcreteFieldObject* ConcreteFieldObject::concreteFieldObjectList[
    NUM_FIELD_OBJECTS] = {&blue_goal_left_post, &blue_goal_right_post,
                          &yellow_goal_left_post, &yellow_goal_right_post};

const ConcreteFieldObject* ConcreteFieldObject::BLUE_GOAL_POSTS[
    NUM_BLUE_GOAL_POSTS] = {&blue_goal_left_post, &blue_goal_right_post};

const ConcreteFieldObject* ConcreteFieldObject::YELLOW_GOAL_POSTS[
    NUM_YELLOW_GOAL_POSTS] = {&yellow_goal_left_post, &yellow_goal_right_post};


const list <const ConcreteFieldObject*> ConcreteFieldObject::blueGoalPosts =
    list <const ConcreteFieldObject*>( ConcreteFieldObject::BLUE_GOAL_POSTS,
                                        &ConcreteFieldObject::
                                        BLUE_GOAL_POSTS[NUM_BLUE_GOAL_POSTS]);

const list <const ConcreteFieldObject*> ConcreteFieldObject::yellowGoalPosts =
    list <const ConcreteFieldObject*>( ConcreteFieldObject::YELLOW_GOAL_POSTS,
                                        &ConcreteFieldObject::YELLOW_GOAL_POSTS[
                                            NUM_YELLOW_GOAL_POSTS]);

const string ConcreteFieldObject::toString() const {
    switch(id) {
    case BLUE_GOAL_LEFT_POST:
        return "Blue Goal Left Post";
    case BLUE_GOAL_RIGHT_POST:
        return "Blue Goal Right Post";
    case YELLOW_GOAL_LEFT_POST:
        return "Yellow Goal Left Post";
    case YELLOW_GOAL_RIGHT_POST:
        return "Yellow Goal Right Post";
    case BLUE_GOAL_POST:
        return "Blue Goal Post";
    case YELLOW_GOAL_POST:
        return "Yellow Goal Post";
    default:
        return "Invalid Field Object ID";
    }

}

const string ConcreteFieldObject::getStringFromID(const fieldObjectID testID) {
    switch (testID) {
    case BLUE_GOAL_LEFT_POST:
        return "Blue goal left post";
    case BLUE_GOAL_RIGHT_POST:
        return "Blue goal right post";
    case YELLOW_GOAL_LEFT_POST:
        return "Yellow goal left post";
    case YELLOW_GOAL_RIGHT_POST:
        return "Yellow goal right post";
    case YELLOW_BLUE_BEACON:
        return "Yellow blue beacon";
    case BLUE_YELLOW_BEACON:
        return "Blue yellow beacon";
    case BLUE_ARC:
        return "Blue arc";
    case YELLOW_ARC:
        return "Yellow arc";
    default:
        return "Invalid ID";
    }
}


const bool ConcreteFieldObject::isAbstract(const fieldObjectID testID)
{
    return (testID == BLUE_GOAL_POST || testID == YELLOW_GOAL_POST);
}

const bool ConcreteFieldObject::isGoal(const fieldObjectID testID) {
    switch (testID) {
    case BLUE_GOAL_LEFT_POST: // Intentional fall throughs
    case BLUE_GOAL_RIGHT_POST:
    case YELLOW_GOAL_LEFT_POST:
    case YELLOW_GOAL_RIGHT_POST:
        return true;
    default:
        return false;
    }
}

const bool ConcreteFieldObject::isBeacon(const fieldObjectID testID) {
    switch (testID) {
    case YELLOW_BLUE_BEACON: // Intentional fall through
    case BLUE_YELLOW_BEACON:
        return true;
    default:
        return false;
    }
}

const bool ConcreteFieldObject::isArc(const fieldObjectID testID) {
    switch (testID) {
    case BLUE_ARC: // Intentional fall through
    case YELLOW_ARC:
        return true;
    default:
        return false;
    }
}

const float ConcreteFieldObject::getHeightFromGround(const fieldObjectID _id) {
    switch (_id) {
    case BLUE_GOAL_LEFT_POST:
    case BLUE_GOAL_RIGHT_POST:
    case YELLOW_GOAL_LEFT_POST:
    case YELLOW_GOAL_RIGHT_POST:
    case BLUE_ARC:
    case YELLOW_ARC:
        return 0;
        // The beacons only consider the blue and yellow portion, which are
        // elevated off the ground
    case YELLOW_BLUE_BEACON:
    case BLUE_YELLOW_BEACON:
        return WHITE_HEIGHT_ON_BEACON;
    default:
        return 0;
    }
}

