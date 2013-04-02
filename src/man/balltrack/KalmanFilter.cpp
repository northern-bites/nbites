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
        std::cout <<"A*cov*A^t:\t";
        for(int i=0; i<4; i++)
            std::cout<<cov(i,i)<<"\t";
        std::cout<<"\n";

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
        // We've already made the matrix using the deviation matrix
        // so lets pump it through
        ufvector4 noiseFromRot = prod(rotationDeviationRotation, x);
        for (int i=0; i<4; i++)
            noise(i) = noiseFromRot(i);

        // Add all this noise to the covariance
        std::cout << "COV:\t";
        for(int i=0; i<4; i++){
            cov(i,i) += noise(i);
            std::cout << cov(i,i) << "\t";
        }
        std::cout<<"\n";

    }

    void KalmanFilter::updateWithObservation(messages::VisionBall visionBall)
    {
        // Declare C and C transpose (ublas)
        // C takes state estimate to observation frame so
        // c = 1  0  0  0
        //     0  1  0  0
        ufmatrix c (2,4);
        for(unsigned i=0; i<c.size1(); i++){
            for(unsigned j=0; j<c.size2(); j++){
                if(i == j)
                    c(i,j) = 1.f;
                else
                    c(i,j) = 0.f;
            }
        }
        ufmatrix cTranspose(4,2);
        cTranspose = trans(c);

        // Calculate the gain
        // Calc c*cov*c^t
        ufmatrix cCovCTranspose(2,2);
        cCovCTranspose = prod(cov,cTranspose);
        cCovCTranspose = prod(c,cCovCTranspose);

        // Add the sensor variance
        cCovCTranspose(0,0) = visionBall.rel_x_variance();
        cCovCTranspose(1,1) = visionBall.rel_y_variance();

        // gain = cov*cTrans*(c*cov*c^t + var)^-1
        ufmatrix kalmanGain(2,2);
        kalmanGain = prod(cTranspose,NBMath::invert2by2(cCovCTranspose));
        kalmanGain = prod(cov,kalmanGain);

        ufvector posEstimates(2);
        posEstimates = prod(c, x);

        // x straight ahead, y to the right
        float sinB,cosB;
        sincosf(visionBall.bearing(),&sinB,&cosB);

        ufvector measurement(2);
        measurement(0) = visionBall.distance()*cosB;
        measurement(1) = visionBall.distance()*sinB;

        ufvector innovation(2);
        innovation = measurement - posEstimates;
        ufvector correction(4);
        correction = prod(kalmanGain,innovation);

        x += correction;
        //cov = cov - k*c*cov
        ufmatrix4 identity;
        identity = boost::numeric::ublas::identity_matrix <float>(4);
        ufmatrix4 modifyCov;
        modifyCov = identity - prod(kalmanGain,c);
        cov = prod(modifyCov,cov);
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
