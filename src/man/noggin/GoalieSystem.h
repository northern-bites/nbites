/*
 * Keeps track of stuff for the Python player pGoalie. There's too much math
 * involved in this to keep it all in Python. Plus I hate working in Python.
 *
 * TODO: Make this system use existing combination objects instead of redoing
 * all of the math :(
 */

#pragma once

#include "CombinationObjects.h"
#include "NBMath.h"
#include "PyNogginConstants.h"
#include "FieldConstants.h"

#include <deque>

namespace noggin {

// How far memory should go back
static const float POST_Q_LENGTH = 20;
static const float CROSS_Q_LENGTH = 5;

// Field constants
static const float RIGHT_X = FIELD_WHITE_LEFT_SIDELINE_X +
                               LINE_CROSS_OFFSET;
static const float RIGHT_Y = FIELD_WHITE_BOTTOM_SIDELINE_Y;

static const float RIGHT_SIDE_LP_ANGLE =
    NBMath::safe_atan2(RIGHT_X - py_constants::LANDMARK_MY_GOAL_LEFT_POST_X,
                       py_constants::LANDMARK_MY_GOAL_LEFT_POST_Y -
                       RIGHT_Y) * TO_DEG;
static const float RIGHT_SIDE_LP_DISTANCE = NBMath::getHypotenuse(RIGHT_X -
                       py_constants::LANDMARK_MY_GOAL_LEFT_POST_X,
                       py_constants::LANDMARK_MY_GOAL_LEFT_POST_Y -
                                                          RIGHT_Y);

static const float RIGHT_SIDE_RP_ANGLE =
    NBMath::safe_atan2(RIGHT_X - py_constants::LANDMARK_MY_GOAL_RIGHT_POST_X,
                       py_constants::LANDMARK_MY_GOAL_RIGHT_POST_Y -
                       RIGHT_Y) * TO_DEG;

static const float RIGHT_SIDE_RP_DISTANCE = NBMath::getHypotenuse(RIGHT_X -
                       py_constants::LANDMARK_MY_GOAL_RIGHT_POST_X,
                       py_constants::LANDMARK_MY_GOAL_RIGHT_POST_Y -
                                                               RIGHT_Y);

static const float RIGHT_SIDE_ANGLE = (RIGHT_SIDE_LP_ANGLE +
                                       RIGHT_SIDE_RP_ANGLE) / 2.f;

static const float LEFT_X = RIGHT_X;
static const float LEFT_Y = FIELD_WHITE_TOP_SIDELINE_Y;
static const float LEFT_SIDE_LP_ANGLE = - RIGHT_SIDE_RP_ANGLE;
static const float LEFT_SIDE_RP_ANGLE = - RIGHT_SIDE_LP_ANGLE;
static const float LEFT_SIDE_LP_DISTANCE = RIGHT_SIDE_RP_DISTANCE;
static const float LEFT_SIDE_RP_DISTANCE = RIGHT_SIDE_LP_DISTANCE;

static const float LEFT_SIDE_ANGLE = - RIGHT_SIDE_ANGLE;

class GoalieSystem
{
public:
    GoalieSystem();
    ~GoalieSystem() {};

    void resetPosts(float rightDistance, float rightBearing,
                    float leftDistance, float leftBearing);

    // Methods for controlling "memory"
    void pushRightPostObservation(float distance, float bearing);
    void pushLeftPostObservation(float distance, float bearing);
    void pushCrossObservation(float bearing);

    void popRightPostObservation();
    void popLeftPostObservation();
    void popCrossObservation();

    // Methods for computing relative positions
    float leftPostBearing();
    float rightPostBearing();
    float leftPostDistance();
    float rightPostDistance();
    float crossBearing();

    float leftPostRelX();
    float leftPostRelY();
    float rightPostRelX();
    float rightPostRelY();

    // Averages left and right post values
    float centerGoalDistanceAvg();
    float centerGoalBearingAvg();
    float centerGoalRelX();
    float centerGoalRelY();

    RelRobotLocation home;

private:
    std::deque<float> rightPostBearings;
    std::deque<float> rightPostDistances;
    std::deque<float> leftPostBearings;
    std::deque<float> leftPostDistances;
    std::deque<float> crossBearings;

    float computeAverage(std::deque<float>);
};
}
