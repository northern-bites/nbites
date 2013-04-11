#include "KalmanFilter.h"

// We're doing lots of NBMath so make life easier...
using namespace NBMath;

namespace man
{
namespace balltrack
{
    KalmanFilter::KalmanFilter(bool stationary_,
                               KalmanFilterParams params_)
        :  params(params_),
           updated(false),
           lastUpdateTime(0),
           deltaTime(0.f)
    {
        stationary = stationary_;

    }

    KalmanFilter::~KalmanFilter(){}

    void KalmanFilter::updateDeltaTime()
    {
        // Get time since last update
        const long long int time = monotonic_micro_time(); //from common
        deltaTime = static_cast<float>(time - lastUpdateTime)/
            1000000.0f; // u_s to sec

        // Guard against a zero dt (maybe possible?)
        if (deltaTime == 0.0){
            deltaTime = 0.0001f;
        }
        lastUpdateTime = time;
    }

    void KalmanFilter::update(messages::VisionBall visionBall,
                              messages::Motion     motion)
    {
        updated = true;

        //Get passed time
        updateDeltaTime();

        predict(motion.odometry());
        updateWithObservation(visionBall);

        // I think?
        filteredDist = std::sqrt(x(0)*x(0) + x(1)*x(1));
        filteredBear = NBMath::safe_atan2(x(1),x(0));
    }

    void KalmanFilter::predict(messages::RobotLocation odometry)
    {
        // Overload for offline data simulation
        predict(odometry, deltaTime);
    }

    void KalmanFilter::predict(messages::RobotLocation odometry, float deltaT)
    {
        // std::cout<<"Ball X Est\t" << x(0)
        //          <<"\nBall Y Est\t" << x(1) << std::endl;
        // std::cout<<"Cov X\t" << cov(0,0)
        //          <<"\nCov Y\t" << cov(1,1) << std::endl;

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
        // Note: our heading change is positive going clockwise, so the rotation
        //       matrix is confusing...

        ufmatrix4 rotation = boost::numeric::ublas::identity_matrix <float>(4);
        ufmatrix4 rotationDeviationRotation = boost::numeric::ublas::identity_matrix <float>(4);

        // nxm matrix is n rows, m columns
        rotation(0,0) = cosDeltaH;
        rotation(1,0) = sinDeltaH;
        rotation(0,1) = -sinDeltaH;
        rotation(1,1) = cosDeltaH;

        rotationDeviationRotation(0,0) = cosRotDev;
        rotationDeviationRotation(1,0) = sinRotDev;
        rotationDeviationRotation(0,1) = -sinRotDev;
        rotationDeviationRotation(1,1) = cosRotDev;

        if (stationary){
            rotation(2,2) = 0.f;
            rotation(3,3) = 0.f;

            rotationDeviationRotation(2,2) = 0.f;
            rotationDeviationRotation(3,3) = 0.f;
        }
        else { // estimate assumes ball is moving
            rotation(2,2) = cosDeltaH;
            rotation(3,2) = sinDeltaH;
            rotation(2,3) = -sinDeltaH;
            rotation(3,3) = cosDeltaH;

            rotationDeviationRotation(2,3) = cosRotDev;
            rotationDeviationRotation(3,2) = sinRotDev;
            rotationDeviationRotation(2,3) = -sinRotDev;
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
        ufvector4 translation = vector4D(-odometry.x(), -odometry.y(), 0.f, 0.f);
        // And deviation
        float xTransDev = odometry.x() * params.transXDeviation;
        xTransDev *= xTransDev;

        float yTransDev = odometry.y() * params.transYDeviation;
        yTransDev *= yTransDev;

        ufvector4 translationDeviation = vector4D(xTransDev,
                                                  yTransDev,
                                                  0.f, 0.f);

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

        if(!stationary){
            noise(2) += params.processDeviationVelX;
            noise(3) += params.processDeviationVelY;
        }

        // Add translation deviation noise
        noise += translationDeviation;

        // Add rotation deviation noise
        // We've already made the matrix using the deviation matrix
        // so lets pump it through
        // #CRUDE_APPROXIMATION @b_human
        ufvector4 noiseFromRot = prod(rotationDeviationRotation, x) - x;

        for (int i=0; i<4; i++)
            noise(i) += std::abs(noiseFromRot(i));

        // Add all this noise to the covariance
         for(int i=0; i<4; i++){
             cov(i,i) += noise(i);
         }

        // std::cout<<"Ball X Est\t" << x(0)
        //          <<"\nBall Y Est\t" << x(1) << std::endl;
        // std::cout<<"Cov X\t" << cov(0,0)
        //          <<"\nCov Y\t" << cov(1,1) << std::endl;
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
        cCovCTranspose(0,0) += visionBall.rel_x_variance();
        cCovCTranspose(1,1) += visionBall.rel_y_variance();

        // gain = cov*c^t*(c*cov*c^t + var)^-1
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

        // std::cout << "See a ball at x:\t" << measurement(0)
        //           <<"\nand at        y:\t" << measurement(1) << std::endl;

        ufvector innovation(2);
        innovation = measurement - posEstimates;

        // std::cout << "Innovation\t" << innovation(0) << " , " << innovation(1) << std::endl;

        ufvector correction(4);
        correction = prod(kalmanGain,innovation);
        // std::cout << "Correction\t" << correction(0) << " , " << correction(1) << std::endl;

        // Lets try using the size of the correction to determine how well the filter has been modeling
        weight = 1 / (std::sqrt(correction(0)*correction(0)
                                + correction(1)*correction(1)));

        x += correction;


        //cov = cov - k*c*cov
        ufmatrix4 identity;
        identity = boost::numeric::ublas::identity_matrix <float>(4);
        ufmatrix4 modifyCov;
        modifyCov = identity - prod(kalmanGain,c);
        cov = prod(modifyCov,cov);

        // std::cout<<"Ball X Est\t" << x(0)
        //          <<"\nBall Y Est\t" << x(1) << std::endl;
        // std::cout<<"Cov X\t" << cov(0,0)
        //          <<"\nCov Y\t" << cov(1,1) << std::endl;
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
