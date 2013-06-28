#include "BallTrackModule.h"
#include "NBMath.h"
#include <math.h>
#include "NBMath.h"
#include "Profiler.h"
#include "FieldConstants.h"

#include "DebugConfig.h"

namespace man {
namespace balltrack {

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
    PROF_ENTER(P_BALL_TRACK);

    // Latch
    visionBallInput.latch();
    odometryInput.latch();
    localizationInput.latch();

    curOdometry.set_x(odometryInput.message().x());
    curOdometry.set_y(odometryInput.message().y());
    curOdometry.set_h(odometryInput.message().h());

    // Update the Ball filter
    // NOTE: Should be tested but having the same observation twice will be damaging
    //       should try and avoid a const cast, but may need same hack as motion...
    filters->update(visionBallInput.message(),
                    curOdometry);

    // Fill the ballMessage with the filters representation
    portals::Message<messages::FilteredBall> ballMessage(0);

    ballMessage.get()->mutable_vis()->CopyFrom(visionBallInput.message());
    ballMessage.get()->set_distance(filters->getFilteredDist());
    ballMessage.get()->set_bearing(filters->getFilteredBear());
    ballMessage.get()->set_bearing_deg(filters->getFilteredBear() * TO_DEG);

    float x = localizationInput.message().x() +
        filters->getFilteredDist() * cosf(localizationInput.message().h() +
                                          filters->getFilteredBear());
    float y = localizationInput.message().y() +
        filters->getFilteredDist() * sinf(localizationInput.message().h() +
                                          filters->getFilteredBear());

    // HACK!! WHY!!??!?!
    if (visionBallInput.message().frames_off() > 720 &&
        x < 70 && y < 70)
    {
        x = CENTER_FIELD_X;
        y = CENTER_FIELD_Y;
    }

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

    ballMessage.get()->set_rel_x_dest(filters->getRelXDest());
    ballMessage.get()->set_rel_y_dest(filters->getRelYDest());
    ballMessage.get()->set_rel_y_intersect_dest(filters->getRelYIntersectDest());
    ballMessage.get()->set_speed(filters->getSpeed());

    ballMessage.get()->set_stat_rel_x(filters->getStationaryRelX());
    ballMessage.get()->set_stat_rel_y(filters->getStationaryRelY());
    ballMessage.get()->set_stat_distance(filters->getStationaryDistance());
    ballMessage.get()->set_stat_bearing(filters->getStationaryBearing());

    ballMessage.get()->set_mov_rel_x(filters->getMovingRelX());
    ballMessage.get()->set_mov_rel_y(filters->getMovingRelY());
    ballMessage.get()->set_mov_vel_x(filters->getMovingVelX());
    ballMessage.get()->set_mov_vel_y(filters->getMovingVelY());
    ballMessage.get()->set_mov_distance(filters->getMovingDistance());
    ballMessage.get()->set_mov_bearing(filters->getMovingBearing());
    ballMessage.get()->set_mov_speed(filters->getMovingSpeed());

#ifdef DEBUG_BALLTRACK
    // Print the observation given, each filter after update, and which filter chosen

    if(visionBallInput.message().on()) {
        std::cout << "See a ball with (dist,bearing):\t( "
                  << visionBallInput.message().distance()
                  << " , " << visionBallInput.message().bearing() << " )"
                  << std::endl;
        std::cout << "and a ball with (relX,relY):  \t( " << filters->visRelX
                  << " , " << filters->visRelY << std::endl;
    }

    std::cout << "Speed:\t" << filters->getSpeed() << std::endl;
    std::cout << "RelY Int Dest:\t" << filters->getRelYIntersectDest() << std::endl;

    std::cout << "Odometry is (x,y,h):\t( " << deltaOdometry.x() << " , "
              << deltaOdometry.y() << " , " << deltaOdometry.h() << " )" << std::endl;

    if(filters->isStationary())
        std::cout << "The STATIONARY filter is best modeling the ball" << std::endl;
    else
        std::cout << "The MOVING filter is best modeling the ball" << std::endl;

    filters->printBothFilters();
#endif




    ballLocationOutput.setMessage(ballMessage);

    PROF_EXIT(P_BALL_TRACK);
}

} //namespace balltrack
} //namespace man
