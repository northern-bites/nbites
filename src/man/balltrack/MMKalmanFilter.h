/**
 * Multimodel Kalman Filter
 *
 * The idea is to have some even number of filters with half assuming the Kalman Filter is
 * is stationary, the other half assuming the ball is moving with some velocity. Each frame
 * the 'best' model is chosen and used to output the estimates. The worst filter of each type
 * (stationary and moving) are also re-initialized based on the previous frames. Boom. Robocup Solved.
 *
 */
#pragma once

#include "KalmanFilter.h"

#include "NBMath.h"

#include "BallModel.pb.h"
#include "Motion.pb.h"
#include "Common.pb.h"

#include <boost/shared_ptr.hpp>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/math/constants/constants.hpp>

#include <vector>

using namespace NBMath;

namespace man{
namespace balltrack{

static const MMKalmanFilterParams DEFAULT_MM_PARAMS =
{
    12,                 // numFilters
    200,                // framesTillReset
    10.f,               // initCovX
    10.f,               // initCovY
    25.f,               // initCovVelX
    25.f                // initCovVelY
};

class MMKalmanFilter
{

public:
    MMKalmanFilter(MMKalmanFilterParams params_ = DEFAULT_MM_PARAMS);
    ~MMKalmanFilter();

    void update(messages::VisionBall visionBall,
                messages::Motion     motion);

    ufvector4 getStateEst(){return stateEst;};
    float getRelXPosEst(){return stateEst(0);};
    float getRelYPosEst(){return stateEst(1);};
    float getRelXVelEst(){return stateEst(2);};
    float getRelYVelEst(){return stateEst(3);};

    ufmatrix4 getCovEst(){return covEst;};
    float getCovXPosEst(){return covEst(0,0);};
    float getCovYPosEst(){return covEst(1,1);};
    float getCovXVelEst(){return covEst(2,2);};
    float getCovYVelEst(){return covEst(3,3);};

    bool isStationary(){return stationary;};

    void printEst(){std::cout << "Filter Estimate:\n\t"
                              << "'Stationary' is\t" << stationary << "\n\t"
                              << "X-Pos:\t" << stateEst(0) << "Y-Pos:\t" << stateEst(1)
                              << "\n\t"
                              << "x-Vel:\t" << stateEst(2) << "y-Vel:\t" << stateEst(3)
                              << "\n\t"
                              << "Uncertainty x:\t" << covEst(0,0) << "\t,\t"
                              << "y:\t" << covEst(1,1) << std::endl;};

    void initialize(float relX, float relY, float covX, float covY);
private:
    void predictFilters(messages::RobotLocation odometry);
    void predictFilters(messages::RobotLocation odometry, float t);
    void updateWithVision(messages::VisionBall visionBall);

    void cycleFilters();

    unsigned normalizeFilterWeights();
    void updateDeltaTime();

    ufvector4 vector4D(float x, float y, float z, float w);

    MMKalmanFilterParams params;

    std::vector<KalmanFilter *> filters;

    int framesWithoutBall;


    ufvector4 prevStateEst;
    ufmatrix4 prevCovEst;

    ufvector4 stateEst;
    ufmatrix4 covEst;


    unsigned bestFilter;

    bool stationary;
    bool consecutiveObservation;

    float lastVisRelX;
    float lastVisRelY;
    float visRelX;
    float visRelY;

    // Keep track of real time passing for calculations
    long long int lastUpdateTime;
    float deltaTime;

};


} //namespace balltrack
} //namespace man
