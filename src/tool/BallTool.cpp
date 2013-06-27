#include "BallTool.h"

namespace tool {

BallTool::BallTool(const char* title) :
    ballView(this),
    EmptyTool(title)
{
    toolTabs->addTab(&ballView, tr("BallView"));

    connect(&diagram, SIGNAL(signalUnloggersReady()),
            this, SLOT(setUpModules()) );
}

BallTool::~BallTool() {
}

void BallTool::setUpModules()
{
    if(diagram.connectToUnlogger<messages::RobotLocation>(ballView.odometryIn,
                                                          "odometry"))
        std::cout << "Connected odometry logs" << std::endl;
    else
        std::cout << "WARNING: no odometry logs for ball tool :(" << std::endl;

    if(diagram.connectToUnlogger<messages::FilteredBall>(ballView.ballIn,
                                                          "filtered_ball"))
        std::cout << "Connected ballfilter logs" << std::endl;
    else
        std::cout << "WARNING: no filtered ball logs for ball tool :(" << std::endl;

    if(diagram.connectToUnlogger<messages::VisionBall>(ballView.visionBallIn,
                                                       "vision_ball"))
        std::cout << "Connected vision ball filter logs" << std::endl;
    else
        std::cout << "WARNING: no vision ball logs for ball tool :(" << std::endl;

    if(diagram.connectToUnlogger<messages::RobotLocation>(ballView.localizationIn,
                                                          "location"))
        std::cout << "Connected ballfilter logs" << std::endl;
    else
        std::cout << "WARNING: no filtered ball logs for ball tool :(" << std::endl;

    diagram.addModule(ballView);
}

} // namespace tool
