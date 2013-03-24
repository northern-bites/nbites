namespace man
{
namespace balltrack
{
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
