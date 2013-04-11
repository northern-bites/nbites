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
        float processDeviationPosX;  //Deviation from difference between model & world
        float processDeviationPosY;
        float processDeviationVelX;
        float processDeviationVelY;
        float ballFriction;      //Loss in acceleration over a unit of time
    };

    struct MMKalmanFilterParams
    {
        int numFilters;          // Number of filters (even)
        int framesTillReset;   // Frames we can see a ball and not reset
        float initCovX;
        float initCovY;
        float initCovVelX;
        float initCovVelY;
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



} // namespace balltrack
} // namespace man
