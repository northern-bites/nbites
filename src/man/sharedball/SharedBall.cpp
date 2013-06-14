#include "SharedBall.h"

namespace man {
namespace context {

SharedBallModule::SharedBallModule()
{
    x = 0.f;
    y = 0.f;
}

SharedBallModule::~SharedBallModule()
{
}

void SharedBallModule::run_() {
    for (int i=0; i<NUM_PLAYERS_PER_TEAM; i++) {
        worldModelIn[i].latch();
        incorporateWorldModel(worldModelIn[i].message());
    }

    std::cout << "Global ball (x,y):\t(" << x << " , " << y << " )" << std::endl;
}

void SharedBallModule::incorporateWorldModel(messages::WorldModel newModel) {
    if(newModel.ball_on()) {
        // heading + bearing
        float hb = TO_RAD*newModel.my_h() + TO_RAD*newModel.ball_bearing();
        float sinHB, cosHB;
        sincosf(hb, &sinHB, &cosHB);

        float globalX = newModel.my_x() + newModel.ball_dist()*cosHB;
        float globalY = newModel.my_y() + newModel.ball_dist()*sinHB;

        x = (x + globalX)/2;
        y = (y + globalY)/2;
    }
}

} // namespace man
} // namespace context
