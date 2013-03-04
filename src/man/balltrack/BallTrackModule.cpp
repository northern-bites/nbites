#include "BallTrackModule.h"

namespace man
{
    namespace balltrack
    {

    BallTrackModule::BallTrackModule() :
        portals::Module(),
        ballLocationOutput(base())
    {
        ballX = 1.f;
        ballY = 2.f;
    }

    BallTrackModule::~BallTrackModule()
    {
    }

    void BallTrackModule::run_()
    {
        portals::Message<messages::RobotLocation> ballMessage(0);

        ballMessage.get()->set_x(ballX);
        ballMessage.get()->set_y(ballY);

        ballLocationOutput.setMessage(ballMessage);
    }

    } //namespace balltrack
} //namespace man
