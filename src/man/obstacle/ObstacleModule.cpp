#include "ObstacleModule.h"

namespace man {
namespace obstacle {

using messages::Obstacle;

ObstacleModule::ObstacleModule() : obstacleOut(base())
{
}

void ObstacleModule::run_()
{
    armContactIn.latch();
    visionIn.latch();
    footBumperIn.latch();
    sonarIn.latch();

    std::cout << visionIn.message().DebugString() << std::endl;
}

Obstacle::ObstaclePosition processArms(messages::ArmContactState& input)
{
    return Obstacle::NONE;
}

}
}
