#include "VisionModule.h"

#include <iostream>
#include <chrono>

#include "Profiler.h"
#include "HighResTimer.h"
#include "BallDetector.h"
#include "EdgeDetector.h"
#include "PostDetector.h"

using namespace portals;

namespace man {
namespace vision {

VisionModule::VisionModule() : Module(),
                               topYImage(),
                               topWhiteImage(),
                               topOrangeImage(),
                               topGreenImage(),
                               bottomYImage(),
                               bottomOrangeImage(),
                               bottomWhiteImage(),
                               bottomGreenImage(),
                               joints(),
                               inertials(),
                               vision_field(base()),
                               vision_ball(base()),
                               vision_robot(base()),
                               vision_obstacle(base())
{
    gradient = new Gradient();
}

VisionModule::~VisionModule()
{
    delete gradient;
}

void VisionModule::run_()
{
    topYImage.latch();
    topWhiteImage.latch();
    topOrangeImage.latch();
    topGreenImage.latch();
    bottomYImage.latch();
    bottomWhiteImage.latch();
    bottomOrangeImage.latch();
    bottomGreenImage.latch();
    joints.latch();
    inertials.latch();

    PROF_ENTER(P_VISION);

    // BallDetector topBallDetector(&topOrangeImage.message());
    // topBallDetector.findBalls();

    // BallDetector bottomBallDetector(&bottomOrangeImage.message());
    // bottomBallDetector.findBalls();

    gradient->reset();
    EdgeDetector edgeDetector;
    edgeDetector.sobelOperator(0, topYImage.message().pixelAddress(0, 0), *gradient);

    PostDetector postDetector(*gradient, topWhiteImage.message());

    PROF_EXIT(P_VISION);    
    
    portals::Message<messages::VisionField> field_data(0);
    vision_field.setMessage(field_data);

    portals::Message<messages::VisionBall> ball_data(0);
    vision_ball.setMessage(ball_data);

    portals::Message<messages::VisionRobot> robot_data(0);
    vision_robot.setMessage(robot_data);

    portals::Message<messages::VisionObstacle> obstacle_data(0);
    vision_obstacle.setMessage(obstacle_data);
}

}
}
