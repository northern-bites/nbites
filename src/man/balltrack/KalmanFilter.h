/**
 * KalmanFilter - Implementation of a Kalman filter for ball tracking
 *
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   March 2013
 */

#pragma once

#include "NBMath.h"
#include "NBMatrixMath.h"
#include "NBVector.h"

#include "BallModel.pb.h"
#include "Motion.pb.h"
#include "Common.pb.h"

using namespace NBMath;

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
                messages::Motion     motion);

    bool isUpdated() {return updated;};
    void setUpdate(bool updated_){updated = updated_;};

    void initialize();

    // HACK - TEMP FOR TESTING
    void predict(messages::RobotLocation odometry, float deltaT);

    ufvector4 getStateEst(){return x;};
    float getRelXPosEst(){return x(0);};
    float getRelYPosEst(){return x(1);};

private:
    bool updated;

    // x is the state estimate, it holds relX & relY position and
    //                                   relX & relY velocity
    // This is the convention for all matrices and vectors
    ufvector4 x;

    // Covariance matrix for the state estimation
    ufmatrix4 cov;


    // Last calculated kalman gain
    float gain;

    // Time passed since last vision observation
    float deltaTime;

    // true if the filter assumes the ball is stationary, if stationary
    // is true, the velocitys should be zero
    bool stationary;

    //temp move to public for testing
//    void predict(messages::RobotLocation odometry, float deltaT);
    ufvector4 vector4D(float x, float y, float z, float w);
};


} //namespace balltrack
} //namespace man
