#include "ObstacleModule.h"

namespace man {
namespace obstacle {

using messages::FieldObstacles;
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
    visionObstacleBuffer = { 0 };
    visionObstacleDistances = { 0.f };
    // Used when setting the message fields
    obstaclesList = { FieldObstacles::Obstacle::NONE,
                      FieldObstacles::Obstacle::NORTH,
                      FieldObstacles::Obstacle::NORTHEAST,
                      FieldObstacles::Obstacle::EAST,
                      FieldObstacles::Obstacle::SOUTHEAST,
                      FieldObstacles::Obstacle::SOUTH,
                      FieldObstacles::Obstacle::SOUTHWEST,
                      FieldObstacles::Obstacle::WEST,
                      FieldObstacles::Obstacle::NORTHWEST
    };
}

void ObstacleModule::run_()
{
    armContactIn.latch();
    visionIn.latch();
    sonarIn.latch();

    // Decide sonars and arms separately
    // FieldObstacles::Obstacle::ObstaclePosition
        // sonars = processSonar(sonarIn.message());
    FieldObstacles::Obstacle::ObstaclePosition
        vision = processVision(visionIn.message());
    FieldObstacles::Obstacle::ObstaclePosition
        arms = processArms(armContactIn.message());

    // Updates vision obstacle buffer with information taken from vision
    updateVisionObstacleBuffer(vision);

    // Used to check if there were any obstacles found
    bool didReturn = false;
    portals::Message<messages::FieldObstacles> current(0);

    // ignore "NONE" direction, start at 1
    for (int i = 1; i < NUM_DIRECTIONS; i++)
    {
        if (visionObstacleBuffer[i] != 0 or int(arms) == i)
        {
            // adds an obstacle in this direction to the message
            FieldObstacles::Obstacle* temp = current.get()->add_obstacle();
            temp->set_position(obstaclesList[i]);
            temp->set_distance(visionObstacleDistances[i]);
            didReturn = true;
        }
    }

    if (!(didReturn))
    {
        FieldObstacles::Obstacle*temp = current.get()->add_obstacle();
        temp->set_position(FieldObstacles::Obstacle::NONE);
        temp->set_distance(0.f);
    }

    obstacleOut.setMessage(current);

/*
 * This code processes both sonars and arms, then returns where
 * a single obstacle is based on information gathered from both inputs.
 * It does not use vision at all.
 *
#ifdef USE_LAB_FIELD // Walls are too close to field for sonar use
    sonars = Obstacle::NONE;
#endif

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
*/
}

FieldObstacles::Obstacle::ObstaclePosition
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
        return FieldObstacles::Obstacle::NORTH;
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
        return FieldObstacles::Obstacle::SOUTH;
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
            return FieldObstacles::Obstacle::SOUTHWEST;
        }
        else if (input.left_push_direction() ==
             ArmContactState::EAST)
        {
            return FieldObstacles::Obstacle::WEST;
        }
        else if (input.left_push_direction() ==
                 ArmContactState::SOUTH ||
                 input.left_push_direction() ==
                 ArmContactState::SOUTHEAST)
        {
            return FieldObstacles::Obstacle::NORTHWEST;
        }
        else if (input.left_push_direction() ==
             ArmContactState::SOUTHWEST)
        {
            return FieldObstacles::Obstacle::NORTH;
        }
        else if (input.left_push_direction() ==
                 ArmContactState::WEST ||
                 input.left_push_direction() ==
                 ArmContactState::NORTHWEST)
        {
            return FieldObstacles::Obstacle::SOUTH;
        }
        else return FieldObstacles::Obstacle::NONE;
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
            return FieldObstacles::Obstacle::SOUTHEAST;
        }
        else if (input.right_push_direction() ==
             ArmContactState::WEST)
        {
            return FieldObstacles::Obstacle::EAST;
        }
        else if (input.right_push_direction() ==
                 ArmContactState::SOUTH ||
                 input.right_push_direction() ==
                 ArmContactState::SOUTHWEST)
        {
            return FieldObstacles::Obstacle::NORTHEAST;
        }
        else if (input.right_push_direction() ==
             ArmContactState::SOUTHEAST)
        {
            return FieldObstacles::Obstacle::NORTH;
        }
        else if (input.right_push_direction() ==
                 ArmContactState::EAST ||
                 input.right_push_direction() ==
                 ArmContactState::NORTHEAST)
        {
            return FieldObstacles::Obstacle::SOUTH;
        }
        else return FieldObstacles::Obstacle::NONE;
    }

    // Potential issue: if both arms are pushed in disagreeing directions,
    // we just decide no obstacle.

    return FieldObstacles::Obstacle::NONE;
}

FieldObstacles::Obstacle::ObstaclePosition
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
        return FieldObstacles::Obstacle::NORTH;
    }
    // Otherwise to right side...
    else if (right < SONAR_THRESH)
    {
        return FieldObstacles::Obstacle::NORTHEAST;
    }
    // ... left side ...
    else if (left < SONAR_THRESH)
    {
        return FieldObstacles::Obstacle::NORTHWEST;
    }
    // .. or no obstacle
    else return FieldObstacles::Obstacle::NONE;
}

// Helper method, used to figure out if vision buffers have good values
FieldObstacles::Obstacle::ObstaclePosition
ObstacleModule::checkAverage(FieldObstacles::Obstacle::ObstaclePosition direction,
             std::list<float> distances)
{
    float avg = average(distances);
    if (avg < VISION_MAX_DIST)
    {
        visionObstacleDistances[(int)direction] = avg;
        return direction;
    }
    return FieldObstacles::Obstacle::NONE;
}

FieldObstacles::Obstacle::ObstaclePosition
ObstacleModule::processVision(const messages::VisionObstacle& input)
{
    float minDist = std::min(std::min(input.left_dist(),
                                      input.mid_dist()),
                             input.right_dist());
    float bearing, avg;
    if (minDist == input.left_dist())
    {
        bearing = input.left_bearing();
    }
    else if (minDist == input.mid_dist())
    {
        bearing = input.mid_bearing();
    }
    else
    {
        bearing = input.right_bearing();
    }

    // now figure out in what direction the closest obstacle is
    if ( bearing < -5.f*ZONE_WIDTH)
    {
        // obstacle to the southeast
        SEDists.push_back(minDist);
        if (SEDists.size() > VISION_FRAMES_TO_BUFFER)
        {
            SEDists.pop_front();
        }

        return checkAverage(FieldObstacles::Obstacle::SOUTHEAST, SEDists);
    }
    else if ( bearing < -3.f*ZONE_WIDTH )
    {
        // obstacle to the east
        EDists.push_back(minDist);
        if (EDists.size() > VISION_FRAMES_TO_BUFFER)
        {
            EDists.pop_front();
        }

        return checkAverage(FieldObstacles::Obstacle::EAST, EDists);
    }
    else if ( bearing < -ZONE_WIDTH )
    {
        // obstacle to northeast
        NEDists.push_back(minDist);
        if (NEDists.size() > VISION_FRAMES_TO_BUFFER)
        {
            NEDists.pop_front();
        }

        return checkAverage(FieldObstacles::Obstacle::NORTHEAST, NEDists);
    }
    else if ( bearing < ZONE_WIDTH )
    {
        // obstacle to north
        NDists.push_back(minDist);
        if (NDists.size() > VISION_FRAMES_TO_BUFFER)
        {
            NDists.pop_front();
        }

        return checkAverage(FieldObstacles::Obstacle::NORTH, NDists);
    }
    else if ( bearing < 3.f*ZONE_WIDTH )
    {
        // obstacle to northwest
        NWDists.push_back(minDist);
        if (NWDists.size() > VISION_FRAMES_TO_BUFFER)
        {
            NWDists.pop_front();
        }

        return checkAverage(FieldObstacles::Obstacle::NORTHWEST, NWDists);
    }
    else if ( bearing < 5.f*ZONE_WIDTH )
    {
        // obstacle to west
        WDists.push_back(minDist);
        if (WDists.size() > VISION_FRAMES_TO_BUFFER)
        {
            WDists.pop_front();
        }

        return checkAverage(FieldObstacles::Obstacle::WEST, WDists);
    }
    else
    {
        // obstacle to southwest
        SWDists.push_back(minDist);
        if (SWDists.size() > VISION_FRAMES_TO_BUFFER)
        {
            WDists.pop_front();
        }

        return checkAverage(FieldObstacles::Obstacle::SOUTHWEST, SWDists);
    }
}

void ObstacleModule::updateVisionObstacleBuffer
(messages::FieldObstacles::Obstacle::ObstaclePosition vision)
{
    // start at 1 to ignore "NONE" direction
    for (int i = 1; i < NUM_DIRECTIONS; i++)
    {
        if (i == int(vision))
        {
            visionObstacleBuffer[i] = 1;
            continue;
        }
        if (visionObstacleBuffer[i] == 0 or
            visionObstacleBuffer[i] > VISION_FRAMES_TO_HAVE_OBSTACLE)
        {
            visionObstacleBuffer[i] = 0;
            visionObstacleDistances[i] = 0;
            continue;
        }
        visionObstacleBuffer[i]++;
    }
}

} // namespace obstacle
} // namespace man