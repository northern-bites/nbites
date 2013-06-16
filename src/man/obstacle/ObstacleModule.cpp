#include "ObstacleModule.h"

namespace man {
namespace obstacle {

ObstacleModule::ObstacleModule() : obstacleOut(base())
{
}

void ObstacleModule::run_()
{
    armContactIn.latch();
    visionIn.latch();
    footBumperIn.latch();
    sonarIn.latch();
}

}
}
