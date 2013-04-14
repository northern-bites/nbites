#include "BallTrackModule.h"
#include "NBMath.h"
#include <math.h>
#include "NBMath.h"

#include "DebugConfig.h"

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
    localizationInput.latch();


    // NOTE: Kalman Filter wants to get deltaX, deltaY, etc...
    lastOdometry.set_x(curOdometry.x());
    lastOdometry.set_y(curOdometry.y());
    lastOdometry.set_h(curOdometry.h());

    curOdometry.set_x(odometryInput.message().x());
    curOdometry.set_y(odometryInput.message().y());
    curOdometry.set_h(odometryInput.message().h());

    deltaOdometry.set_x(curOdometry.x() - lastOdometry.x());
    deltaOdometry.set_y(curOdometry.y() - lastOdometry.y());
    deltaOdometry.set_h(curOdometry.h() - lastOdometry.h());

    // Update the Ball filter
    // NOTE: Should be tested but having the same observation twice will be damaging
    //       should try and avoid a const cast, but may need same hack as motion...
    filters->update(visionBallInput.message(),
                    deltaOdometry);

    // Fill the ballMessage with the filters representation
    portals::Message<messages::FilteredBall> ballMessage(0);

    ballMessage.get()->mutable_vis()->CopyFrom(visionBallInput.message());
    ballMessage.get()->set_distance(filters->getFilteredDist());
    ballMessage.get()->set_bearing(filters->getFilteredBear());
    ballMessage.get()->set_bearing_deg(filters->getFilteredBear() * TO_DEG);

    // From Wils for behaviors
    float x = localizationInput.message().x() +
        ballMessage.get()->distance() * cosf(localizationInput.message().h() +
                                             ballMessage.get()->bearing());
    float y = localizationInput.message().y() +
        ballMessage.get()->distance() * sinf(localizationInput.message().h() +
                                             ballMessage.get()->bearing());
    ballMessage.get()->set_x(x);
    ballMessage.get()->set_y(y);

    ballMessage.get()->set_rel_x(filters->getRelXPosEst());
    ballMessage.get()->set_rel_y(filters->getRelYPosEst());
    ballMessage.get()->set_vel_x(filters->getRelXVelEst());
    ballMessage.get()->set_vel_y(filters->getRelYVelEst());

    ballMessage.get()->set_var_rel_x(filters->getCovXPosEst());
    ballMessage.get()->set_var_rel_y(filters->getCovYPosEst());
    ballMessage.get()->set_var_vel_x(filters->getCovXVelEst());
    ballMessage.get()->set_var_vel_y(filters->getCovYVelEst());

    ballMessage.get()->set_is_stationary(filters->isStationary());

#ifdef DEBUG_BALLTRACK
    // Print the observation given, each filter after update, and which filter chosen

    if(visionBallInput.message().on()) {
        std::cout << "See a ball with (dist,bearing):\t( " << visionBallInput.message().distance()
                  << " , " << visionBallInput.message().bearing() << " )" << std::endl;
    }

    std::cout << "Odometry is (x,y,h):\t( " << deltaOdometry.x() << " , "
              << deltaOdometry.y() << " , " << deltaOdometry.h() << " )" << std::endl;

    if(filters->isStationary())
        std::cout << "The STATIONARY filter is best modeling the ball" << std::endl;
    else
        std::cout << "The MOVING filter is best modeling the ball" << std::endl;

    filters->printBothFilters();
#endif




    ballLocationOutput.setMessage(ballMessage);
}

} //namespace balltrack
} //namespace man
