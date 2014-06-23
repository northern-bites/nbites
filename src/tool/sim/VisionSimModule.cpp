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
    // Link the controls to the correct parts of the model
    connect(&gui.controls, SIGNAL(robotMoved(float, float, float)), this,
            SLOT(updateRobot(float, float, float)));
    connect(&gui.controls, SIGNAL(headMoved(float, float)), this,
            SLOT(updateHead(float, float)));
    connect(&gui.controls, SIGNAL(ballMoved(float, float)), this,
            SLOT(updateBall(float, float)));

    // Link the model to the viewers so that they can update appropriately
    connect(this, SIGNAL(modelChanged()), &gui, SLOT(updateView()));

}

/*
 * Move the robot around in the world.
 *
 * @param x -- the robot's new x position
 * @param y -- the robot's new y position
 * @param h -- the robot's new heading
 */
void VisionSimModule::updateRobot(float x, float y, float h)
{
    world.moveRobotTo(x, y, h);
    updateImages();
    emit(modelChanged());
}

/*
 * Move the robot's head. We DO NOT control roll.
 * @see Image worldToCameraCoords
 *
 * @param yaw -- the robot's new head yaw
 * @param pitch -- the robot's new head pitch
 */

void VisionSimModule::updateHead(float yaw, float pitch)
{
    world.moveHeadTo(yaw, pitch);
    updateImages();
    emit(modelChanged());
}

/*
 * Move the ball around the field.
 *
 * @param x -- the ball's new x position
 * @param y -- the ball's new y position
 */

void VisionSimModule::updateBall(float x, float y)
{
    world.moveBallTo(x, y);
    updateImages();
    emit(modelChanged());
}

// Update both images at one time; helper method
void VisionSimModule::updateImages()
{
    topImage.update();
    bottomImage.update();
}

// Not really implemented yet! Will output stuff.
void VisionSimModule::run_()
{
    std::cout << "Vision Sim run!" << std::endl;
}

}
}
