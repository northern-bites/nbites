 #include "ObstacleModule.h"

namespace man {
namespace obstacle {

using messages::Obstacle;
using messages::ArmContactState;

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
    sonarIn.latch();

    Obstacle::ObstaclePosition sonars = processSonar(sonarIn.message());
    Obstacle::ObstaclePosition arms = processArms(armContactIn.message());

    portals::Message<messages::Obstacle> current(0);

    // agreement
    if (arms == sonars)
    {
        current.get()->set_position(arms);
    }
    // trust sonars first
    else if (arms == Obstacle::NONE)
    {
        current.get()->set_position(sonars);
    }
    // almost agreement
    else if (sonars == Obstacle::NORTH && (arms == Obstacle::NORTHWEST ||
                                           arms == Obstacle::NORTHEAST))
    {
        current.get()->set_position(arms);
    }
    else if (arms == Obstacle::NORTH &&  (sonars == Obstacle::NORTHWEST ||
                                          sonars == Obstacle::NORTHEAST))
    {
        current.get()->set_position(sonars);
    }
    // confused, trust arms
    else
    {
        current.get()->set_position(arms);
    }

    obstacleOut.setMessage(current);
}

Obstacle::ObstaclePosition
ObstacleModule::processArms(const messages::ArmContactState& input)
{
    if ((input.right_push_direction() ==
         ArmContactState::SOUTH ||
         input.right_push_direction() ==
         ArmContactState::SOUTHEAST) &&
        (input.left_push_direction() ==
         ArmContactState::SOUTH ||
         input.left_push_direction() ==
         ArmContactState::SOUTHWEST))
    {
        return Obstacle::NORTH;
    }
    else if ((input.right_push_direction() ==
              ArmContactState::NORTH ||
              input.right_push_direction() ==
              ArmContactState::NORTHEAST) &&
             (input.left_push_direction() ==
              ArmContactState::NORTH ||
              input.left_push_direction() ==
              ArmContactState::NORTHWEST))
    {
        return Obstacle::SOUTH;
    }
    else if (input.right_push_direction() ==
             ArmContactState::NONE)
    {
        if (input.left_push_direction() ==
                 ArmContactState::NORTH ||
                 input.left_push_direction() ==
                 ArmContactState::NORTHEAST)
        {
            return Obstacle::SOUTHWEST;
        }
        else if (input.left_push_direction() ==
             ArmContactState::EAST)
        {
            return Obstacle::WEST;
        }
        else if (input.left_push_direction() ==
                 ArmContactState::SOUTH ||
                 input.left_push_direction() ==
                 ArmContactState::SOUTHEAST)
        {
            return Obstacle::NORTHWEST;
        }
        else if (input.left_push_direction() ==
             ArmContactState::SOUTHWEST)
        {
            return Obstacle::NORTH;
        }
        else if (input.left_push_direction() ==
                 ArmContactState::WEST ||
                 input.left_push_direction() ==
                 ArmContactState::NORTHWEST)
        {
            return Obstacle::SOUTH;
        }
        else return Obstacle::NONE;
    }
    else if (input.left_push_direction() ==
             ArmContactState::NONE)
    {
        if (input.right_push_direction() ==
                 ArmContactState::NORTH ||
                 input.right_push_direction() ==
                 ArmContactState::NORTHWEST)
        {
            return Obstacle::SOUTHEAST;
        }
        else if (input.right_push_direction() ==
             ArmContactState::WEST)
        {
            return Obstacle::EAST;
        }
        else if (input.right_push_direction() ==
                 ArmContactState::SOUTH ||
                 input.right_push_direction() ==
                 ArmContactState::SOUTHWEST)
        {
            return Obstacle::NORTHEAST;
        }
        else if (input.right_push_direction() ==
             ArmContactState::SOUTHEAST)
        {
            return Obstacle::NORTH;
        }
        else if (input.right_push_direction() ==
                 ArmContactState::EAST ||
                 input.right_push_direction() ==
                 ArmContactState::NORTHEAST)
        {
            return Obstacle::SOUTH;
        }
        else return Obstacle::NONE;
    }

    return Obstacle::NONE;
}

Obstacle::ObstaclePosition
ObstacleModule::processSonar(const messages::SonarState& input)
{
    rightSonars.push_back(input.us_right());
    leftSonars.push_back(input.us_left());

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
