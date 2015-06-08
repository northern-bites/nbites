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

ObstacleModule::ObstacleModule(bool arm, bool vision) : obstacleOut(base())
{
    memset(obstacleBuffer, 0, sizeof(obstacleBuffer));
    memset(obstacleDistances, 0, sizeof(obstacleDistances));
    memset(obstacleDetectors, 0, sizeof(obstacleDetectors));

    obstaclesList[0] = FieldObstacles::Obstacle::NONE;
    obstaclesList[1] = FieldObstacles::Obstacle::NORTH;
    obstaclesList[2] = FieldObstacles::Obstacle::NORTHEAST;
    obstaclesList[3] = FieldObstacles::Obstacle::EAST;
    obstaclesList[4] = FieldObstacles::Obstacle::SOUTHEAST;
    obstaclesList[5] = FieldObstacles::Obstacle::SOUTH;
    obstaclesList[6] = FieldObstacles::Obstacle::SOUTHWEST;
    obstaclesList[7] = FieldObstacles::Obstacle::WEST;
    obstaclesList[8] = FieldObstacles::Obstacle::NORTHWEST;

    lastSonar = 0.f;

    usingArms = arm;
    usingLeftSonar = true;
    usingRightSonar = true;
    usingVision = vision;
}

void ObstacleModule::setSonars(bool sonarL, bool sonarR){
    usingLeftSonar = sonarL;
    usingRightSonar = sonarR;
}

void ObstacleModule::run_()
{
    std::cout<<"obst"<<usingArms<<usingLeftSonar<<usingRightSonar<<usingVision<<std::endl;
    visionIn.latch();
    armContactIn.latch();
    sonarIn.latch();

    // Don't need this kind of buffer when we aren't using vision
    if (!usingVision)
    {
        for (int i = 0; i < NUM_DIRECTIONS; i++) {
            obstacleBuffer[i] = 0;
        }
    }

    FieldObstacles::Obstacle::ObstaclePosition sonars, arms;

    // Process vision in all three sections of frame separately
    if (usingVision) {
        processVision(visionIn.message().left_dist(),
                      visionIn.message().left_bearing());
        processVision(visionIn.message().mid_dist(),
                      visionIn.message().mid_bearing());
        processVision(visionIn.message().right_dist(),
                      visionIn.message().right_bearing());
    }

    // Decide arms
    if (usingArms) { arms = processArms( armContactIn.message()); }
    else { arms = FieldObstacles::Obstacle::NONE; }
    // if not combining arms and sonars
    // updateObstacleArrays(FieldObstacles::Obstacle::ARMS, arms, -1.f);

    // Decide sonars
    if (usingLeftSonar || usingRightSonar) { sonars = processSonar(sonarIn.message()); }
    else { sonars = FieldObstacles::Obstacle::NONE; }
    // if not combining arms and sonars:
    // updateObstacleArrays(FieldObstacles::Obstacle::SONARS, sonars, lastSonar);

#ifdef USE_LAB_FIELD // Walls are too close to field for sonar use
    sonars = FieldObstacles::Obstacle::NONE;
#endif

    // combines input from both arms and sonars
    // Can also choose to do this separately
    combineArmsAndSonars(arms, sonars);

    // Now we take information and return relevant obstacles
    portals::Message<messages::FieldObstacles> current(0);

    // ignore "NONE" direction, start at 1
    for (int i = 1; i < NUM_DIRECTIONS; i++)
    {
        if (obstacleBuffer[i]==0) { continue; } //no obstacle here

        FieldObstacles::Obstacle* temp = current.get()->add_obstacle();
        temp->set_position(obstaclesList[i]);
        temp->set_distance(obstacleDistances[i]);
        temp->set_detector(obstacleDetectors[i]);
    }

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
    // If front thresh is larger than regular upper thresh
    float right = SONAR_FRONT_THRESH_UPPER;
    float left = SONAR_FRONT_THRESH_UPPER;

    // Buffer the current sonar values
    rightSonars.push_back(input.us_right());
    leftSonars.push_back(input.us_left());

    // Get rid of old values
    if (usingRightSonar && rightSonars.size() > SONAR_FRAMES_TO_BUFFER)
    {
        rightSonars.pop_front();
    }

    if (usingLeftSonar && leftSonars.size() > SONAR_FRAMES_TO_BUFFER)
    {
        leftSonars.pop_front();
    }

    // Use the current average for our decision
    if (usingRightSonar) { right = average(rightSonars); }
    if (usingLeftSonar) { left = average(leftSonars); }
    // std::cout<<"RIGHT = "<<right<<" , LEFT = "<<left<<std::endl;

    // Both sonars picking up an obstacle? It's probably in front
    if (right < SONAR_FRONT_THRESH_UPPER && left < SONAR_FRONT_THRESH_UPPER &&
        right > SONAR_THRESH_LOWER && left > SONAR_THRESH_LOWER)
    {
        lastSonar = .5f * (right + left);
        return FieldObstacles::Obstacle::NORTH;
    }
    // Otherwise to right side...
    else if (right < SONAR_THRESH_UPPER && right > SONAR_THRESH_LOWER)
    {
        lastSonar = right;
        return FieldObstacles::Obstacle::NORTHEAST;
    }
    // ... left side ...
    else if (left < SONAR_THRESH_UPPER && left > SONAR_THRESH_LOWER)
    {
        lastSonar = left;
        return FieldObstacles::Obstacle::NORTHWEST;
    }
    // .. or no obstacle
    else
    {
        lastSonar = 0.f;
        return FieldObstacles::Obstacle::NONE;
    }
}


void ObstacleModule::combineArmsAndSonars
(FieldObstacles::Obstacle::ObstaclePosition arms,
 FieldObstacles::Obstacle::ObstaclePosition sonars)
{
    // How do we combine the arm and sonar decisions?
    // If they agree, easy
    if (arms == sonars)
    {
        updateObstacleArrays(FieldObstacles::Obstacle::ARMS, arms, -1.f);
    }
    // Trust sonars before we get any arm input
    else if (arms == FieldObstacles::Obstacle::NONE)
    {
        updateObstacleArrays(FieldObstacles::Obstacle::SONARS, sonars, lastSonar);
    }
    // If they sort of agree, use the value that gives us better dodging info
    else if (sonars == FieldObstacles::Obstacle::NORTH &&
             (arms == FieldObstacles::Obstacle::NORTHWEST ||
              arms == FieldObstacles::Obstacle::NORTHEAST))
    {
        updateObstacleArrays(FieldObstacles::Obstacle::ARMS, arms, -1.f);
    }
    else if (arms == FieldObstacles::Obstacle::NORTH &&
             (sonars == FieldObstacles::Obstacle::NORTHWEST ||
              sonars == FieldObstacles::Obstacle::NORTHEAST))
    {
        updateObstacleArrays(FieldObstacles::Obstacle::SONARS, sonars, lastSonar);
    }
    // If they don't agree or get no sonars, trust the arms
    else
    {
        updateObstacleArrays(FieldObstacles::Obstacle::ARMS, arms, -1.f);
    }

}

void ObstacleModule::updateVisionBuffer
(FieldObstacles::Obstacle::ObstaclePosition pos,
 std::list<float> dists, float distance)
{
    dists.push_back(distance);
    if (dists.size() > VISION_FRAMES_TO_BUFFER)
    {
        dists.pop_front();
    }

    float avg = average(dists);
    updateObstacleArrays(FieldObstacles::Obstacle::VISION, pos, avg);
}

void ObstacleModule::processVision(float distance, float bearing)
{
    // Process what direction it is in: act appropriately
    if ( bearing < -5.f*ZONE_WIDTH)
    {
        // obstacle to the southeast
        updateVisionBuffer(FieldObstacles::Obstacle::SOUTHEAST, SEDists, distance);
    }
    else if ( bearing < -3.f*ZONE_WIDTH )
    {
        // obstacle to the east
        updateVisionBuffer(FieldObstacles::Obstacle::EAST, EDists, distance);
    }
    else if ( bearing < -ZONE_WIDTH )
    {
        // obstacle to northeast
        updateVisionBuffer(FieldObstacles::Obstacle::NORTHEAST, NEDists, distance);
    }
    else if ( bearing < ZONE_WIDTH )
    {
        // obstacle to north
        updateVisionBuffer(FieldObstacles::Obstacle::NORTH, NDists, distance);
    }
    else if ( bearing < 3.f*ZONE_WIDTH )
    {
        // obstacle to northwest
        updateVisionBuffer(FieldObstacles::Obstacle::NORTHWEST, NWDists, distance);
    }
    else if ( bearing < 5.f*ZONE_WIDTH )
    {
        // obstacle to west
        updateVisionBuffer(FieldObstacles::Obstacle::WEST, WDists, distance);
    }
    else if ( bearing < 7.f * ZONE_WIDTH )
    {
        // obstacle to southwest
        updateVisionBuffer(FieldObstacles::Obstacle::SOUTHWEST, SWDists, distance);
    }
    // else // south
    // {
    //     return FieldObstacles::Obstacle::NONE;
    // }
}

void ObstacleModule::updateObstacleArrays
(FieldObstacles::Obstacle::ObstacleDetector detector,
 FieldObstacles::Obstacle::ObstaclePosition pos, float dist)
{
    // if (detector==FieldObstacles::Obstacle::SONARS)
    //     std::cout<<"SONARS "<<dist<<std::endl;
    // start the buffer count
    obstacleBuffer[int(pos)] = 1;
    obstacleDistances[int(pos)] = dist;
    obstacleDetectors[int(pos)] = detector;
}

void ObstacleModule::updateObstacleBuffer()
{
    for (int i = 1; i < NUM_DIRECTIONS; i++)
    {
        if (obstacleBuffer[i] == 0 or
            obstacleBuffer[i] > VISION_FRAMES_TO_HAVE_OBSTACLE)
        {
            obstacleBuffer[i] = 0;
            obstacleDistances[i] = 0.f;
            continue;
        }
        obstacleBuffer[i]++;
    }
}

} // namespace obstacle
} // namespace man
