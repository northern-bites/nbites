#include "KalmanFilter.h"

using namespace NBMath;

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
                              messages::Motion     motion)
    {
        updated = true;
    }

    void KalmanFilter::predict(messages::RobotLocation odometry, float deltaT)
    {
        //Calculate A = rotation matrix * trajectory matrix
        //First rotation
        float deltaH = odometry.h();
        float sinDeltaH, cosDeltaH;
        sincosf(deltaH,&sinDeltaH, &cosDeltaH);
        ufmatrix4 rotation = boost::numeric::ublas::identity_matrix <float>(4);
        // nxm matrix is sn rows, m columns
        rotation(0,0) = cosDeltaH;
        rotation(1,0) = -sinDeltaH;
        rotation(0,1) = sinDeltaH;
        rotation(1,1) = cosDeltaH;
        if (stationary){
            rotation(2,2) = 0.f;
            rotation(3,3) = 0.f;
        }
        else { // estimate assumes ball is moving
            rotation(2,2) = cosDeltaH;
            rotation(3,2) = -sinDeltaH;
            rotation(2,3) = sinDeltaH;
            rotation(3,3) = cosDeltaH;
        }

        // Calculate the trajectory
        ufmatrix4 trajectory = boost::numeric::ublas::identity_matrix <float>(4);
        if (!stationary) { //if estimate is moving, predict to where using velocity
            trajectory(0,2) = deltaT;
            trajectory(1,3) = deltaT;
        }

        // Calculate the translation from odometry
        // NOTE: Using notation from Probabilistic Robotics, B = Identity
        //       so no need to compute it or calculate anything with it
        ufvector4 translation = vector4D(odometry.x(), odometry.y(), 0.f, 0.f);

        // Calculate the expected state
        ufmatrix4 A = prod(rotation,trajectory);
        ufmatrix4 ATranspose = trans(A);
        ufvector4 p = prod(A,x);
        x = p + translation;

        // Calculate the covariance Cov = A*Cov*ATranspose

        // Add noise: Process noise, rotation dev, translation dev
    }

    void KalmanFilter::initialize()
    {
        x = vector4D(0.f, 0.f, 1.f, 0.5f);
    }

    ufvector4 KalmanFilter::vector4D(float x, float y, float z, float w)
    {
        ufvector4 p = boost::numeric::ublas::zero_vector <float> (4);
        p(0) = x;
        p(1) = y;
        p(2) = z;
        p(3) = w;
        return p;
    }




} // namespace balltrack
} // namespace man
