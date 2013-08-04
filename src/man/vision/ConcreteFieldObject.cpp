#include "ConcreteFieldObject.h"
using namespace std;

namespace man {
namespace vision {

// Constructors
ConcreteFieldObject::ConcreteFieldObject(const float _fieldX,
                                         const float _fieldY,
                                         const fieldObjectID _id)
    : ConcreteLandmark(_fieldX, _fieldY), id(_id) {}

// Copy constructor - just pairwise copy the elements
ConcreteFieldObject::ConcreteFieldObject(const ConcreteFieldObject& other)
    : ConcreteLandmark(other), id(other.id) {}
// No memory to free
ConcreteFieldObject::~ConcreteFieldObject() {}

///////////////////////////////////////////////////////////////////////////////
// Constants for absolute coordinates on the field of the specific fieldObject.
// (0,0) is the lower left fieldObject of the field when BLUE goal is at the
// bottom
///////////////////////////////////////////////////////////////////////////////

const ConcreteFieldObject ConcreteFieldObject::blue_goal_left_post(
    LANDMARK_BLUE_GOAL_TOP_POST_X, LANDMARK_BLUE_GOAL_TOP_POST_Y,
    BLUE_GOAL_LEFT_POST);

const ConcreteFieldObject ConcreteFieldObject::blue_goal_right_post(
    LANDMARK_BLUE_GOAL_BOTTOM_POST_X, LANDMARK_BLUE_GOAL_BOTTOM_POST_Y,
    BLUE_GOAL_RIGHT_POST);

const ConcreteFieldObject ConcreteFieldObject::yellow_goal_left_post(
    LANDMARK_YELLOW_GOAL_BOTTOM_POST_X, LANDMARK_YELLOW_GOAL_BOTTOM_POST_Y,
    YELLOW_GOAL_LEFT_POST);

const ConcreteFieldObject ConcreteFieldObject::yellow_goal_right_post(
    LANDMARK_YELLOW_GOAL_TOP_POST_X, LANDMARK_YELLOW_GOAL_TOP_POST_Y,
    YELLOW_GOAL_RIGHT_POST);

const ConcreteFieldObject* ConcreteFieldObject::concreteFieldObjectList[
    NUM_FIELD_OBJECTS] = {&blue_goal_left_post, &blue_goal_right_post,
                          &yellow_goal_left_post, &yellow_goal_right_post};

const ConcreteFieldObject* ConcreteFieldObject::BLUE_GOAL_POSTS[
    NUM_BLUE_GOAL_POSTS] = {&blue_goal_left_post, &blue_goal_right_post};

const ConcreteFieldObject* ConcreteFieldObject::YELLOW_GOAL_POSTS[
    2] = {&yellow_goal_left_post, &yellow_goal_right_post};

const ConcreteFieldObject* ConcreteFieldObject::RIGHT_GOAL_POSTS[
    NUM_RIGHT_GOAL_POSTS] = {&yellow_goal_right_post, &blue_goal_right_post};

const ConcreteFieldObject* ConcreteFieldObject::LEFT_GOAL_POSTS[
    NUM_LEFT_GOAL_POSTS] = {&yellow_goal_left_post, &blue_goal_left_post};


const list <const ConcreteFieldObject*> ConcreteFieldObject::blueGoalPosts =
    list <const ConcreteFieldObject*>( ConcreteFieldObject::BLUE_GOAL_POSTS,
                                        &ConcreteFieldObject::
                                        BLUE_GOAL_POSTS[NUM_BLUE_GOAL_POSTS]);

const list <const ConcreteFieldObject*> ConcreteFieldObject::yellowGoalPosts =
    list <const ConcreteFieldObject*>( ConcreteFieldObject::YELLOW_GOAL_POSTS,
                                        &ConcreteFieldObject::YELLOW_GOAL_POSTS[
                                            NUM_YELLOW_GOAL_POSTS]);

const list <const ConcreteFieldObject*> ConcreteFieldObject::rightPosts =
    list <const ConcreteFieldObject*>( ConcreteFieldObject::RIGHT_GOAL_POSTS,
                                        &ConcreteFieldObject::RIGHT_GOAL_POSTS[
                                            NUM_RIGHT_GOAL_POSTS]);

const list <const ConcreteFieldObject*> ConcreteFieldObject::leftPosts =
    list <const ConcreteFieldObject*>( ConcreteFieldObject::LEFT_GOAL_POSTS,
                                        &ConcreteFieldObject::LEFT_GOAL_POSTS[
                                            NUM_LEFT_GOAL_POSTS]);

const list <const ConcreteFieldObject*> ConcreteFieldObject::allPosts =
    list <const ConcreteFieldObject*>( ConcreteFieldObject::concreteFieldObjectList,
                                        &ConcreteFieldObject::concreteFieldObjectList[
                                            NUM_FIELD_OBJECTS]);


// Lists of single posts, to make the functions easier to call...
const list <const ConcreteFieldObject*>
ConcreteFieldObject::blueGoalLeftPostList =
    list <const ConcreteFieldObject*>( 1, &blue_goal_left_post);
const list <const ConcreteFieldObject*>
ConcreteFieldObject::blueGoalRightPostList =
    list <const ConcreteFieldObject*>( 1, &blue_goal_right_post);
const list <const ConcreteFieldObject*>
ConcreteFieldObject::yellowGoalLeftPostList =
    list <const ConcreteFieldObject*>( 1, &yellow_goal_left_post);
const list <const ConcreteFieldObject*>
ConcreteFieldObject::yellowGoalRightPostList =
    list <const ConcreteFieldObject*>( 1, &yellow_goal_right_post);

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

const string ConcreteFieldObject::getStringFromID(const int testID)
{
    switch (testID) {
    case BLUE_GOAL_LEFT_POST:
        return "Blue goal left post";
    case BLUE_GOAL_RIGHT_POST:
        return "Blue goal right post";
    case YELLOW_GOAL_LEFT_POST:
        return "Yellow goal left post";
    case YELLOW_GOAL_RIGHT_POST:
        return "Yellow goal right post";
    default:
        return "Invalid ID";
    }
}


const bool ConcreteFieldObject::isAbstract(const int testID)
{
    return (testID == BLUE_GOAL_POST || testID == YELLOW_GOAL_POST);
}

const bool ConcreteFieldObject::isGoal(const int testID) {
    switch (testID) {
    case BLUE_GOAL_LEFT_POST: // Intentional fall throughs
    case BLUE_GOAL_RIGHT_POST:
    case YELLOW_GOAL_LEFT_POST:
    case YELLOW_GOAL_RIGHT_POST:
    case YELLOW_GOAL_POST:
    case BLUE_GOAL_POST:
        return true;
    default:
        return false;
    }
}

const float ConcreteFieldObject::getHeightFromGround(const int _id)
{
    return 0;
}

}
}
