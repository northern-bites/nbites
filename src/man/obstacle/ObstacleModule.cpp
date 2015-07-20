#include "ObstacleModule.h"

namespace man {
namespace obstacle {

using messages::FieldObstacles;
using messages::ArmContactState;
using nblog::SExpr;

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

ObstacleModule::ObstacleModule(std::string filepath, std::string robotName)
: obstacleOut(base()),
lastSonar(0.f),
usingArms(false),
usingLeftSonar(false),
usingRightSonar(false),
usingVision(false)
{
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

    if(FILE *file = fopen(filepath.c_str(),"r")) {
        fclose(file);

        std::ifstream inputFile(filepath);
        std::string readInFile((std::istreambuf_iterator<char>(inputFile)),
                                std::istreambuf_iterator<char>());

        int i=0;
        SExpr params = *SExpr::read(readInFile,i);
        params.print();

        if(params.count() >= 2) {
            std::cout<<"[OBSTACLE ] Reading from SExpr"<<std::endl;
            std::cout<<"[OBSTACLE ] PATH: "<<filepath<<std::endl;

            bool all_sonars = false;
            SExpr *tempArms, *tempVision, *tempAll;
            tempArms = params.find("arms")->get(1);
            tempVision = params.find("vision")->get(1);
            tempAll = params.find("set_all_sonar")->get(1);

            if (tempArms) { usingArms = tempArms->valueAsInt()==1 ? true : false; }
            if (tempVision) { usingVision = tempVision->valueAsInt()==1 ? true : false; }
            if (tempAll) { all_sonars = tempAll->valueAsInt()==1 ? true : false; }

            if (all_sonars) {
                std::cout<<"[OBSTACLE ] Setting all robots the same"<<std::endl;

                SExpr *leftTemp = NULL, *rightTemp = NULL;
                leftTemp = params.find("all_left_sonar")->get(1);
                rightTemp = params.find("all_right_sonar")->get(1);

                if (leftTemp) { usingLeftSonar = leftTemp->valueAsInt() == 1 ? true : false; }
                if (rightTemp) { usingRightSonar = rightTemp->valueAsInt() == 1 ? true : false; }
            } else {
                SExpr *leftTemp = NULL, *rightTemp = NULL;
                SExpr *leftTemp2 = NULL, *rightTemp2 = NULL;
                SExpr* temp1 = params.find(robotName);
                if (temp1) {
                    leftTemp = temp1->get(1);
                    rightTemp = temp1->get(2);
                }
                if (leftTemp) { leftTemp2 = leftTemp->get(1); }
                if (rightTemp) { rightTemp2 = rightTemp->get(1); }

                if (leftTemp2) {
                    usingLeftSonar = leftTemp2->valueAsInt() == 1 ? true : false;
                }
                if (rightTemp2) {
                    usingRightSonar = rightTemp2->valueAsInt() == 1 ? true : false;
                }
            }
        } else {
            std::cout<<"[ERR ] Invalid SExpr"<<std::endl;
            std::cout<<"[ERR ] Check /nbites/Config/ for them"<<std::endl;
        }
    } else {
        std::cout<<"[ERR ] Config files not found."<<std::endl;
    }
    std::cout<<"[OBSTACLE ] VARS:";
    if (usingVision) { std::cout<<" (VISION)"; }
    if (usingArms) { std::cout<<" (ARMS)"; }
    if (usingLeftSonar) { std::cout<<" (LEFT SONAR)"; }
    if (usingRightSonar) { std::cout<<" (RIGHT SONAR)"; }
    std::cout<<std::endl;
}

void ObstacleModule::run_()
{
    armContactIn.latch();
    sonarIn.latch();
    visionIn.latch();

    // init arrays
    for (int i = 0; i < NUM_DIRECTIONS; i++) {
        obstacleDistances[i] = 0;
        obstacleDetectors[i] = FieldObstacles::Obstacle::NA;
    }

    FieldObstacles::Obstacle::ObstaclePosition sonars, arms;

    // Decide arms
    if (usingArms) { arms = processArms( armContactIn.message()); }
    else { arms = FieldObstacles::Obstacle::NONE; }
    // if not combining arms and sonars, uncomment this:
    // updateObstacleArrays(FieldObstacles::Obstacle::ARMS, arms, -1.f);

    // Decide sonars
    if (usingLeftSonar || usingRightSonar) { sonars = processSonar(sonarIn.message()); }
    else { sonars = FieldObstacles::Obstacle::NONE; }
    // if not combining arms and sonars, uncomment this:
    // updateObstacleArrays(FieldObstacles::Obstacle::SONARS, sonars, lastSonar);

#ifdef USE_LAB_FIELD // Walls are too close to field for sonar use
    sonars = FieldObstacles::Obstacle::NONE;
#endif

    // combines input from both arms and sonars
    // Comment this out if we choose to do this separately
    combineArmsAndSonars(arms, sonars);

    // Process vision if we want to use it
    if (usingVision) { processVision(visionIn.message()); }

    // Now we take information and return relevant obstacles
    portals::Message<messages::FieldObstacles> current(0);

    // ignore "NONE" direction, start at 1
    for (int i = 1; i < NUM_DIRECTIONS; i++)
    {
        if (obstacleDistances[i]==0) { continue; } //no obstacle here

        FieldObstacles::Obstacle* temp = current.get()->add_obstacle();
        temp->set_position(obstaclesList[i]);
        temp->set_distance(obstacleDistances[i]);
        temp->set_detector(obstacleDetectors[i]);

        // update vision box
        if (obstacleBox[0] == i && obstacleDetectors[i] ==
            FieldObstacles::Obstacle::VISION) {
            temp->set_closest_y(obstacleBox[1]);
            temp->set_box_bottom(obstacleBox[2]);
            temp->set_box_left(obstacleBox[3]);
            temp->set_box_right(obstacleBox[4]);
        }
    }

    obstacleOut.setMessage(current);
}

FieldObstacles::Obstacle::ObstaclePosition
ObstacleModule::processArms(const messages::ArmContactState& input)
{
    // Both arms pushed approximately backwards...something is in front
    if ((input.right_push_direction() == ArmContactState::SOUTH ||
         input.right_push_direction() == ArmContactState::SOUTHEAST ||
         input.right_push_direction() == ArmContactState::SOUTHWEST) &&
        (input.left_push_direction() == ArmContactState::SOUTH ||
         input.left_push_direction() == ArmContactState::SOUTHWEST ||
         input.left_push_direction() == ArmContactState::SOUTHEAST))
    {
        return FieldObstacles::Obstacle::NORTH;
    }
    // Both arms pushed approximately forward... something is behind
    else if ((input.right_push_direction() == ArmContactState::NORTH ||
              input.right_push_direction() == ArmContactState::NORTHEAST ||
              input.right_push_direction() == ArmContactState::NORTHWEST) &&
             (input.left_push_direction() == ArmContactState::NORTH ||
              input.left_push_direction() == ArmContactState::NORTHWEST ||
              input.left_push_direction() == ArmContactState::NORTHEAST))
    {
        return FieldObstacles::Obstacle::SOUTH;
    }
    // Not getting pushed on right arm... decide based on left
    else if (input.right_push_direction() == ArmContactState::NONE)
    {
        if (input.left_push_direction() == ArmContactState::NORTH ||
                 input.left_push_direction() == ArmContactState::NORTHEAST)
        {
            return FieldObstacles::Obstacle::SOUTHWEST;
        }
        else if (input.left_push_direction() == ArmContactState::EAST)
        {
            return FieldObstacles::Obstacle::WEST;
        }
        else if (input.left_push_direction() == ArmContactState::SOUTH ||
                 input.left_push_direction() == ArmContactState::SOUTHEAST)
        {
            return FieldObstacles::Obstacle::NORTHWEST;
        }
        else if (input.left_push_direction() == ArmContactState::SOUTHWEST)
        {
            return FieldObstacles::Obstacle::NORTH;
        }
        else if (input.left_push_direction() == ArmContactState::WEST ||
                 input.left_push_direction() == ArmContactState::NORTHWEST)
        {
            return FieldObstacles::Obstacle::SOUTH;
        }
        else return FieldObstacles::Obstacle::NONE;
    }
    // Not getting pushed on left arm... decide based on right
    else if (input.left_push_direction() == ArmContactState::NONE)
    {
        if (input.right_push_direction() == ArmContactState::NORTH ||
            input.right_push_direction() == ArmContactState::NORTHWEST)
        {
            return FieldObstacles::Obstacle::SOUTHEAST;
        }
        else if (input.right_push_direction() ==
             ArmContactState::WEST)
        {
            return FieldObstacles::Obstacle::EAST;
        }
        else if (input.right_push_direction() == ArmContactState::SOUTH ||
                 input.right_push_direction() == ArmContactState::SOUTHWEST)
        {
            return FieldObstacles::Obstacle::NORTHEAST;
        }
        else if (input.right_push_direction() == ArmContactState::SOUTHEAST)
        {
            return FieldObstacles::Obstacle::NORTH;
        }
        else if (input.right_push_direction() == ArmContactState::EAST ||
                 input.right_push_direction() == ArmContactState::NORTHEAST)
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
 const messages::RobotObstacle& input)
{
    // update with distance in meters instead of cm
    updateObstacleArrays(FieldObstacles::Obstacle::VISION, pos, .01f*input.box_bottom());

    obstacleBox[0] = (float)pos;
    obstacleBox[1] = input.closest_y();
    obstacleBox[2] = input.box_bottom();
    obstacleBox[3] = input.box_left();
    obstacleBox[4] = input.box_right();

    // printf("Obstacle Box OBST2: (%g, %g, %g, %g)\n",
    //         obstacleBox[1], obstacleBox[2], obstacleBox[3], obstacleBox[4]);
}

void ObstacleModule::processVision(const messages::RobotObstacle& input)
{
    // printf("Obstacle Box OBST: (%g, %g, %g, %g)\n",
    //         input.closest_y(), input.box_bottom(),
    //         input.box_left(), input.box_right());

    // reset obstacle box
    for (int i = 0; i < 5; i++) {
        obstacleBox[i] = -1;
    }

    // check for no obstacle in vision
    if (input.closest_y() == -1) { return; }

    // Don't want to dodge obstacles too far away
    if (input.box_bottom() > VISION_MAX_DIST) { return; }

    float bearing = (float)atan2(input.box_bottom(),
                    ((input.box_left() - input.box_right()) / 2.f));

    // Robot facing bearing of pi/2, east is 0, west is pi
    // Process what direction obstacle is in: act appropriately
    if ( bearing < ZONE_WIDTH )
    {
        // obstacle to the east
        updateVisionBuffer(FieldObstacles::Obstacle::EAST, input);
        // std::cout<<"[OBSTACLE ] EAST"<<std::endl;
    }
    else if ( bearing < 3.f * ZONE_WIDTH )
    {
        // obstacle to northeast
        updateVisionBuffer(FieldObstacles::Obstacle::NORTHEAST, input);
        // std::cout<<"[OBSTACLE ] NORTHEAST"<<std::endl;
    }
    else if ( bearing < 5.f * ZONE_WIDTH )
    {
        // obstacle to north
        updateVisionBuffer(FieldObstacles::Obstacle::NORTH, input);
        // std::cout<<"[OBSTACLE ] NORTH"<<std::endl;
    }
    else if ( bearing < 7.f * ZONE_WIDTH )
    {
        // obstacle to northwest
        updateVisionBuffer(FieldObstacles::Obstacle::NORTHWEST, input);
        // std::cout<<"[OBSTACLE ] NORTHWEST"<<std::endl;
    }
    else if ( bearing < 9.f * ZONE_WIDTH )
    {
        // obstacle to west
        updateVisionBuffer(FieldObstacles::Obstacle::WEST, input);
        // std::cout<<"[OBSTACLE ] WEST"<<std::endl;
    }
}

void ObstacleModule::updateObstacleArrays
(FieldObstacles::Obstacle::ObstacleDetector detector,
 FieldObstacles::Obstacle::ObstaclePosition pos, float dist)
{
    if (int(pos) == 0) { return; }
    obstacleDistances[int(pos)] = dist;
    obstacleDetectors[int(pos)] = detector;
}

} // namespace obstacle
} // namespace man
