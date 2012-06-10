/*
 * GoalieSystem
 */

#include "GoalieSystem.h"

namespace noggin {

GoalieSystem::GoalieSystem() : rightPostBearings(),
                               rightPostDistances(),
                               leftPostBearings(),
                               leftPostDistances(),
                               home(CENTER_FIELD_Y - INITIAL_Y,
                                    LINE_CROSS_OFFSET)
{
    // need to fix these values
    pushRightPostObservation(300.f, 30.f);
    pushLeftPostObservation(300.f, 30.f);
}

void GoalieSystem::pushRightPostObservation(float distance, float bearing)
{
    rightPostDistances.push_back(distance);
    rightPostBearings.push_back(bearing);
    if(rightPostDistances.size() > DEQUE_LENGTH)
        popRightPostObservation();
}

void GoalieSystem::pushLeftPostObservation(float distance, float bearing)
{
    leftPostDistances.push_back(distance);
    leftPostBearings.push_back(bearing);
    if(leftPostDistances.size() > DEQUE_LENGTH)
        popLeftPostObservation();
}

void GoalieSystem::popRightPostObservation()
{
    rightPostDistances.pop_front();
    rightPostBearings.pop_front();
}

void GoalieSystem::popLeftPostObservation()
{
    leftPostDistances.pop_front();
    leftPostBearings.pop_front();
}

float GoalieSystem::leftPostBearing()
{
    return computeAverage(leftPostBearings);
}

float GoalieSystem::leftPostDistance()
{
    return computeAverage(leftPostDistances);
}

float GoalieSystem::rightPostBearing()
{
    return computeAverage(rightPostBearings);
}

float GoalieSystem::rightPostDistance()
{
    return computeAverage(rightPostDistances);
}

float GoalieSystem::leftPostRelX()
{
    return leftPostDistance() * std::cos(leftPostBearing() * TO_RAD);
}

float GoalieSystem::leftPostRelY()
{
    return leftPostDistance() * std::sin(leftPostBearing() * TO_RAD);
}

float GoalieSystem::rightPostRelX()
{
    return rightPostDistance() * std::cos(rightPostBearing() * TO_RAD);
}

float GoalieSystem::rightPostRelY()
{
    return rightPostDistance() * std::sin(rightPostBearing() * TO_RAD);
}

float GoalieSystem::centerGoalBearingAvg()
{
    return (leftPostBearing() + rightPostBearing()) / 2.f;
}

float GoalieSystem::centerGoalDistanceAvg()
{
    return (leftPostDistance() + rightPostDistance()) / 2.f;
}

float GoalieSystem::centerGoalRelX()
{
    return ((leftPostRelX() - 70.f) + (rightPostRelX() + 70.f)) / 2.f;
}

float GoalieSystem::centerGoalRelY()
{
    return (leftPostRelY() + rightPostRelY()) / 2.f;
}

float GoalieSystem::computeAverage(std::deque<float> q)
{
    float sum = 0;
    for(std::deque<float>::iterator i = q.begin(); i != q.end(); i++)
    {
        sum += *i;
    }
    return sum / q.size();
}
}
