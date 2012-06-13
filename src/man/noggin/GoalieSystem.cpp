/*
 * GoalieSystem
 */

#include "GoalieSystem.h"

namespace noggin {

GoalieSystem::GoalieSystem() : home(),
                               rightPostBearings(),
                               rightPostDistances(),
                               leftPostBearings(),
                               leftPostDistances()
{
    // to avoid a crash....
    pushRightPostObservation(0.f, 0.f);
    pushLeftPostObservation(0.f, 0.f);
    pushCrossObservation(0.f, 0.f);
}

void GoalieSystem::resetPosts(float rightDistance, float rightBearing,
                              float leftDistance, float leftBearing)
{
    rightPostBearings.clear();
    rightPostDistances.clear();
    leftPostBearings.clear();
    leftPostDistances.clear();
    pushRightPostObservation(rightDistance, rightBearing);
    pushLeftPostObservation(leftDistance, leftBearing);
}

void GoalieSystem::resetCross(float distance, float bearing)
{
    crossBearings.clear();
    crossDistances.clear();
    pushCrossObservation(distance, bearing);
}

void GoalieSystem::pushRightPostObservation(float distance, float bearing)
{
    rightPostDistances.push_back(distance);
    rightPostBearings.push_back(bearing);
    if(rightPostDistances.size() > POST_Q_LENGTH)
        popRightPostObservation();
}

void GoalieSystem::pushLeftPostObservation(float distance, float bearing)
{
    leftPostDistances.push_back(distance);
    leftPostBearings.push_back(bearing);
    if(leftPostDistances.size() > POST_Q_LENGTH)
        popLeftPostObservation();
}

void GoalieSystem::pushCrossObservation(float distance, float bearing)
{
    crossBearings.push_back(bearing);
    crossBearings.push_back(distance);
    if(crossBearings.size() > CROSS_Q_LENGTH)
        popCrossObservation();
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

void GoalieSystem::popCrossObservation()
{
    crossBearings.pop_front();
    crossDistances.pop_front();
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

float GoalieSystem::crossBearing()
{
    return computeAverage(crossBearings);
}

float GoalieSystem::crossDistance()
{
    return computeAverage(crossDistances);
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

float GoalieSystem::crossRelX()
{
    return crossDistance() * std::cos(crossBearing() * TO_RAD);
}

float GoalieSystem::crossRelY()
{
    return crossDistance() * std::sin(crossBearing() * TO_RAD);
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
    return (leftPostRelX() + rightPostRelX()) / 2.f;
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
    return sum / float(q.size());
}
}
