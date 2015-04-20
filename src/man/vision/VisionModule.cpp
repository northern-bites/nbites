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
                               inertials()
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

    BallDetector topBallDetector(&topOrangeImage.message());
    topBallDetector.findBalls();

    BallDetector bottomBallDetector(&bottomOrangeImage.message());
    bottomBallDetector.findBalls();

    HighResTimer timer("Gradient");

    gradient->reset();
    EdgeDetector edgeDetector;
    edgeDetector.sobelOperator(0, topYImage.message().pixelAddress(0, 0), *gradient);

    timer.end("Post");

    PostDetector postDetector(*gradient, topWhiteImage.message());

    timer.lap();

    PROF_EXIT(P_VISION);
}

}
}
