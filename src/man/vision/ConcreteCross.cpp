#include "ConcreteCross.h"
using namespace std;

// Constructors
ConcreteCross::ConcreteCross(const float _fieldX,
                                         const float _fieldY,
                                         const crossID _id)
    : ConcreteLandmark(_fieldX, _fieldY), id(_id) {}

// Copy constructor - just pairwise copy the elements
ConcreteCross::ConcreteCross(const ConcreteCross& other)
    : ConcreteLandmark(other), id(other.id) {}
// No memory to free
ConcreteCross::~ConcreteCross() {}

///////////////////////////////////////////////////////////////////////////////
// Constants for absolute coordinates on the field of the specific cross.
// (0,0) is the lower left cross of the field when BLUE goal is at the
// bottom
///////////////////////////////////////////////////////////////////////////////

const ConcreteCross ConcreteCross::blue_goal_cross(
    LANDMARK_BLUE_GOAL_CROSS_X, LANDMARK_BLUE_GOAL_CROSS_Y,
    BLUE_GOAL_CROSS);
const ConcreteCross ConcreteCross::yellow_goal_cross(
    LANDMARK_YELLOW_GOAL_CROSS_X, LANDMARK_YELLOW_GOAL_CROSS_Y,
    YELLOW_GOAL_CROSS);

const ConcreteCross* ConcreteCross::concreteCrossList[NUM_FIELD_CROSSES] =
{&blue_goal_cross, &yellow_goal_cross};

const ConcreteCross* ConcreteCross::FIELD_CROSSES[NUM_FIELD_CROSSES] =
{&blue_goal_cross, &yellow_goal_cross};
const ConcreteCross* ConcreteCross::BLUE_GOAL_CROSS_LIST[1] =
{&blue_goal_cross};
const ConcreteCross* ConcreteCross::YELLOW_GOAL_CROSS_LIST[1] =
{&yellow_goal_cross};

const list <const ConcreteCross*> ConcreteCross::fieldCrosses =
    list <const ConcreteCross*>( ConcreteCross::FIELD_CROSSES,
                                 &ConcreteCross::FIELD_CROSSES[
                                     NUM_FIELD_CROSSES]);

// Lists of single posts, to make the functions easier to call...
const list <const ConcreteCross*> ConcreteCross::blueGoalCrossList =
    list <const ConcreteCross*>( ConcreteCross::BLUE_GOAL_CROSS_LIST,
                                 &ConcreteCross::BLUE_GOAL_CROSS_LIST[1]);
const list <const ConcreteCross*> ConcreteCross::yellowGoalCrossList =
    list <const ConcreteCross*>( ConcreteCross::YELLOW_GOAL_CROSS_LIST,
                                 &ConcreteCross::YELLOW_GOAL_CROSS_LIST[1]);
const list <const ConcreteCross*> ConcreteCross::abstractCrossList =
    list <const ConcreteCross*>( ConcreteCross::concreteCrossList,
                                 &ConcreteCross::concreteCrossList[
                                     NUM_FIELD_CROSSES]);

const string ConcreteCross::toString() const {
    switch(id) {
    case BLUE_GOAL_CROSS:
        return "Blue goal field cross";
    case YELLOW_GOAL_CROSS:
        return "Yellow goal field cross";
    case ABSTRACT_CROSS:
        return "Abstract field cross";
    default:
        return "Invalid ID";
    }

}

const string ConcreteCross::getStringFromID(const int testID) {
    switch (testID) {
    case BLUE_GOAL_CROSS:
        return "Blue goal field cross";
    case YELLOW_GOAL_CROSS:
        return "Yellow goal cross";
    case ABSTRACT_CROSS:
        return "Abstract field cross";
    default:
        return "Invalid ID";
    }
}

const float ConcreteCross::getHeightFromGround(const int _id)
{
    return 0;
}

