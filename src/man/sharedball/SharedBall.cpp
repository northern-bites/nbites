#include "SharedBall.h"

namespace man {
namespace context {

SharedBallModule::SharedBallModule() :
    portals::Module(),
    sharedBallOutput(base()),
    sharedBallReset(base())
{
    x = 0;
    y = 0;
    ballOn = false;
    resetx = 0.f;
    resety = 0.f;
    reseth = 0.f;
    timestamp = 0;
    my_num = int(MY_PLAYER_NUMBER);
}

SharedBallModule::~SharedBallModule()
{
}

void SharedBallModule::run_()
{
    //reset variables
    numRobotsOn = 0;
    robotToIgnore = -1;

    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; i++) {
        worldModelIn[i].latch();
        messages[i] = worldModelIn[i].message();
        if (messages[i].ball_on()) {
            numRobotsOn++;
        }
        // resets ball estimates for each robot
        ballX[i] = -1.f;
        ballY[i] = -1.f;
    }
    if (!(numRobotsOn)) {
        x = -100;
        y = -100;
        ballOn = false;
    } else if (numRobotsOn < 3) {
        // makes a normal weighted average of ball from robots that see the ball
        weightedavg();
    } else {
        eliminateBadRobots();
        weightedavg();
        checkForPlayerFlip();
    }

    portals::Message<messages::SharedBall> sharedBallMessage(0);
    portals::Message<messages::RobotLocation> sharedBallResetMessage(0);

    // sets the regular shared ball message
    sharedBallMessage.get()->set_x(x);
    sharedBallMessage.get()->set_y(y);
    sharedBallMessage.get()->set_ball_on(ballOn);
    sharedBallMessage.get()->set_num_robots(numRobotsOn);
    sharedBallOutput.setMessage(sharedBallMessage);

    // sets the message to reset the flipped robot to correct location
// TOOL: Uncomment the set_uncert line. Used to tell which player to flip.
//    float flippedNum = float(robotToIgnore) + 1.f;
    sharedBallResetMessage.get()->set_x(resetx);
    sharedBallResetMessage.get()->set_y(resety);
    sharedBallResetMessage.get()->set_h(reseth);
    sharedBallResetMessage.get()->set_timestamp(timestamp);
//    sharedBallResetMessage.get()->set_uncert(flippedNum);
    sharedBallReset.setMessage(sharedBallResetMessage);
}

void SharedBallModule::eliminateBadRobots()
{
    // get robot that has largest
    int maxes[NUM_PLAYERS_PER_TEAM] = { 0 }; //all elements are 0
    int bad_robot_max_num = 0;
    int bad_robot = -1;
    float bigDist = 0.f;
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; i++) {
        if (!messages[i].ball_on()) {
            continue;
        }
        int j_max = -1;
        float max_distance = -1.f;
        for (int j = 0; j < NUM_PLAYERS_PER_TEAM; j++) {
            if (!messages[j].ball_on() || i == j) {
                continue;
            }
            float curr_distance = getBallDistanceSquared(i, j);
            if (curr_distance > max_distance) {
                max_distance = curr_distance;
                j_max = j;
            }
        }
        if (max_distance > bigDist) {
            bigDist = max_distance;
        }
        if (j_max != -1) {
            maxes[j_max]++;
            if (maxes[j_max] > bad_robot_max_num) {
                bad_robot_max_num = maxes[j_max];
                bad_robot = j_max;
            }
        } else {
            std::cout<<"error"<<std::endl;
        }
    }

    // now get rid of that robot if every player thinks its the bad robot
    if ((bad_robot_max_num == numRobotsOn - 1) &&
        (bigDist > DISTANCE_TOO_FAR*DISTANCE_TOO_FAR)) {
            robotToIgnore = bad_robot;
            numRobotsOn--;
    }
}

/* Calculates a weighted average of the robot's ball locations, where
 * weight is determined by distance to the ball.
 * The goalie's estimate is weighted twice as much.
 */
void SharedBallModule::weightedavg()
{
    float numx = 0;       // numerator of weighted average of x
    float numy = 0;       // numerator of weighted average of y
    float sumweight = 0;  // denominator of weighted average = sum of weights
    float weight = 0;     // determined by distance to ball
    float tempx, tempy;
    float dist, hb, sinHB, cosHB;
    int weightFactor;

    for (int i=0; i<NUM_PLAYERS_PER_TEAM; i++) {
        if (i == robotToIgnore or !messages[i].ball_on()) {
            continue;
        }
        // avoids recalculating ball coord. if they have already been calculated
        calculateBallCoords(i);
        tempx = ballX[i];
        tempy = ballY[i];

        if (i == 0) {
            weightFactor = 2;
        } else {
            weightFactor = 1;
        }

        weight = weightFactor/(messages[i].ball_dist());

        numx += tempx * weight;
        numy += tempy * weight;
        sumweight += weight;
    }

    // at least one robot sees the ball
    if (sumweight != 0) {
        x = numx / sumweight;
        y = numy / sumweight;
        ballOn = true;
    } else {
        std::cout<<"ERROR: sumweight is 0"<<std::endl;
    }
}

/* If the robot which was named to have "bad ball information" has
 * a ball estimate which would be correct if the robot were flipped,
 * then flip this robot.
 */
void SharedBallModule::checkForPlayerFlip()
{
    if (robotToIgnore != my_num) {
//    TOOL: Comment out above line and uncomment below line
//    if (robotToIgnore == -1){
        return;
    }
    if (messages[robotToIgnore].my_x() > MIDFIELD_X - TOO_CLOSE_TO_MIDFIELD and
        messages[robotToIgnore].my_x() < MIDFIELD_X + TOO_CLOSE_TO_MIDFIELD) {
        return;
    }

    calculateBallCoords(robotToIgnore);
    int rob = robotToIgnore;

    float flipbx = (-1*(ballX[rob] - MIDFIELD_X)) + MIDFIELD_X;
    float flipby = (-1*(ballY[rob] - MIDFIELD_Y)) + MIDFIELD_Y;

    float flipX =  (-1*(messages[rob].my_x() - MIDFIELD_X)) + MIDFIELD_X;
    float flipY = (-1*(messages[rob].my_y() - MIDFIELD_Y)) + MIDFIELD_Y;

    float distance_sq = ( (x-flipbx)*(x-flipbx) + (y-flipby)*(y-flipby) );
    if (distance_sq < DISTANCE_FOR_FLIP*DISTANCE_FOR_FLIP) {
        resetx = flipX;
        resety = flipY;
        reseth = messages[rob].my_h() + 180;
        if (reseth > 180){
            reseth -= 360;
        }
        timestamp = messages[rob].timestamp();
    }
}

/* Calculates ball coordinates and puts them in global array if they
 * have not been calculated yet. The idea is to not repeatedly
 * calculate the same thing.
 */
void SharedBallModule::calculateBallCoords(int i) {
    if (ballX[i] == -1 || ballY[i] == -1) {
        float sinHB, cosHB;
        float hb = TO_RAD*messages[i].my_h() + TO_RAD*messages[i].ball_bearing();
        sincosf(hb, &sinHB, &cosHB);

        float newx = messages[i].my_x() + messages[i].ball_dist()*cosHB;
        float newy = messages[i].my_y() + messages[i].ball_dist()*sinHB;

        ballX[i] = newx;
        ballY[i] = newy;
    }
}

/*
 * Calculates distance between two players' balls.
 */
float SharedBallModule::getBallDistanceSquared(int i, int j)
{
    float x1, x2, y1, y2;
    float hb, sinHB, cosHB;

    calculateBallCoords(i);
    x1 = ballX[i];
    y1 = ballY[i];

    calculateBallCoords(j);
    x2 = ballX[j];
    y2 = ballY[j];

    return ( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) );
}

// As of now we are not calling this method but if we decide the goalie should
// assume a fixed postition, call this in weightedAvg if i = 0
/*
void SharedBallModule::incorporateGoalieWorldModel(messages::WorldModel newModel)
{
    if(newModel.ball_on()) {
        float dist = newModel.ball_dist();
        weight = 1/(dist);

        float hb = TO_RAD*HEADING_RIGHT + TO_RAD*newModel.ball_bearing();
        float sinHB, cosHB;
        sincosf(hb, &sinHB, &cosHB);

        x = FIELD_WHITE_LEFT_SIDELINE_X + newModel.ball_dist()*cosHB;
        y = CENTER_FIELD_Y + newModel.ball_dist()*sinHB;

    }
    else {
        x = 0;
        y = 0;
        weight = 0;
    }
}
*/

} // namespace man
} // namespace context
