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

float minimum(std::list<float>& buf)
{
    float min = 500.f;

    for (std::list<float>::iterator i = buf.begin(); i != buf.end(); i++)
    {
        if (*i < min)
            min = *i;
    }

    return min;
}

ObstacleModule::ObstacleModule() : obstacleOut(base())
{
    memset(obstacleBuffer, 0, sizeof(obstacleBuffer));
    memset(obstacleDistances, 0, sizeof(obstacleDistances));

    obstaclesList[0] = FieldObstacles::Obstacle::NONE;
    obstaclesList[1] = FieldObstacles::Obstacle::NORTH;
    obstaclesList[2] = FieldObstacles::Obstacle::NORTHEAST;
    obstaclesList[3] = FieldObstacles::Obstacle::EAST;
    obstaclesList[4] = FieldObstacles::Obstacle::SOUTHEAST;
    obstaclesList[5] = FieldObstacles::Obstacle::SOUTH;
    obstaclesList[6] = FieldObstacles::Obstacle::SOUTHWEST;
    obstaclesList[7] = FieldObstacles::Obstacle::WEST;
    obstaclesList[8] = FieldObstacles::Obstacle::NORTHWEST;

    // obstacleBuffer = { 0 };
    // obstacleDistances = { 0.f };
    // Used when setting the message fields
    // obstaclesList = { FieldObstacles::Obstacle::NONE,
    //                   FieldObstacles::Obstacle::NORTH,
    //                   FieldObstacles::Obstacle::NORTHEAST,
    //                   FieldObstacles::Obstacle::EAST,
    //                   FieldObstacles::Obstacle::SOUTHEAST,
    //                   FieldObstacles::Obstacle::SOUTH,
    //                   FieldObstacles::Obstacle::SOUTHWEST,
    //                   FieldObstacles::Obstacle::WEST,
    //                   FieldObstacles::Obstacle::NORTHWEST
    // };
}

void ObstacleModule::run_()
{
    armContactIn.latch();
    visionIn.latch();
    sonarIn.latch();

    // Decide sonars
    // FieldObstacles::Obstacle::ObstaclePosition
    //     sonars = processSonar(sonarIn.message());

    // Process vision in all three sections of frame separately
    FieldObstacles::Obstacle::ObstaclePosition
        visionL = processVision(visionIn.message().left_dist(),
                                visionIn.message().left_bearing());
    FieldObstacles::Obstacle::ObstaclePosition
        visionM = processVision(visionIn.message().mid_dist(),
                                visionIn.message().mid_bearing());
    FieldObstacles::Obstacle::ObstaclePosition
        visionR = processVision(visionIn.message().right_dist(),
                                visionIn.message().right_bearing());

    // update obstacle buffer with new information from vision
    updateObstacleBuffer(visionL, visionM, visionR);

    // Decide arms
    FieldObstacles::Obstacle::ObstaclePosition
        arms = processArms(armContactIn.message());

    // Used to check if there were any obstacles found
    bool didReturn = false;
    portals::Message<messages::FieldObstacles> current(0);

    // std::cout<<"OBSTACLE: ";

    // ignore "NONE" direction, start at 1
    for (int i = 1; i < NUM_DIRECTIONS; i++)
    {
        if (obstacleBuffer[i] != 0)
        {
            FieldObstacles::Obstacle* temp = current.get()->add_obstacle();
            temp->set_position(obstaclesList[i]);
            temp->set_distance(obstacleDistances[i]);
            // std::cout<<obstacleDistances[i]<<", ";
            didReturn = true;
        }
    }
    // std::cout<<std::endl;

    FieldObstacles::Obstacle*temp = current.get()->add_obstacle();
    temp->set_position(arms);
    temp->set_distance(1.f);

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
         ArmContactState::SOUTHEAST ||
         input.right_push_direction() ==
         ArmContactState::SOUTHWEST) &&
        (input.left_push_direction() ==
         ArmContactState::SOUTH ||
         input.left_push_direction() ==
         ArmContactState::SOUTHWEST ||
         input.left_push_direction() ==
         ArmContactState::SOUTHEAST))
    {
        return FieldObstacles::Obstacle::NORTH;
    }
    // Both arms pushed approximately forward... something is behind
    else if ((input.right_push_direction() ==
              ArmContactState::NORTH ||
              input.right_push_direction() ==
              ArmContactState::NORTHEAST ||
              input.right_push_direction() ==
              ArmContactState::NORTHWEST) &&
             (input.left_push_direction() ==
              ArmContactState::NORTH ||
              input.left_push_direction() ==
              ArmContactState::NORTHWEST ||
              input.left_push_direction() ==
              ArmContactState::NORTHEAST))
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

FieldObstacles::Obstacle::ObstaclePosition
ObstacleModule::processVision(float distance, float bearing)
{
    FieldObstacles::Obstacle::ObstaclePosition dir;
    float avg;

    // Process what direction it is in: act appropriately
    if ( bearing < -5.f*ZONE_WIDTH)
    {
        // obstacle to the southeast
        SEDists.push_back(distance);
        if (SEDists.size() > VISION_FRAMES_TO_BUFFER)
        {
            SEDists.pop_front();
        }
        dir = FieldObstacles::Obstacle::SOUTHEAST;
        avg = average(SEDists);
        obstacleDistances[int(dir)] = avg;
        return dir;
    }
    else if ( bearing < -3.f*ZONE_WIDTH )
    {
        // obstacle to the east
        EDists.push_back(distance);
        if (EDists.size() > VISION_FRAMES_TO_BUFFER)
        {
            EDists.pop_front();
        }

        dir = FieldObstacles::Obstacle::EAST;
        avg = average(EDists);
        obstacleDistances[int(dir)] = avg;

        return dir;
    }
    else if ( bearing < -ZONE_WIDTH )
    {
        // obstacle to northeast
        NEDists.push_back(distance);
        if (NEDists.size() > VISION_FRAMES_TO_BUFFER)
        {
            NEDists.pop_front();
        }

        dir = FieldObstacles::Obstacle::NORTHEAST;
        avg = average(NEDists);
        obstacleDistances[int(dir)] = avg;

        return dir;
    }
    else if ( bearing < ZONE_WIDTH )
    {
        // obstacle to north
        NDists.push_back(distance);
        if (NDists.size() > VISION_FRAMES_TO_BUFFER)
        {
            NDists.pop_front();
        }

        dir = FieldObstacles::Obstacle::NORTH;
        avg = average(NDists);
        obstacleDistances[int(dir)] = avg;

        return dir;
    }
    else if ( bearing < 3.f*ZONE_WIDTH )
    {
        // obstacle to northwest
        NWDists.push_back(distance);
        if (NWDists.size() > VISION_FRAMES_TO_BUFFER)
        {
            NWDists.pop_front();
        }

        dir = FieldObstacles::Obstacle::NORTHWEST;
        avg = average(NWDists);
        obstacleDistances[int(dir)] = avg;

        return dir;
    }
    else if ( bearing < 5.f*ZONE_WIDTH )
    {
        // obstacle to west
        WDists.push_back(distance);
        if (WDists.size() > VISION_FRAMES_TO_BUFFER)
        {
            WDists.pop_front();
        }

        dir = FieldObstacles::Obstacle::WEST;
        avg = average(WDists);
        obstacleDistances[int(dir)] = avg;

        return dir;
    }
    else if ( bearing < 7.f * ZONE_WIDTH )
    {
        // obstacle to southwest
        SWDists.push_back(distance);
        if (SWDists.size() > VISION_FRAMES_TO_BUFFER)
        {
            SWDists.pop_front();
        }

        dir = FieldObstacles::Obstacle::SOUTHWEST;
        avg = average(SWDists);
        obstacleDistances[int(dir)] = avg;

        return dir;
    }
    else // south
    {
        return FieldObstacles::Obstacle::NONE;
    }
}

void ObstacleModule::updateObstacleBuffer
(FieldObstacles::Obstacle::ObstaclePosition visionL,
 FieldObstacles::Obstacle::ObstaclePosition visionM,
 FieldObstacles::Obstacle::ObstaclePosition visionR)
{
    // std::cout<<"Obstacle Buffer: ";
    // start at 1 to ignore "NONE" direction
    for (int i = 1; i < NUM_DIRECTIONS; i++)
    {
        if ( i == int(visionL) || i == int(visionM) ||
             i == int(visionR) )
        {
            obstacleBuffer[i] = 1;
            // std::cout<<obstacleBuffer[i]<<", ";
            continue;
        }
        if (obstacleBuffer[i] == 0 or
            obstacleBuffer[i] > VISION_FRAMES_TO_HAVE_OBSTACLE)
        {
            obstacleBuffer[i] = 0;
            obstacleDistances[i] = 0;
            // std::cout<<obstacleBuffer[i]<<", ";
            continue;
        }
        obstacleBuffer[i]++;
        // std::cout<<obstacleBuffer[i]<<", ";
    }
    // std::cout<<std::endl;
}

} // namespace obstacle
} // namespace man
