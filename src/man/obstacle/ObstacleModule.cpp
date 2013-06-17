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

ObstacleModule::ObstacleModule() : obstacleOut(base())
{
}

void ObstacleModule::run_()
{
    armContactIn.latch();
    visionIn.latch();
    footBumperIn.latch();
    sonarIn.latch();

    portals::Message<messages::Obstacle> current(0);

    current.get()->set_position(processSonar(sonarIn.message()));
    obstacleOut.setMessage(current);
}

Obstacle::ObstaclePosition ObstacleModule::processArms(messages::ArmContactState& input)
{
    return Obstacle::NONE;
}

Obstacle::ObstaclePosition ObstacleModule::processSonar(const messages::SonarState& input)
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
