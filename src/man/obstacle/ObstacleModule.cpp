#include "ObstacleModule.h"

namespace man {
namespace obstacle {

const bool USING_ARMS = false;
const bool USING_SONARS = true;
const bool USING_VISION = false;

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
    obstaclesList[1] = FieldObstacles::Obstacle::N;
    obstaclesList[2] = FieldObstacles::Obstacle::NE;
    obstaclesList[3] = FieldObstacles::Obstacle::E;
    obstaclesList[4] = FieldObstacles::Obstacle::SE;
    obstaclesList[5] = FieldObstacles::Obstacle::S;
    obstaclesList[6] = FieldObstacles::Obstacle::SW;
    obstaclesList[7] = FieldObstacles::Obstacle::W;
    obstaclesList[8] = FieldObstacles::Obstacle::NW;
}

void ObstacleModule::run_()
{
    armContactIn.latch();
    visionIn.latch();
    sonarIn.latch();

    FieldObstacles::Obstacle::ObstaclePosition sonars, visionL, visionM, visionR, arms;

    // Decide sonars
    if (USING_SONARS) { sonars = processSonar(sonarIn.message()); } 
    else { sonars = FieldObstacles::Obstacle::NONE; }

#ifdef USE_LAB_FIELD // Walls are too close to field for sonar use
    sonars = FieldObstacles::Obstacle::NONE;
#endif

    // Process vision in all three sections of frame separately
    if (USING_VISION) {
        visionL = processVision(visionIn.message().left_dist(),
                                visionIn.message().left_bearing());
        visionM = processVision(visionIn.message().mid_dist(),
                                visionIn.message().mid_bearing());
        visionR = processVision(visionIn.message().right_dist(),
                                visionIn.message().right_bearing());
        // update obstacle buffer with new information from vision
        updateObstacleBuffer(visionL, visionM, visionR);
    } else {
        updateObstacleBuffer(FieldObstacles::Obstacle::NONE,
                             FieldObstacles::Obstacle::NONE,
                             FieldObstacles::Obstacle::NONE);
    }

    // Decide arms
    if (USING_ARMS) { arms = processArms( armContactIn.message()); }
    else { arms = FieldObstacles::Obstacle::NONE; }




/*
 * This is not fully implemented:
 *      Currently only uses vision if obstacles and sonars find
 *      no obstacle. Otherwise, combines sonar and arm input.
 */
    // How do we combine the arm and sonar decisions?
    // If they agree, easy
    if (arms == sonars)
    {
        // temp.get()->set_position(arms);
        obstacleBuffer[(int)arms] = VISION_FRAMES_TO_HAVE_OBSTACLE+5;
    }
    // Trust sonars before we get any arm input
    else if (arms == FieldObstacles::Obstacle::NONE)
    {
        // temp.get()->set_position(sonars);
        obstacleBuffer[(int)sonars] = VISION_FRAMES_TO_HAVE_OBSTACLE+5;
        std::cout<<"SONARS: ";
    }
    // If they sort of agree, use the value that gives us better dodging info
    else if (sonars == FieldObstacles::Obstacle::N && 
             (arms == FieldObstacles::Obstacle::NW ||
              arms == FieldObstacles::Obstacle::NE))
    {
        // temp.get()->set_position(arms);
        obstacleBuffer[(int)arms] = VISION_FRAMES_TO_HAVE_OBSTACLE+5;
    }
    else if (arms == FieldObstacles::Obstacle::N &&
             (sonars == FieldObstacles::Obstacle::NW || 
              sonars == FieldObstacles::Obstacle::NE))
    {
        // temp.get()->set_position(sonars);
        obstacleBuffer[(int)sonars] = VISION_FRAMES_TO_HAVE_OBSTACLE+5;
        std::cout<<"SONARS: ";
    }
    // If they don't agree or get no sonars, trust the arms
    else
    {
        // temp.get()->set_position(arms);
        obstacleBuffer[(int)arms] = VISION_FRAMES_TO_HAVE_OBSTACLE+5;
    }


    portals::Message<messages::FieldObstacles> current(0);


    // std::cout<<"OBSTACLE: ";

    // bool didReturn = false;
    // ignore "NONE" direction, start at 1
    for (int i = 1; i < NUM_DIRECTIONS; i++)
    {
        if (obstacleBuffer[i]==0) { continue; } //no obstacle here
        
        FieldObstacles::Obstacle* temp = current.get()->add_obstacle();
        temp->set_position(obstaclesList[i]);
        if (obstacleBuffer[i] == VISION_FRAMES_TO_HAVE_OBSTACLE+5) {
            temp->set_distance(0.f); //arms or sonars don't have distance
            std::cout<<"OBSTACLE AT "<<i<<std::endl;
        } else {
            temp->set_distance(obstacleDistances[i]);
        }

        // std::cout<<obstacleDistances[i]<<", ";
        // didReturn = true;
    }
    // std::cout<<std::endl;

    obstacleOut.setMessage(current);
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
        return FieldObstacles::Obstacle::N;
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
        return FieldObstacles::Obstacle::S;
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
            return FieldObstacles::Obstacle::SW;
        }
        else if (input.left_push_direction() ==
             ArmContactState::EAST)
        {
            return FieldObstacles::Obstacle::W;
        }
        else if (input.left_push_direction() ==
                 ArmContactState::SOUTH ||
                 input.left_push_direction() ==
                 ArmContactState::SOUTHEAST)
        {
            return FieldObstacles::Obstacle::NW;
        }
        else if (input.left_push_direction() ==
             ArmContactState::SOUTHWEST)
        {
            return FieldObstacles::Obstacle::N;
        }
        else if (input.left_push_direction() ==
                 ArmContactState::WEST ||
                 input.left_push_direction() ==
                 ArmContactState::NORTHWEST)
        {
            return FieldObstacles::Obstacle::S;
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
            return FieldObstacles::Obstacle::SE;
        }
        else if (input.right_push_direction() ==
             ArmContactState::WEST)
        {
            return FieldObstacles::Obstacle::E;
        }
        else if (input.right_push_direction() ==
                 ArmContactState::SOUTH ||
                 input.right_push_direction() ==
                 ArmContactState::SOUTHWEST)
        {
            return FieldObstacles::Obstacle::NE;
        }
        else if (input.right_push_direction() ==
             ArmContactState::SOUTHEAST)
        {
            return FieldObstacles::Obstacle::N;
        }
        else if (input.right_push_direction() ==
                 ArmContactState::EAST ||
                 input.right_push_direction() ==
                 ArmContactState::NORTHEAST)
        {
            return FieldObstacles::Obstacle::S;
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
    std::cout<<"RIGHT = "<<right<<" , LEFT = "<<left<<std::endl;

    // Both sonars picking up an obstacle? It's probably in front
    if (right < SONAR_FRONT_THRESH && left < SONAR_FRONT_THRESH &&
        right != 0.f && left!= 0.f)
    {
        return FieldObstacles::Obstacle::N;
    }
    // Otherwise to right side...
    else if (right < SONAR_THRESH && right != 0.f)
    {
        return FieldObstacles::Obstacle::NE;
    }
    // ... left side ...
    else if (left < SONAR_THRESH && left != 0.f)
    {
        return FieldObstacles::Obstacle::NW;
    }
    // .. or no obstacle
    else return FieldObstacles::Obstacle::NONE;
}

// Very much a hack!
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
        dir = FieldObstacles::Obstacle::SE;
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

        dir = FieldObstacles::Obstacle::E;
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

        dir = FieldObstacles::Obstacle::NE;
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

        dir = FieldObstacles::Obstacle::N;
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

        dir = FieldObstacles::Obstacle::NW;
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

        dir = FieldObstacles::Obstacle::W;
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

        dir = FieldObstacles::Obstacle::SW;
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
