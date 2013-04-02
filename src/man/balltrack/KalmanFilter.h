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

#include "BallTrackStructs.h"

using namespace NBMath;

namespace man{
namespace balltrack{

// HACK HACK HACK - Currently completely made up and untested
// @TODO test this and determine correct values
static const KalmanFilterParams DEFAULT_PARAMS =
{
                            // @TODO Verify data set (newby?)
    .00117784f,             // transXDeviation   CALC March 2013
    .01263870f,             // trandYDeviation   CALC March 2013
    .00420612f,             // rotationDeviation CALC March 2013
    .6f,                    // processDeviation
    .6f,
    .6f,
    .6f,
    1.f                     // ballFriction?
};

class KalmanFilter
{

public:
    KalmanFilter(KalmanFilterParams params_ = DEFAULT_PARAMS);
    ~KalmanFilter();

    void update(messages::VisionBall visionBall,
                messages::Motion     motion);

    bool isUpdated() {return updated;};
    void setUpdate(bool updated_){updated = updated_;};

    void initialize();
    void initialize(ufvector4 x_, ufmatrix4 cov_);

    // HACK - TEMP FOR TESTING
    // In theory should be private
    void predict(messages::RobotLocation odometry, float deltaT);
    void updateWithObservation(messages::VisionBall visionBall);

    ufvector4 getStateEst(){return x;};
    float getRelXPosEst(){return x(0);};
    float getRelYPosEst(){return x(1);};
    float getRelXVelEst(){return x(2);};
    float getRelYVelEst(){return x(3);};

    ufmatrix4 getCovEst(){return cov;};
    float getCovXPosEst(){return cov(0,0);};
    float getCovYPosEst(){return cov(1,1);};
    float getCovXVelEst(){return cov(2,2);};
    float getCovYVelEst(){return cov(3,3);};

private:
    KalmanFilterParams params;

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
