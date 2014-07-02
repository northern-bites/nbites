#include "SharedBall.h"

namespace man {
namespace context {

SharedBallModule::SharedBallModule() :
    portals::Module(),
    sharedBallOutput(base())
{
    //initialize variables
    numx = 0;
    numy = 0;
    sumweight = 0;
    weight = 0;
    frames_on = 0;
    frames_off = 0;
}

SharedBallModule::~SharedBallModule()
{
}


void SharedBallModule::run_() {

    // makes a weighted average of the ball locations from each robot that
    //sees the ball
    weightedavg();

    portals::Message<messages::SharedBall> sharedBallMessage(0);


    // sets the message
    sharedBallMessage.get()->set_x(x);
    sharedBallMessage.get()->set_y(y);

    // TODO turned off, waiting on Megan's PR
    sharedBallMessage.get()->set_ball_on(false);
    sharedBallMessage.get()->set_frames_on(0);
    sharedBallMessage.get()->set_frames_off(100);

    sharedBallOutput.setMessage(sharedBallMessage);
}

// makes a weighted average of the locations from
// each robot that sees the ball
void SharedBallModule::weightedavg() {
    numx = 0;
    numy = 0;
    sumweight = 0;
    for (int i=0; i<NUM_PLAYERS_PER_TEAM; i++) {

        worldModelIn[i].latch();

        //the goalie
        // if(i == 0){
        //     incorporateGoalieWorldModel(worldModelIn[i].message());
        //      }
        // the other players

        // else{

        incorporateWorldModel(worldModelIn[i].message());

            // }

        numx = numx + x*weight;
        numy = numy + y*weight;
        sumweight = sumweight + weight;
    }

    //at least one robot sees the ball
    if (sumweight != 0) {
        x = numx / sumweight;
        y = numy / sumweight;
        frames_on++;
        frames_off = 0;
        ball_on = true;
    }
    //no one sees the ball
    else {
        // if none of the robots see the ball
        x = -100;
        y = -100;
        frames_off++;
        frames_on = 0;
        ball_on = false;
    }

}

void SharedBallModule::incorporateWorldModel(messages::WorldModel newModel) {
    if(newModel.ball_on()) {
        float uncert = newModel.my_uncert();
        float dist = newModel.ball_dist();
        weight = 1/(dist * uncert);


        float hb = TO_RAD*newModel.my_h() + TO_RAD*newModel.ball_bearing();
        float sinHB, cosHB;
        sincosf(hb, &sinHB, &cosHB);

        x = newModel.my_x() + newModel.ball_dist()*cosHB;
        y = newModel.my_y() + newModel.ball_dist()*sinHB;

    }
    else{
        x = 0;
        y = 0;
        weight = 0;
    }
}

// As of now we are not calling this method but if we decide the goalie should
// assume a fixed postition, call this in weightedAvg if i = 0
/*
void SharedBallModule::incorporateGoalieWorldModel(messages::WorldModel newModel) {
    if(newModel.ball_on()) {

        float uncert = newModel.my_uncert();
        float dist = newModel.ball_dist();
        weight = 1/(dist * uncert);

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
