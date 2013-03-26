namespace man
{
namespace balltrack
{

    struct KalmanFilterParams
    {
        float transXDeviation;   //Deviation from odometry in x direction
        float transYDeviation;   //Deviation from odometry in y direction
        float rotationDeviation; //Deviation from odometry in rotation
        float processDevitaion;  //Deviation from difference between model & world
        float ballFriction;      //Loss in acceleration over a unit of time

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
