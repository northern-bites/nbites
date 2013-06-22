#include "SharedBall.h"

namespace man {
namespace context {

SharedBallModule::SharedBallModule()
{
    x = CENTER_FIELD_X;
    y = CENTER_FIELD_Y;
}

SharedBallModule::~SharedBallModule()
{
}

void SharedBallModule::run_() {
    for (int i=0; i<NUM_PLAYERS_PER_TEAM; i++) {
        worldModelIn[i].latch();
        if(i == 0)
            incorporateGoalieWorldModel(worldModelIn[i].message());
        // else
        //     incorporateWorldModel(worldModelIn[i].message());
    }
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
    }
}



} // namespace man
} // namespace context
