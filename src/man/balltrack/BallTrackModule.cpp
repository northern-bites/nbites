#include "BallTrackModule.h"

namespace man
{
    namespace balltrack
    {

    BallTrackModule::BallTrackModule() :
        portals::Module(),
        ballLocationOutput(base())
    {
        ballFilter = new BallFilter();

        ballX = 1.f;
        ballY = 2.f;
    }

    BallTrackModule::~BallTrackModule()
    {
    }

    void BallTrackModule::run_()
    {
        // Latch

        // Update the Ball filter

        // Fill the ballMessage with the filters representation


        portals::Message<messages::FilteredBall> ballMessage(0);

        ballMessage.get()->set_filter_rel_x(ballX);
        ballMessage.get()->set_filter_rel_y(ballY);

        ballLocationOutput.setMessage(ballMessage);
    }

    } //namespace balltrack
} //namespace man
