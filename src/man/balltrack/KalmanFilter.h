/**
 * KalmanFilter - Implementation of a Kalman filter for ball tracking
 *
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   March 2013
 */

#pragma once

#include "NBMath.h"
#include "NBMatrixMath.h"

#include "BallModel.pb.h"
#include "Motion.pb.h"

namespace man
{
namespace balltrack
{

class KalmanFilter
{
public:
    KalmanFilter();
    ~KalmanFilter();

    void update(messages::VisionBall visionBall,
                messages::Motion odometry);

    bool isUpdated() {return updated;};
    void setUpdate(bool updated_){updated = updated_;};

private:
    bool updated;

};


} //namespace balltrack
} //namespace man
