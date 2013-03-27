#include "KalmanFilter.h"

// We're doing lots of NBMath so make life easier...
using namespace NBMath;

namespace man
{
namespace balltrack
{
    KalmanFilter::KalmanFilter(KalmanFilterParams params_)
    {
        updated = false;
        params = params_;
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
        //First calc rotation matrix
        float deltaH = odometry.h();
        float sinDeltaH, cosDeltaH;
        sincosf(deltaH,&sinDeltaH, &cosDeltaH);

        // Keep track of the deviation
        float rotationDeviation = odometry.h() * params.rotationDeviation;
        float sinRotDev, cosRotDev;
        sincosf(rotationDeviation, &sinRotDev, &cosRotDev);

        // Generate the rotation and rotationDeviationRotation matrices
        ufmatrix4 rotation = boost::numeric::ublas::identity_matrix <float>(4);
        ufmatrix4 rotationDeviationRotation = boost::numeric::ublas::identity_matrix <float>(4);

        // nxm matrix is n rows, m columns
        rotation(0,0) = cosDeltaH;
        rotation(1,0) = -sinDeltaH;
        rotation(0,1) = sinDeltaH;
        rotation(1,1) = cosDeltaH;

        rotationDeviationRotation(0,0) = cosRotDev;
        rotationDeviationRotation(1,0) = -sinRotDev;
        rotationDeviationRotation(0,1) = sinRotDev;
        rotationDeviationRotation(1,1) = cosRotDev;

        if (stationary){
            rotation(2,2) = 0.f;
            rotation(3,3) = 0.f;

            rotationDeviationRotation(2,2) = 0.f;
            rotationDeviationRotation(3,3) = 0.f;
        }
        else { // estimate assumes ball is moving
            rotation(2,2) = cosDeltaH;
            rotation(3,2) = -sinDeltaH;
            rotation(2,3) = sinDeltaH;
            rotation(3,3) = cosDeltaH;

            rotationDeviationRotation(2,3) = cosRotDev;
            rotationDeviationRotation(3,2) = -sinRotDev;
            rotationDeviationRotation(2,3) = sinRotDev;
            rotationDeviationRotation(3,3) = cosRotDev;
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
        // And deviation
        ufvector4 translationDeviation = vector4D(odometry.x() * params.transXDeviation,
                                                  odometry.y() * params.transYDeviation, 0.f, 0.f);


        // TODO - INCORPORATE FRICTION


        // Calculate the expected state
        ufmatrix4 A = prod(rotation,trajectory);
        ufmatrix4 ATranspose = trans(A);
        ufvector4 p = prod(A,x);
        x = p + translation;

        // Calculate the covariance Cov = A*Cov*ATranspose
        ufmatrix4 covTimesATranspose = prod(cov, ATranspose);
        cov = prod(A, covTimesATranspose);

        // Add noise: Process noise, rotation dev, translation dev
        ufvector4 noise;
        noise = boost::numeric::ublas::zero_vector<float>(4);

       // Add process noise
        noise(0) += params.processDeviationPosX;
        noise(1) += params.processDeviationPosY;
        noise(2) += params.processDeviationVelX;
        noise(3) += params.processDeviationVelY;

        // Add translation deviation noise
        noise(0) += odometry.x() * params.transXDeviation;
        noise(1) += odometry.y() * params.transYDeviation;

        // Add rotation deviation noise
        // This is a little tricky, the rotation deviation is 1 dimensional
        // and in degrees... lets assume rotating affects x and y equally
        // and further assume the params are smart and calculated the rotation
        // deviation out of degrees! Yay minimizing real time computation
        for (int i=0; i<4; i++)
            noise(i) = odometry.h() * params.rotationDeviation;

        // Add all this noise to the covariance
        for(int i=0; i<4; i++)
            cov(i,i) = noise(i);

    }

    void KalmanFilter::initialize()
    {
        x = vector4D(0.f, 0.f, 1.f, 0.5f);
        cov = boost::numeric::ublas::identity_matrix <float>(4);
    }

    void KalmanFilter::initialize(ufvector4 x_,
                                  ufmatrix4 cov_)
    {
        // references, not pointers so will copy values
        x = x_;
        cov = cov_;
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
