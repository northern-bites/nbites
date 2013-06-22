#include "SharedBall.h"

namespace man {
namespace context {

SharedBallModule::SharedBallModule() :
    portals::Module(),
    sharedBallOutput(base())
{
    x = CENTER_FIELD_X;
    y = CENTER_FIELD_Y;

    framesSinceUpdate = 0;
}

SharedBallModule::~SharedBallModule()
{
}

void SharedBallModule::run_() {
    updatedThisFrame = false;

    for (int i=0; i<NUM_PLAYERS_PER_TEAM; i++) {
        worldModelIn[i].latch();
        if(i == 0)
            incorporateGoalieWorldModel(worldModelIn[i].message());
        // else
        //     incorporateWorldModel(worldModelIn[i].message());
    }

    if(updatedThisFrame)
        framesSinceUpdate = 0;
    else
        framesSinceUpdate++;

    portals::Message<messages::SharedBall> sharedBallMessage(0);

    sharedBallMessage.get()->set_x(x);
    sharedBallMessage.get()->set_y(y);
    sharedBallMessage.get()->set_age(framesSinceUpdate);

    sharedBallOutput.setMessage(sharedBallMessage);
}

void SharedBallModule::incorporateWorldModel(messages::WorldModel newModel) {
    if(newModel.ball_on()) {
        // heading + bearing
        float hb = TO_RAD*newModel.my_h() + TO_RAD*newModel.ball_bearing();
        float sinHB, cosHB;
        sincosf(hb, &sinHB, &cosHB);

        float globalX = newModel.my_x() + newModel.ball_dist()*cosHB;
        float globalY = newModel.my_y() + newModel.ball_dist()*sinHB;

        x = ALPHA*globalX + (1-ALPHA)*x;
        y = ALPHA*globalY + (1-ALPHA)*y;
        updatedThisFrame = true;
    }
}

void SharedBallModule::incorporateGoalieWorldModel(messages::WorldModel newModel) {
    if(newModel.ball_on()) {
        // heading + bearing

        // Assume goalie in position (FIELD_WHITE_LEFT_SIDELINE,
        //                            CENTER_FIELD_Y,
        //                            HEADING_RIGHT

        float hb = TO_RAD*HEADING_RIGHT + TO_RAD*newModel.ball_bearing();
        float sinHB, cosHB;
        sincosf(hb, &sinHB, &cosHB);

        float globalX = FIELD_WHITE_LEFT_SIDELINE_X + newModel.ball_dist()*cosHB;
        float globalY = CENTER_FIELD_Y + newModel.ball_dist()*sinHB;

        x = ALPHA*globalX + (1-ALPHA)*x;
        y = ALPHA*globalY + (1-ALPHA)*y;
        updatedThisFrame = true;
    }
}



} // namespace man
} // namespace context
