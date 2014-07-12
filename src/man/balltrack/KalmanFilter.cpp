#include "KalmanFilter.h"

// We're doing lots of NBMath so make life easier...
using namespace NBMath;

namespace man {
namespace balltrack {

KalmanFilter::KalmanFilter(bool stationary_,
                           KalmanFilterParams params_)
    :  params(params_),
       updated(false),
       lastUpdateTime(0),
       deltaTime(0.03f),
       relXDest(0.f),
       relYDest(0.f)
{
    score = 0.f;
    curEntry = 0.f;
    correctionMagBuffer = new float[BUFF_LENGTH];
    stationary = stationary_;
    initialize();
}

KalmanFilter::~KalmanFilter() {
    delete correctionMagBuffer;
}

void KalmanFilter::updateDeltaTime()
{
    // Get time since last update
    const long long int time = monotonic_micro_time(); //from common
    deltaTime = static_cast<float>(time - lastUpdateTime)/1000000.0f; // u_s to sec

    // Guard against a zero dt (maybe possible?)
    if (deltaTime <= 0.0){
        deltaTime = 0.0001f;
    }

    if (deltaTime > 1)
        deltaTime = .03f; // Guard against first frame issues

    lastUpdateTime = time;
}

void KalmanFilter::update(messages::VisionBall visionBall,
                          messages::RobotLocation  motion)
{
    updated = true;

    //Get passed time
    updateDeltaTime();

    predict(motion);
    //Note: Only update if we have an observation...
    if(visionBall.on())
        updateWithObservation(visionBall);
}

void KalmanFilter::predict(messages::RobotLocation odometry)
{
    // Overload for offline data simulation
    predict(odometry, deltaTime);
}

void KalmanFilter::predict(messages::RobotLocation odometry, float deltaT)
{
    float diffX = odometry.x() - lastOdometry.x();
    lastOdometry.set_x(odometry.x());
    float diffY = odometry.y() - lastOdometry.y();
    lastOdometry.set_y(odometry.y());
    float diffH = odometry.h() - lastOdometry.h();
    lastOdometry.set_h(odometry.h());

    float sinh, cosh;
    sincosf(odometry.h(), &sinh, &cosh);

    // change into the robot frame
    float dX = cosh*diffX + sinh*diffY;
    float dY = cosh*diffY - sinh*diffX;
    float dH = diffH;// * 2.4f;

    if( (std::fabs(dX) > 3.f) || (std::fabs(dY) > 3.f) ) {
        //Probably reset odometry somewhere so skip a frame
        dX = 0.f;
        dY = 0.f;
        dH = 0.f;
    }

    //Calculate A = rotation matrix * trajectory matrix
    //First calc rotation matrix
    float sinDeltaH, cosDeltaH;
    sincosf(dH,&sinDeltaH, &cosDeltaH);

    // Keep track of the deviation
    float rotationDeviation = dH * params.rotationDeviation;
    float sinRotDev, cosRotDev;
    sincosf(rotationDeviation, &sinRotDev, &cosRotDev);

    // Generate the rotation and rotationDeviationRotation matrices
    // We rotate counterclockwise for positive dH
    // So everything rotates clockwise around us
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
    // If we go left (positive y, everything else shifts down
    // NOTE: Using notation from Probabilistic Robotics, B = Identity
    //       so no need to compute it or calculate anything with it
    ufvector4 translation = NBMath::vector4D(-dX, -dY, 0.f, 0.f);
    // And deviation
    float xTransDev = dX * params.transXDeviation;
    xTransDev *= xTransDev;

    float yTransDev = dY * params.transYDeviation;
    yTransDev *= yTransDev;

    ufvector4 translationDeviation = NBMath::vector4D(xTransDev,
                                                      yTransDev,
                                                      0.f, 0.f);

    // Incorporate Friction
    // Have params.ballFriction in cm/sec^2
    // ballFriction * deltaT = impact from friction that frame in cm/sec (velocity)
    // in each direction, so need to add that impact if velocity is positive,
    //                       need to subtract that impact if velocity is negative

    // Incorporate friction if the ball is moving

    if (!stationary) // moving
    {
        float xVelFactor = (std::abs(x(2)) + params.ballFriction*deltaT)/std::abs(x(2));
        float yVelFactor = (std::abs(x(3)) + params.ballFriction*deltaT)/std::abs(x(3));

        if( xVelFactor < 0.001f)
            x(2) = .0001f;
        if( yVelFactor < 0.001f)
            x(3) = .0001f;

        // Determine if ball is still moving
        float velMagnitude = getSpeed();

        if(velMagnitude > 2.f) // basically still moving
        {
            // vel = vel * (absVel + decel)/absVel
            x(2) *= xVelFactor;
            x(3) *= yVelFactor;
        }
    }

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

    // Housekeep
    filteredDist = std::sqrt(x(0)*x(0) + x(1)*x(1));
    filteredBear = NBMath::safe_atan2(x(1),x(0));
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

    cCovCTranspose(0,0) += params.obsvRelXVariance;
    cCovCTranspose(1,1) += params.obsvRelYVariance;

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

    // Housekeep
    filteredDist = std::sqrt(x(0)*x(0) + x(1)*x(1));
    filteredBear = NBMath::safe_atan2(x(1),x(0));

    float curErr = std::sqrt(correction(0)*correction(0) + correction(1)*correction(1));
    correctionMagBuffer[curEntry] = curErr;

    score = 0.f;
    for(int i=0; i<BUFF_LENGTH; i++)
        score+= correctionMagBuffer[i];
    score = score/10; // avg correction over 10 frames

    //get magnitude of correction
    // std::cout << "Is moving filter " << isStationary() << std::endl;
    // std::cout << "Score: " << score << "  cur error " << curErr << std::endl;
}

void KalmanFilter::initialize()
{
    x = NBMath::vector4D(10.0f, 0.0f, 0.f, 0.f);
    cov = boost::numeric::ublas::identity_matrix <float>(4);
}

void KalmanFilter::initialize(ufvector4 x_,
                              ufmatrix4 cov_)
{
    // references, not pointers so will copy values
    x = x_;
    cov = cov_;
}

void KalmanFilter::predictBallDest()
{
    if(stationary)
    {
        relXDest = x(0);
        relYDest = x(1);
    }
    else // moving
    {
        float speed = getSpeed();

        //Calculate time until stop
        float timeToStop = std::abs(speed / params.ballFriction);

        //Calculate deceleration in each direction
        float decelX = (x(2)/speed) * params.ballFriction;
        float decelY = (x(3)/speed) * params.ballFriction;

        // Calculate end position
        relXDest = x(0) + x(2)*timeToStop + .5f*decelX*timeToStop*timeToStop;
        relYDest = x(1) + x(3)*timeToStop + .5f*decelY*timeToStop*timeToStop;

        //Calculate the time until intersects with robots y axis
        float timeToIntersect = NBMath::getLargestMagRoot(x(0),x(2),.5f*decelX);
        // Use quadratic :(
        relYIntersectDest = x(1) + x(3)*timeToStop + .5f*decelY*timeToStop*timeToStop;
    }

}

} // namespace balltrack
} // namespace man
