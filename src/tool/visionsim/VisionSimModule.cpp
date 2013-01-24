#include "VisionSimModule.h"
#include <iostream>

using namespace portals;

namespace tool{
namespace visionsim{

VisionSimModule::VisionSimModule() : QObject(),
                                     Module(),
                                     world(),
                                     topImage(world, TOP),
                                     bottomImage(world, BOTTOM)
{}

void VisionSimModule::updateRobot(float x, float y, float h)
{
    world.moveRobotTo(x, y, h);
}

void VisionSimModule::updateHead(float yaw, float pitch)
{
    world.moveHeadTo(yaw, pitch);
}

void VisionSimModule::updateBall(float x, float y)
{
    world.moveBallTo(x, y);
}

void VisionSimModule::run_()
{
    std::cout << "Vision Sim run!" << std::endl;
}

}
}
