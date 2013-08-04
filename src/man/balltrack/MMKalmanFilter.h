/**
 * Multimodel Kalman Filter
 *
 * The idea is to have 2 filters, one assuming the ball is stationary,
 * one assuming the ball is moving. When the moving filter thinks the velocity
 * is above a threshold then we switch to using that filter for our estimates.
 * Otherwise we use the stationary filter
 *
 * The initial concept was to pull a B-Human and compute 12 filters, constantly
 * re-initializing the worst filters, but the inability to choose the best filter
 * consistently due to (obviously) noisy estimates lead to currently having 2.
 * There is still some legacy code from that attempt in case it wants to be
 * RESURRECTED! (spelling?)
 *
 */
#pragma once

#include "KalmanFilter.h"

#include "NBMath.h"

#include "BallModel.pb.h"
#include "Motion.pb.h"
#include "RobotLocation.pb.h"

#include <boost/shared_ptr.hpp>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/math/constants/constants.hpp>

#include <vector>

using namespace NBMath;

namespace man{
namespace balltrack{

static const bool TRACK_MOVEMENT = false;

static const MMKalmanFilterParams DEFAULT_MM_PARAMS =
{
    2,                  // numFilters
    500,                // framesTillReset
    10.f,               // initCovX
    10.f,               // initCovY
    25.f,               // initCovVelX
    25.f,               // initCovVelY
    35.f,               // threshold for ball is moving!
    7,                  // buffer size
    30.f                // badStationaryThresh
};

class MMKalmanFilter
{

public:
    MMKalmanFilter(MMKalmanFilterParams params_ = DEFAULT_MM_PARAMS);
    ~MMKalmanFilter();

    void update(messages::VisionBall    visionBall,
                messages::RobotLocation odometry);

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

    float getFilteredDist(){return filters.at((unsigned)bestFilter)->getFilteredDist();};
    float getFilteredBear(){return filters.at((unsigned)bestFilter)->getFilteredBear();};

    float getSpeed(){return filters.at((unsigned)1)->getSpeed();};
    float getRelXDest(){return filters.at((unsigned)1)->getRelXDest();};
    float getRelYDest(){return filters.at((unsigned)1)->getRelYDest();};
    float getRelYIntersectDest(){return filters.at((unsigned)1)->getRelYIntersectDest();};

    float getStationaryRelX(){return filters.at((unsigned) 0)->getRelXPosEst();};
    float getStationaryRelY(){return filters.at((unsigned) 0)->getRelYPosEst();};
    float getStationaryDistance(){return filters.at((unsigned) 0)->getFilteredDist();};
    float getStationaryBearing() {return filters.at((unsigned) 0)->getFilteredBear();};

    float getMovingRelX(){return filters.at((unsigned) 1)->getRelXPosEst();};
    float getMovingRelY(){return filters.at((unsigned) 1)->getRelYPosEst();};
    float getMovingVelX(){return filters.at((unsigned) 1)->getRelXVelEst();};
    float getMovingVelY(){return filters.at((unsigned) 1)->getRelYVelEst();};
    float getMovingDistance(){return filters.at((unsigned) 1)->getFilteredDist();};
    float getMovingBearing() {return filters.at((unsigned) 1)->getFilteredBear();};
    float getMovingSpeed() {return calcSpeed(getMovingVelX(), getMovingVelY());};

    bool isStationary(){return stationary;};

    void printBothFilters();

    void printEst(){std::cout << "Filter Estimate:\n\t"
                              << "'Stationary' is\t" << stationary << "\n\t"
                              << "X-Pos:\t" << stateEst(0) << "Y-Pos:\t" << stateEst(1)
                              << "\n\t"
                              << "x-Vel:\t" << stateEst(2) << "y-Vel:\t" << stateEst(3)
                              << "\n\t"
                              << "Uncertainty x:\t" << covEst(0,0) << "\t,\t"
                              << "y:\t" << covEst(1,1) << std::endl;};

    void initialize(float relX=50.f, float relY=50.f, float covX=50.f, float covY=50.f);

    float visRelX;
    float visRelY;
private:
    void predictFilters(messages::RobotLocation odometry);
    void predictFilters(messages::RobotLocation odometry, float t);
    void updateWithVision(messages::VisionBall visionBall);

    void updatePredictions();

    void updateDeltaTime();

    CartesianObservation calcVelocityOfBuffer();
    float diff(float a, float b);
    float calcSpeed(float a, float b);

    MMKalmanFilterParams params;

    std::vector<KalmanFilter *> filters;

    int framesWithoutBall;


    ufvector4 prevStateEst;
    ufmatrix4 prevCovEst;

    ufvector4 stateEst;
    ufmatrix4 covEst;

    // Keep track of the last couple observations
    CartesianObservation *obsvBuffer;
    int curEntry;
    bool fullBuffer;

    int bestFilter;

    bool stationary;
    bool consecutiveObservation;

    float lastVisRelX;
    float lastVisRelY;


    // Keep track of real time passing for calculations
    long long int lastUpdateTime;
    float deltaTime;

};


} //namespace balltrack
} //namespace man
