#include "KalmanFilter.h"

namespace man
{
namespace balltrack
{
    KalmanFilter::KalmanFilter()
    {
        updated = false;
    }

    KalmanFilter::~KalmanFilter(){}

    void KalmanFilter::update(messages::VisionBall visionBall,
                              messages::Motion     odometry)
    {
        updated = true;
    }




} // namespace balltrack
} // namespace man
