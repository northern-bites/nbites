#include "SharedBall.h"

namespace man {
namespace context {

SharedBallModule::SharedBallModule(int playerNumber) :
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
    flippedRobot = 0.f;
    my_num = playerNumber;
}

int inNoFlipZone(float xx, float yy)
{
    if ( (xx > MIDFIELD_X - TOO_CLOSE_TO_MIDFIELD_X &&
          xx < MIDFIELD_X + TOO_CLOSE_TO_MIDFIELD_X) &&
         (yy > MIDFIELD_Y - TOO_CLOSE_TO_MIDFIELD_Y &&
          yy < MIDFIELD_Y + TOO_CLOSE_TO_MIDFIELD_Y) )
    {
        return 1;
    }
    return 0;
}

SharedBallModule::~SharedBallModule()
{
}

void SharedBallModule::run_()
{
    //reset variables
    numRobotsOn = 0;
    reliability = 0;

    // this is in case we have 0 robots on: when these var. wont' get set
    ballOn = false;

    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; i++)
    {
        worldModelIn[i].latch();
        worldMessages[i] = worldModelIn[i].message();
        if (worldMessages[i].ball_on())
        {
            numRobotsOn++;
        }
        // resets ball estimates for each robot
        ballX[i] = -1.f;
        ballY[i] = -1.f;
    }

    // get myInfo:
    locIn.latch();
    myX = locIn.message().x();
    myY = locIn.message().y();
    myH = locIn.message().h();
    ballIn.latch();
    myBall = ballIn.message();

    if (numRobotsOn)
    {
        ballOn = true;
        chooseRobots();
        weightedavg();
        checkForPlayerFlip();
    }

    portals::Message<messages::SharedBall> sharedBallMessage(0);
    portals::Message<messages::RobotLocation> sharedBallResetMessage(0);

    // sets the regular shared ball message
    sharedBallMessage.get()->set_x(x);
    sharedBallMessage.get()->set_y(y);
    sharedBallMessage.get()->set_ball_on(ballOn);
    sharedBallMessage.get()->set_reliability(reliability);
    sharedBallOutput.setMessage(sharedBallMessage);

    // sets the message to reset the flipped robot to correct location
    // TOOL: Uncomment the set_uncert line. Used to tell which player to flip.
    sharedBallResetMessage.get()->set_x(resetx);
    sharedBallResetMessage.get()->set_y(resety);
    sharedBallResetMessage.get()->set_h(reseth);
    sharedBallResetMessage.get()->set_timestamp(timestamp);
//    sharedBallResetMessage.get()->set_uncert(flippedRobot);
    sharedBallReset.setMessage(sharedBallResetMessage);
}

/* Makes groups for each robot that include those other robots who
 * have a ball estimate within the given threshold from its own
 * ball estimate. Then it sets a global array based on groupings that
 * tell other methods which robots to ignore.
 */
void SharedBallModule::chooseRobots()
{
    int inEstimate[NUM_PLAYERS_PER_TEAM][NUM_PLAYERS_PER_TEAM];
    int numInEstimate[NUM_PLAYERS_PER_TEAM];
    int maxInEstimate = 0;
    int goodPlayer = -1;
    int numWithMaxEstimate = 1;  //num robots with max num of consensuses

    // Is the goalie in its box? If no, don't include it in estimate!
    bool includeGoalie = inGoalieBox(worldMessages[0].my_x(), worldMessages[0].my_y());

    // go through and see who is in each robot's ball estimate
    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; i++)
    {
        numInEstimate[i] = 0;

        for (int j = 0; j < NUM_PLAYERS_PER_TEAM; j++)
        {
            if (!worldMessages[i].ball_on() || !worldMessages[j].ball_on()
                || !worldMessages[i].active() || !worldMessages[j].active())
            {
                inEstimate[i][j] = 0;
                continue;
            }
            // If I don't want goalie, don't use him!
            if ( !includeGoalie && (i == 0 or j == 0) )
            {
                inEstimate[i][j] = 0;
                continue;
            }

            float dist = getBallDistanceSquared(i, j);
            inEstimate[i][j] = 0;
            if (dist < CONSENSUS_THRESHOLD * CONSENSUS_THRESHOLD
                or i == j)
            {
                inEstimate[i][j] = 1;
                numInEstimate[i]++;
                if (numInEstimate[i] > maxInEstimate)
                {
                    maxInEstimate = numInEstimate[i];
                    numWithMaxEstimate = 1;
                    goodPlayer = i;
                }
                else if (numInEstimate[i] == maxInEstimate)
                {
                    numWithMaxEstimate++;
                    if (goodPlayer == -1) {
                        // mostly for case when there are a bunch of single robots
                        goodPlayer = i;
                    }
                }
            }
        }
    }

    // now decide whether or not to use sharedball
    if (numWithMaxEstimate > maxInEstimate)
    {
        bool goalieOn = false;
        for (int i = 0; i < NUM_PLAYERS_PER_TEAM; i++)
        {
            // if the goalie is in my estimate, and the goalie's grouping is max
            if (inEstimate[i][0] && numInEstimate[0] == maxInEstimate)
            {
                goalieOn = true;
                goodPlayer = i;
                break;
            }
        }
        if (!goalieOn)
        {
            // don't want to use shared ball: not large enough consensus, no goalie
            ballOn = false;
            return;
        }
        // else the goalie will be used to break the tie!
    }

    for (int i = 0; i < NUM_PLAYERS_PER_TEAM; i++)
    {
        if (inEstimate[goodPlayer][i])
        {
            ignoreRobot[i] = 0;
        }
        else
        {
            ignoreRobot[i] = 1;
        }
    }
}

/* Calculates a weighted average of the robot's ball locations, where
 * weight is determined by distance to the ball.
 * The goalie's estimate is weighted twice as much.
 */
void SharedBallModule::weightedavg()
{
    if (!ballOn)
    {
        return;
    }

    float numx = 0;       // numerator of weighted average of x
    float numy = 0;       // numerator of weighted average of y
    float sumweight = 0;  // denominator of weighted average = sum of weights
    float weight = 0;     // determined by distance to ball
    float tempx, tempy;
    float dist, hb, sinHB, cosHB;
    int weightFactor;


    for (int i=0; i<NUM_PLAYERS_PER_TEAM; i++)
    {
        if (ignoreRobot[i])
        {
            continue;
        }

        if ( i == 0 )
        {
            // goalie is on: counts double for reliability
            reliability++;
        }

        // avoids recalculating ball coord. if they have already been calculated
        calculateBallCoords(i);
        tempx = ballX[i];
        tempy = ballY[i];

        weight = 1/(worldMessages[i].ball_dist());

        numx += tempx * weight;
        numy += tempy * weight;
        sumweight += weight;
        reliability++;
    }

    // at least one robot sees the ball
    if (sumweight != 0) {
        x = numx / sumweight;
        y = numy / sumweight;
        ballOn = true;
    } else {
        std::cout<<"Error! SumWeight is 0!"<<std::endl;
    }
}


/* If the robot which was named to have "bad ball information" has
 * a ball estimate which would be correct if the robot were flipped,
 * then flip this robot.
 */
void SharedBallModule::checkForPlayerFlip()
{
    if (!ballOn or reliability < 2)
    {
        // std::cout<<"Flip return"<<std::endl;
        return;
    }

    int i = my_num-1;

    if (!worldMessages[i].ball_on() or !ignoreRobot[i] or i == 0)
    {
        // If I was a good robot, or I'm the goalie, don't check for flip!
        return;
    }

    calculateBallCoords(i);
    // if my ball or sharedball is in no-flip zone-> don't flip!
    if (inNoFlipZone(ballX[i], ballY[i]) || inNoFlipZone(x, y)) {
        return;
    }

    float flipbx = (-1*(ballX[i] - MIDFIELD_X)) + MIDFIELD_X;
    float flipby = (-1*(ballY[i] - MIDFIELD_Y)) + MIDFIELD_Y;

    float flipX =  (-1*(myX - MIDFIELD_X)) + MIDFIELD_X;
    float flipY = (-1*(myY - MIDFIELD_Y)) + MIDFIELD_Y;

    calculateBallCoords(0);
    float gx = ballX[0];
    float gy = ballY[0];

    // if the goalie is reliable: on and in goal box, he must agree
    if ( worldMessages[0].ball_on() && worldMessages[0].active() &&
         inGoalieBox(worldMessages[0].my_x(), worldMessages[0].my_y()) )
    {
        float goalie_sq = ( (gx-flipbx)*(gx-flipbx) + (gy-flipby)*(gy-flipby) );
        if (goalie_sq > DISTANCE_FOR_FLIP*DISTANCE_FOR_FLIP) {
            return;
        }
    }
    float distance_sq = ( (x-flipbx)*(x-flipbx) +
                          (y-flipby)*(y-flipby) );
    if (distance_sq < DISTANCE_FOR_FLIP*DISTANCE_FOR_FLIP) {
        resetx = flipX;
        resety = flipY;
        reseth = myH + 180;
        if (reseth > 180){
            reseth -= 360;
        }
        timestamp = int(worldMessages[i].timestamp());
        flippedRobot = float(i + 1);
        std::cout<<"FLIPPED! I am "<<my_num<<" and I flipped with reliability "
                     <<reliability<<"!"<<std::endl;
        std::cout<<"I flipped from "<<myX<<", "<<myY<<std::endl;
        std::cout<<"I flipped to "<<flipX<<", "<<flipY<<std::endl;
        std::cout<<"...Because shared ball is at "<<x<<", "<<y<<std::endl;
        std::cout<<"And my ball was at "<<ballX[i]<<", "<<ballY[i]<<std::endl;
    }
}

/* Calculates ball coordinates and puts them in global array if they
 * have not been calculated yet. The idea is to not repeatedly
 * calculate the same thing.
 */
void SharedBallModule::calculateBallCoords(int i)
{
    if (ballX[i] != -1 && ballY[i] != -1) {
        return;
    }

    float sinHB, cosHB;
    if (my_num-1 != i) {
        float hb = TO_RAD*worldMessages[i].my_h() + TO_RAD*worldMessages[i].ball_bearing();
        sincosf(hb, &sinHB, &cosHB);

        float newx = worldMessages[i].my_x() + worldMessages[i].ball_dist()*cosHB;
        float newy = worldMessages[i].my_y() + worldMessages[i].ball_dist()*sinHB;

        ballX[i] = newx;
        ballY[i] = newy;
    } else {
        float hb = TO_RAD*myH + TO_RAD*myBall.bearing();
        sincosf(hb, &sinHB, &cosHB);

        float newx = myX + myBall.distance()*cosHB;
        float newy = myY + myBall.distance()*sinHB;

        ballX[i] = newx;
        ballY[i] = newy;
    }
}

/*
 * Calculates distance between two players' balls. (squared because no need sqrt)
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

int SharedBallModule::getQuadrantNumber(int i)
{
    calculateBallCoords(i);
    if (ballX[i] < MIDFIELD_X && ballY[i] < MIDFIELD_Y)
    {
        return 1;
    }
    else if (ballX[i] < MIDFIELD_X && ballY[i] > MIDFIELD_Y)
    {
        return 2;
    }
    else if (ballX[i] > MIDFIELD_X && ballY[i] < MIDFIELD_Y)
    {
        return 3;
    }
    else
    {
        return 4;
    }
}

bool SharedBallModule::inGoalieBox(float x, float y) {
    if ((x > FIELD_WHITE_LEFT_SIDELINE_X - GOAL_DEPTH) and
        (x <  FIELD_WHITE_LEFT_SIDELINE_X + GOALBOX_DEPTH) and
        (y > BLUE_GOALBOX_BOTTOM_Y) and
        (y < BLUE_GOALBOX_BOTTOM_Y + GOALBOX_WIDTH))
    {
        return true;
    }
    return false;
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
