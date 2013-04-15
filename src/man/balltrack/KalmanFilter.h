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
#include "Common.h"

#include "BallModel.pb.h"
#include "Motion.pb.h"
#include "RobotLocation.pb.h"

#include "BallTrackStructs.h"

using namespace NBMath;

namespace man{
namespace balltrack{

// HACK HACK HACK - Currently completely made up and untested
// @TODO test this and determine correct values
static const KalmanFilterParams DEFAULT_PARAMS =
{
                            // @TODO Verify data set (newby?)
    .5f,             // transXDeviation   CALC March 2013
    .5f,             // trandYDeviation   CALC March 2013
    .5f,             // rotationDeviation CALC March 2013
    10.f,            // obsvRelX deviation
    10.f,            // obsvRelY deviation
    .1f,                    // processDeviation
    .1f,
    .1f,
    .1f,
    -29.f                     // ballFriction?
};

class KalmanFilter
{

public:
    KalmanFilter(bool stationary_ = true,
                 KalmanFilterParams params_ = DEFAULT_PARAMS);
    ~KalmanFilter();

    void update(messages::VisionBall visionBall,
                messages::RobotLocation     motion);

    bool isUpdated() {return updated;};
    void setUpdate(bool updated_){updated = updated_;};

    void initialize();
    void initialize(ufvector4 x_, ufmatrix4 cov_);

    void predict(messages::RobotLocation odometry);
    void predict(messages::RobotLocation odometry, float deltaT);
    void updateWithObservation(messages::VisionBall visionBall);
    void predictBallDest();

    // FOR OFFLINE TESTING
    messages::RobotLocation genOdometry(float x, float y, float h)
    {
        messages::RobotLocation odometry;
        odometry.set_x(x);
        odometry.set_y(y);
        odometry.set_h(h);

        return odometry;
    };
    messages::VisionBall genVisBall(float dist, float bear)
    {
        messages::VisionBall obsv;
        obsv.set_rel_x_variance(5.f);
        obsv.set_rel_y_variance(5.f);
        obsv.set_distance(dist);
        obsv.set_bearing(bear);
        obsv.set_on(true);

        return obsv;
    };



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

    float getFilteredDist(){return filteredDist;};
    float getFilteredBear(){return filteredBear;};

    float getRelXDest(){return relXDest;};
    float getRelYDest(){return relYDest;};
    float getRelYIntersectDest(){return relYIntersectDest;};

    float getWeight(){return weight;};
    void setWeight(float weight_){weight=weight_;};

    bool isStationary(){return stationary;};

    float getSpeed(){return std::sqrt(x(2)*x(2) + x(3)*x(3));};

    void printEst(){std::cout << "Filter Estimate:\n\t"
                              << "'Stationary' is\t" << stationary << "\n\t"
                              << "X-Pos:\t" << x(0) << "Y-Pos:\t" << x(1)
                              << "\n\t"
                              << "x-Vel:\t" << x(2) << "y-Vel:\t" << x(3)
                              << "\n\t"
                              << "Uncertainty x:\t" << cov(0,0) << "\t,\t"
                              << "y:\t" << cov(1,1) << std::endl;};

private:
    KalmanFilterParams params;

    bool updated;

    void updateDeltaTime();

    // x is the state estimate, it holds relX & relY position and
    //                                   relX & relY velocity
    // This is the convention for all matrices and vectors
    ufvector4 x;

    // Covariance matrix for the state estimation
    ufmatrix4 cov;


    // Last calculated kalman gain
    float gain;

    // Time passed since last vision observation
    long long int lastUpdateTime;
    float deltaTime;

    // true if the filter assumes the ball is stationary, if stationary
    // is true, the velocitys should be zero
    bool stationary;

    //temp move to public for testing
//    void predict(messages::RobotLocation odometry, float deltaT);
    ufvector4 vector4D(float x, float y, float z, float w);

    float filteredDist;
    float filteredBear;

    float relXDest;
    float relYDest;
    float relYIntersectDest;

    // For the MMKalman
    float weight;
    float uncertainty;
};


} //namespace balltrack
} //namespace man
