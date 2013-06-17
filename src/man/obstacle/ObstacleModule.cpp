#include "ObstacleModule.h"

namespace man {
namespace obstacle {

using messages::Obstacle;

float average(std::list<float>& buf)
{
    float avg = 0.f;

    for (std::list<float>::iterator i = buf.begin(); i != buf.end(); i++)
    {
        avg += *i;
    }

    avg /= float(buf.size());

    return avg;
}

int numberOutOf(std::list<bool>& buf)
{
    int num = 0;

    for (std::list<bool>::iterator i = buf.begin(); i != buf.end(); i++)
    {
        if (*i) num++;
    }

    return num;
}

ObstacleModule::ObstacleModule() : obstacleOut(base())
{
}

void ObstacleModule::run_()
{
    armContactIn.latch();
    visionIn.latch();
    footBumperIn.latch();
    sonarIn.latch();

    Obstacle::ObstaclePosition sonars = processSonar(sonarIn.message());
    //Obstacle::ObstaclePosition feet = processFeet(footBumperIn.message());

    portals::Message<messages::Obstacle> current(0);

    current.get()->set_position(sonars);
    obstacleOut.setMessage(current);
}

Obstacle::ObstaclePosition
ObstacleModule::processArms(const messages::ArmContactState& input)
{
    return Obstacle::NONE;
}

Obstacle::ObstaclePosition
ObstacleModule::processFeet(const messages::FootBumperState& input)
{
    rightFeet.push_back(footBumperIn.message().r_foot_bumper_left().pressed() ||
                        footBumperIn.message().r_foot_bumper_right().pressed());

    leftFeet.push_back(footBumperIn.message().l_foot_bumper_left().pressed() ||
                       footBumperIn.message().l_foot_bumper_right().pressed());

    if (rightFeet.size() > FEET_FRAMES_TO_BUFFER)
    {
        rightFeet.pop_front();
    }

    if (leftFeet.size() > FEET_FRAMES_TO_BUFFER)
    {
        leftFeet.pop_front();
    }

    int numLeftHits = numberOutOf(leftFeet);
    int numRightHits = numberOutOf(rightFeet);

    std::cout << "LEFT " << numLeftHits << ", RIGHT " << numRightHits
              << std::endl;

    return Obstacle::NONE;
}

Obstacle::ObstaclePosition
ObstacleModule::processSonar(const messages::SonarState& input)
{
    rightSonars.push_back(sonarIn.message().us_right());
    leftSonars.push_back(sonarIn.message().us_left());

    if (rightSonars.size() > SONAR_FRAMES_TO_BUFFER)
    {
        rightSonars.pop_front();
    }

    if (leftSonars.size() > SONAR_FRAMES_TO_BUFFER)
    {
        leftSonars.pop_front();
    }

    float right = average(rightSonars);
    float left = average(leftSonars);

    if (right < SONAR_THRESH && left < SONAR_THRESH)
    {
        return Obstacle::NORTH;
    }
    else if (right < SONAR_THRESH)
    {
        return Obstacle::NORTHEAST;
    }
    else if (left < SONAR_THRESH)
    {
        return Obstacle::NORTHWEST;
    }
    else return Obstacle::NONE;
}

}
}
