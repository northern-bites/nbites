#include "VisionSimModule.h"
#include <iostream>

using namespace portals;

namespace tool{
namespace visionsim{

VisionSimModule::VisionSimModule() : QObject(),
                                     Module(),
                                     world(),
                                     topImage(world, TOP),
                                     bottomImage(world, BOTTOM),
                                     gui(topImage, bottomImage, world)
{
    connect(&gui.controls, SIGNAL(robotMoved(float, float, float)), this,
            SLOT(updateRobot(float, float, float)));
    connect(&gui.controls, SIGNAL(headMoved(float, float)), this,
            SLOT(updateHead(float, float)));
    connect(&gui.controls, SIGNAL(ballMoved(float, float)), this,
            SLOT(updateBall(float, float)));

    connect(this, SIGNAL(modelChanged()), &gui, SLOT(updateView()));

}

void VisionSimModule::updateRobot(float x, float y, float h)
{
    world.moveRobotTo(x, y, h);
    updateImages();
    emit(modelChanged());
}

void VisionSimModule::updateHead(float yaw, float pitch)
{
    world.moveHeadTo(yaw, pitch);
    updateImages();
    emit(modelChanged());
}

void VisionSimModule::updateBall(float x, float y)
{
    world.moveBallTo(x, y);
    updateImages();
    emit(modelChanged());
}

void VisionSimModule::updateImages()
{
    topImage.update();
    bottomImage.update();
}

void VisionSimModule::run_()
{
    std::cout << "Vision Sim run!" << std::endl;
}

}
}
