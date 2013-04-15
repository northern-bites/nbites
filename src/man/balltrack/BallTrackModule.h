/*
 * @brief  Defines a module for tracking and predicting the ball
 *
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   March 2013
 */
#pragma once

#include "NaiveBallFilter.h"
#include "KalmanFilter.h"
#include "MMKalmanFilter.h"

#include "RoboGrams.h"
#include "BallModel.pb.h"
#include "RobotLocation.pb.h"
#include <boost/shared_ptr.hpp>

namespace man
{
namespace balltrack
{

class BallTrackModule : public portals::Module
{
public:
    BallTrackModule();
    ~BallTrackModule();

    portals::InPortal<messages::VisionBall> visionBallInput;
    portals::InPortal<messages::RobotLocation> odometryInput;
    portals::InPortal<messages::RobotLocation> localizationInput;
    portals::OutPortal<messages::FilteredBall> ballLocationOutput;

protected:
    /**
     * @brief called through diagrams
     */
    void run_();

private:
    MMKalmanFilter* filters;

    messages::RobotLocation lastOdometry;
    messages::RobotLocation curOdometry;
    messages::RobotLocation deltaOdometry;
};


} //namespace balltrack
} //namespace man
