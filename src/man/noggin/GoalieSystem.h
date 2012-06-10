/*
 * Keeps track of stuff for the Python player pGoalie. There's too much math
 * involved in this to keep it all in Python. Plus I hate working in Python.
 */

#pragma once

#include "CombinationObjects.h"
#include "NBMath.h"
#include "PyNogginConstants.h"
#include "FieldConstants.h"

#include <deque>

namespace noggin {

// How far memory should go back
static const float DEQUE_LENGTH = 50;

// Field constants
static const float INITIAL_X = FIELD_WHITE_LEFT_SIDELINE_X +
                               LINE_CROSS_OFFSET;
static const float INITIAL_Y = FIELD_WHITE_BOTTOM_SIDELINE_Y;
static const float INITIAL_ANGLE =
    NBMath::safe_atan2(INITIAL_X - py_constants::LANDMARK_MY_GOAL_LEFT_POST_X,
                       py_constants::LANDMARK_MY_GOAL_LEFT_POST_Y -
                       INITIAL_Y) * TO_DEG;

class GoalieSystem
{
public:
    GoalieSystem();
    ~GoalieSystem() {};

    // Methods for controlling "memory"
    void pushRightPostObservation(float distance, float bearing);
    void pushLeftPostObservation(float distance, float bearing);

    void popRightPostObservation();
    void popLeftPostObservation();

    // Methods for computing relative positions
    float leftPostBearing();
    float rightPostBearing();
    float leftPostDistance();
    float rightPostDistance();

    float leftPostRelX();
    float leftPostRelY();
    float rightPostRelX();
    float rightPostRelY();

    // Averages left and right post bearings
    float centerGoalDistanceAvg();
    float centerGoalBearingAvg();

    // Computes expected location of center of goal based on both posts
    // then averages them
    float centerGoalRelX();
    float centerGoalRelY();

    RelLocation home;

private:
    std::deque<float> rightPostBearings;
    std::deque<float> rightPostDistances;
    std::deque<float> leftPostBearings;
    std::deque<float> leftPostDistances;

    float computeAverage(std::deque<float>);
};
}
