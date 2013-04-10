#include "BallTrackModule.h"
#include "NBMath.h"
#include <math.h>

#include "NBMath.h"

namespace man
{
namespace balltrack
{

BallTrackModule::BallTrackModule() :
    portals::Module(),
    ballLocationOutput(base())
{
    filters = new MMKalmanFilter();
}

BallTrackModule::~BallTrackModule()
{
    delete filters;
}

void BallTrackModule::run_()
{
    // Latch
    visionBallInput.latch();
    odometryInput.latch();

    // Update the Ball filter
    // NOTE: Should be tested but having the same observation twice will be damaging
    //       should try and avoid a const cast, but may need same hack as motion...
    filters->update(visionBallInput.message(),
                    odometryInput.message());

    // Fill the ballMessage with the filters representation
    portals::Message<messages::FilteredBall> ballMessage(0);

    ballMessage.get()->mutable_vis()->CopyFrom(visionBallInput.message());
    ballMessage.get()->set_distance(filters->getFilteredDist());
    ballMessage.get()->set_bearing(filters->getFilteredBear());
    ballMessage.get()->set_bearing_deg(filters->getFilteredBear() * TO_DEG);

    ballMessage.get()->set_rel_x(filters->getRelXPosEst());
    ballMessage.get()->set_rel_y(filters->getRelYPosEst());
    ballMessage.get()->set_vel_x(filters->getRelXVelEst());
    ballMessage.get()->set_vel_y(filters->getRelYVelEst());

    ballMessage.get()->set_var_rel_x(filters->getCovXPosEst());
    ballMessage.get()->set_var_rel_y(filters->getCovYPosEst());
    ballMessage.get()->set_var_vel_x(filters->getCovXVelEst());
    ballMessage.get()->set_var_vel_y(filters->getCovYVelEst());

    ballMessage.get()->set_is_stationary(filters->isStationary());

    ballLocationOutput.setMessage(ballMessage);
}

} //namespace balltrack
} //namespace man
