#pragma once

namespace man
{
namespace balltrack
{

    struct KalmanFilterParams
    {
        float transXDeviation;   //Deviation from odometry in x direction
        float transYDeviation;   //Deviation from odometry in y direction
        float rotationDeviation; //Deviation from odometry in rotation
        float obsvRelXVariance;  //Variance from observing a ball
        float obsvRelYVariance;  //Variance from observing a ball in y
        float processDeviationPosX;  //Deviation from difference between model & world
        float processDeviationPosY;
        float processDeviationVelX;
        float processDeviationVelY;
        float ballFriction;      // cm/sec^2
    };

    struct MMKalmanFilterParams
    {
        int numFilters;          // Number of filters (even)
        int framesTillReset;   // Frames we can see a ball and not reset
        float initCovX;
        float initCovY;
        float initCovVelX;
        float initCovVelY;
        float movingThresh;
        int bufferSize;       // Size of buffer used to re-init moving filters
        float badStationaryThresh;
    };

    /*
     * @brief Struct to take observations from vision
     */
    struct BallObservation
    {
        BallObservation(float dist_, float bear_) : dist(dist_), bear(bear) {}
        BallObservation() {}
        float dist;
        float bear;
    };

    /*
     * @brief Struct to pass back Cartesian Ball Estimate
     */
    struct CartesianBallEstimate
    {
        CartesianBallEstimate(float relX_, float relY_) : relX(relX_), relY(relY_) {}
        CartesianBallEstimate() {}
        float relX;
        float relY;
    };

    /*
     * @brief Struct to use when storing visual history
     */
    struct CartesianObservation
    {
        CartesianObservation(float relX_, float relY_) : relX(relX_), relY(relY_) {}
        CartesianObservation() {}
        float relX;
        float relY;
    };



} // namespace balltrack
} // namespace man
