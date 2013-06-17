#include "ObstacleModule.h"

namespace man {
namespace obstacle {

using messages::Obstacle;
using messages::ArmContactState;

// Helper method, used to get the average of the sonar value buffers
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
    //visionIn.latch();
    sonarIn.latch();

    // Decide sonars and arms separately
    Obstacle::ObstaclePosition sonars = processSonar(sonarIn.message());
    Obstacle::ObstaclePosition arms = processArms(armContactIn.message());

    portals::Message<messages::Obstacle> current(0);

    // How do we combine the two decisions?
    // If they agree, easy
    if (arms == sonars)
    {
        current.get()->set_position(arms);
    }
    // Trust sonars before we get any arm input
    else if (arms == Obstacle::NONE)
    {
        current.get()->set_position(sonars);
    }
    // If they sort of agree, use the value that gives us better dodging info
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
    // If they don't agree or get no sonars, trust the arms
    else
    {
        current.get()->set_position(arms);
    }

    obstacleOut.setMessage(current);
}

Obstacle::ObstaclePosition
ObstacleModule::processArms(const messages::ArmContactState& input)
{
    // Both arms pushed approximately backwards...something is in front
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
    // Both arms pushed approximately forward... something is behind
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
    // Not getting pushed on right arm... decide based on left
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
    // Not getting pushed on left arm... decide based on right
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

    // Potential issue: if both arms are pushed in disagreeing directions,
    // we just decide no obstacle.

    return Obstacle::NONE;
}

Obstacle::ObstaclePosition
ObstacleModule::processSonar(const messages::SonarState& input)
{
    // Buffer the current sonar values
    rightSonars.push_back(input.us_right());
    leftSonars.push_back(input.us_left());

    // Get rid of old values
    if (rightSonars.size() > SONAR_FRAMES_TO_BUFFER)
    {
        rightSonars.pop_front();
    }

    if (leftSonars.size() > SONAR_FRAMES_TO_BUFFER)
    {
        leftSonars.pop_front();
    }

    // Use the current average for our decision
    float right = average(rightSonars);
    float left = average(leftSonars);

    // Both sonars picking up an obstacle? It's probably in front
    if (right < SONAR_THRESH && left < SONAR_THRESH)
    {
        return Obstacle::NORTH;
    }
    // Otherwise to right side...
    else if (right < SONAR_THRESH)
    {
        return Obstacle::NORTHEAST;
    }
    // ... left side ...
    else if (left < SONAR_THRESH)
    {
        return Obstacle::NORTHWEST;
    }
    // .. or no obstacle
    else return Obstacle::NONE;
}

}
}
