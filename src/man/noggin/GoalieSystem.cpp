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
}

void GoalieSystem::pushRightPostObservation(float distance, float bearing)
{
    rightPostDistances.push_back(distance);
    rightPostBearings.push_back(bearing);
}

void GoalieSystem::pushLeftPostObservation(float distance, float bearing)
{
    leftPostDistances.push_back(distance);
    leftPostBearings.push_back(bearing);
}

void GoalieSystem::popRightPostObservation()
{
    rightPostDistances.pop_front(distance);
    rightPostBearings.pop_front(bearing);
}

void GoalieSystem::popLeftPostObservation()
{
    leftPostDistances.pop_front(distance);
    leftPostBearings.pop_front(bearing);
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
    return leftPostDistance() * cos(leftPostBearing() * TO_RAD);
}

float GoalieSystem::leftPostRelY()
{
    return leftPostDistance() * cos(leftPostBearing() * TO_RAD);
}

float GoalieSystem::leftPostRelX()
{
    return leftPostDistance() * sin(leftPostBearing() * TO_RAD);
}

float GoalieSystem::rightPostRelY()
{
    return rightPostDistance() * cos(rightPostBearing() * TO_RAD);
}

float GoalieSystem::rightPostRelX()
{
    return rightPostDistance() * sin(rightPostBearing() * TO_RAD);
}

float GoalieSystem::centerGoalBearingAvg()
{
    return (leftPostBearing() + rightPostBearing()) / 2.f;
}

float GoalieSystem::centerGoalDistanceAvg()
{
    return (leftPostDistance() + rightPostDistancd()) / 2.f;
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
    for(deque<float>::iterator i = q.begin(); i != q.end(); i++)
    {
        sum += *i;
    }
    return sum / q.size();
}
}
