#include "BallEKF.h"
using namespace boost::numeric;
using namespace boost;

using namespace NBMath;

// Parameters
const float BallEKF::ASSUMED_FPS = 15.0;
// How much uncertainty naturally grows per update
const float BallEKF::BETA_BALL = 5.0f;
// How much ball velocity should effect uncertainty
const float BallEKF::GAMMA_BALL = 0.4f;
const float BallEKF::CARPET_FRICTION = -25.0f; // 25 cm/s^2
const float BallEKF::BALL_DECAY_PERCENT = 0.25f;

// Default initialization values
const float BallEKF::INIT_BALL_X = 370.0f;
const float BallEKF::INIT_BALL_Y = 270.0f;
const float BallEKF::INIT_BALL_X_VEL = 0.0f;
const float BallEKF::INIT_BALL_Y_VEL = 0.0f;
const float BallEKF::X_UNCERT_MAX = 740.0f;
const float BallEKF::Y_UNCERT_MAX = 270.0f;
const float BallEKF::VELOCITY_UNCERT_MAX = 300.0;
const float BallEKF::X_UNCERT_MIN = 1.0e-6;
const float BallEKF::Y_UNCERT_MIN = 1.0e-6;
const float BallEKF::VELOCITY_UNCERT_MIN = 1.0e-6;
const float BallEKF::INIT_X_UNCERT = 740.0f;
const float BallEKF::INIT_Y_UNCERT = 270.0f;
const float BallEKF::INIT_X_VEL_UNCERT = 300.0f;
const float BallEKF::INIT_Y_VEL_UNCERT = 300.0f;
const float BallEKF::X_EST_MIN = -1000.0f;
const float BallEKF::Y_EST_MIN = -600.0f;
const float BallEKF::X_EST_MAX = 1000.0f;
const float BallEKF::Y_EST_MAX = 600.0f;
const float BallEKF::VELOCITY_EST_MAX = 300.0f;
const float BallEKF::VELOCITY_EST_MIN = -300.0f;

BallEKF::BallEKF()
    : EKF<RangeBearingMeasurement, MotionModel, BALL_EKF_DIMENSION,
          BALL_MEASUREMENT_DIMENSION>(BETA_BALL,GAMMA_BALL),
      useCartesian(true)
{
    // ones on the diagonal
    A_k(0,0) = 1.0;
    A_k(1,1) = 1.0;
    A_k(2,2) = 1.0;
    A_k(3,3) = 1.0;

    // Assummed change in position necessary for velocity to work correctly
    A_k(0,2) = 1.0 / ASSUMED_FPS;
    A_k(1,3) = 1.0 / ASSUMED_FPS;

    // Setup initial values
    setXEst(INIT_BALL_X);
    setYEst(INIT_BALL_Y);
    setXVelocityEst(INIT_BALL_X_VEL);
    setYVelocityEst(INIT_BALL_Y_VEL);
    setXUncert(INIT_X_UNCERT);
    setYUncert(INIT_Y_UNCERT);
    setXVelocityUncert(INIT_X_VEL_UNCERT);
    setYVelocityUncert(INIT_Y_VEL_UNCERT);
}


/**
 * Constructor for the BallEKF class
 *
 * @param _mcl The Monte Carlo localization sytem for the robot
 * @param _initX An initial value for the x estimate
 * @param _initY An initial value for the y estimate
 * @param _initXUncert An initial value for the x uncertainty
 * @param _initYUncert An initial value for the y uncertainty
 * @param _initVelX An initial value for the x velocity estimate
 * @param _initVelY An initial value for the y velocity estimate
 * @param _initVelXUncert An initial value for the x velocity uncertainty
 * @param _initVelYUncert An initial value for the y velocity uncertainty
 */
BallEKF::BallEKF(float initX, float initY,
                 float initVelX, float initVelY,
                 float initXUncert,float initYUncert,
                 float initVelXUncert, float initVelYUncert)
    : EKF<RangeBearingMeasurement, MotionModel, BALL_EKF_DIMENSION,
          BALL_MEASUREMENT_DIMENSION>(BETA_BALL,GAMMA_BALL),
      useCartesian(true)
{
    // ones on the diagonal
    A_k(0,0) = 1.0;
    A_k(1,1) = 1.0;
    A_k(2,2) = 1.0;
    A_k(3,3) = 1.0;

    // Assummed change in position necessary for velocity to work correctly
    A_k(0,2) = 1.0 / ASSUMED_FPS;
    A_k(1,3) = 1.0 / ASSUMED_FPS;

    // Setup initial values
    setXEst(initX);
    setYEst(initY);
    setXVelocityEst(initVelX);
    setYVelocityEst(initVelY);
    setXUncert(initXUncert);
    setYUncert(initYUncert);
    setXVelocityUncert(initVelXUncert);
    setYVelocityUncert(initVelYUncert);
}

/**
 * Method to deal with updating the entire ball model
 *
 * @param ball the ball seen this frame.
 */
void BallEKF::updateModel(VisualBall * ball, PoseEst p, bool _useCartesian)
{
    robotPose = p;
    useCartesian = _useCartesian;
    // Update expected ball movement
    timeUpdate(MotionModel());
    limitAPrioriEst();
    limitAPrioriUncert();

    // We've seen a ball
    if (ball->getDistance() > 0.0) {
        sawBall(ball);

        // } else if (TEAMMATE BALL REPORT) { // A teammate has seen a ball
    } else { // No ball seen

        setXVelocityEst(getXVelocityEst() * (1.0f - BALL_DECAY_PERCENT));
        setYVelocityEst(getYVelocityEst() * (1.0f - BALL_DECAY_PERCENT));

        noCorrectionStep();
    }
    limitPosteriorEst();
    limitPosteriorUncert();
}

/**
 * Method to deal with sighting of a ball by the robot
 * @param ball a copy of the ball seen by the robot
 */
void BallEKF::sawBall(VisualBall * ball)
{
    RangeBearingMeasurement m;
    std::vector<RangeBearingMeasurement> z;

    m.distance = ball->getDistance();
    m.bearing = ball->getBearing();
    m.distanceSD = ball->getDistanceSD();
    m.bearingSD = ball->getBearingSD();

    z.push_back(m);
    correctionStep(z);
}


/**
 * Method incorporate the expected change in ball position from the last
 * frame.  Updates the values of the covariance matrix Q_k and the jacobian
 * A_k.
 *
 * @param u The motion model of the last frame.  Ignored for the ball.
 * @return The expected change in ball position (x,y, xVelocity, yVelocity)
 */
EKF<RangeBearingMeasurement, MotionModel, BALL_EKF_DIMENSION,
    BALL_MEASUREMENT_DIMENSION>::StateVector BallEKF::associateTimeUpdate(
        MotionModel u)
{
    // Calculate the assumed change in ball position
    // Assume no decrease in ball velocity
    StateVector deltaBall(BALL_EKF_DIMENSION);
    deltaBall(0) = getXVelocityEst() * (1.0f / ASSUMED_FPS);
    deltaBall(1) = getYVelocityEst() * (1.0f / ASSUMED_FPS);
    deltaBall(2) = sign(getXVelocityEst()) * (CARPET_FRICTION / ASSUMED_FPS);
    deltaBall(3) = sign(getYVelocityEst()) * (CARPET_FRICTION / ASSUMED_FPS);

    return deltaBall;
}

/**
 * Method to deal with incorporating a ball measurement into the EKF
 *
 * @param z the measurement to be incorporated
 * @param H_k the jacobian associated with the measurement, to be filled out
 * @param R_k the covariance matrix of the measurement, to be filled out
 * @param V_k the measurement invariance
 *
 * @return the measurement invariance
 */
void BallEKF::incorporateMeasurement(RangeBearingMeasurement z,
                                     StateMeasurementMatrix &H_k,
                                     MeasurementMatrix &R_k,
                                     MeasurementVector &V_k)
{
    // Convert our sighting to cartesian coordinates
    const float x_b_r = z.distance * cos(z.bearing);
    const float y_b_r = z.distance * sin(z.bearing);
    MeasurementVector z_x(2);

    z_x(0) = x_b_r;
    z_x(1) = y_b_r;

    // Get expected values of ball
    const float x_b = getXEst();
    const float y_b = getYEst();
    float sinh, cosh;
    sincosf(robotPose.h, &sinh, &cosh);

    MeasurementVector d_x(2);

    d_x(0) = (x_b - robotPose.x) * cosh + (y_b - robotPose.y) * sinh;
    d_x(1) = -(x_b - robotPose.x) * sinh + (y_b - robotPose.y) * cosh;

    // Calculate invariance
    V_k = z_x - d_x;

    // Calculate jacobians
    H_k(0,0) = cosh;
    H_k(0,1) = sinh;
    H_k(1,0) = -sinh;
    H_k(1,1) = cosh;

    // Update the measurement covariance matrix
    R_k(0,0) = z.distanceSD;
    R_k(1,1) = z.distanceSD;

}

/**
 * Method to ensure that the ball estimate does have any unrealistic values
 */
void BallEKF::limitAPrioriEst()
{
    if(xhat_k_bar(0) > X_EST_MAX) {
        xhat_k_bar(0) = X_EST_MAX;
    }
    if(xhat_k_bar(0) < X_EST_MIN) {
        xhat_k_bar(0) = X_EST_MIN;
    }
    if(xhat_k_bar(1) > Y_EST_MAX) {
        xhat_k_bar(1) = Y_EST_MAX;
    }
    if(xhat_k_bar(1) < Y_EST_MIN) {
        xhat_k_bar(1) = Y_EST_MIN;
    }
    if(xhat_k_bar(2) > VELOCITY_EST_MAX) {
        xhat_k_bar(2) = VELOCITY_EST_MAX;
    }
    if(xhat_k_bar(2) < VELOCITY_EST_MIN) {
        xhat_k_bar(2) = VELOCITY_EST_MIN;
    }
    if(xhat_k_bar(3) > VELOCITY_EST_MAX) {
        xhat_k_bar(3) = VELOCITY_EST_MAX;
    }
    if(xhat_k_bar(3) < VELOCITY_EST_MIN) {
        xhat_k_bar(3) = VELOCITY_EST_MIN;
    }
}

/**
 * Method to ensure that the ball estimate does have any unrealistic values
 */
void BallEKF::limitPosteriorEst()
{
    if(xhat_k(0) > X_EST_MAX) {
        xhat_k_bar(0) = X_EST_MAX;
        xhat_k(0) = X_EST_MAX;
    }
    if(xhat_k(0) < X_EST_MIN) {
        xhat_k_bar(0) = X_EST_MIN;
        xhat_k(0) = X_EST_MIN;
    }
    if(xhat_k(1) > Y_EST_MAX) {
        xhat_k_bar(1) = Y_EST_MAX;
        xhat_k(1) = Y_EST_MAX;
    }
    if(xhat_k(1) < Y_EST_MIN) {
        xhat_k_bar(1) = Y_EST_MIN;
        xhat_k(1) = Y_EST_MIN;
    }
    if(xhat_k(2) > VELOCITY_EST_MAX) {
        xhat_k_bar(2) = VELOCITY_EST_MAX;
        xhat_k(2) = VELOCITY_EST_MAX;
    }
    if(xhat_k(2) < VELOCITY_EST_MIN) {
        xhat_k_bar(2) = VELOCITY_EST_MIN;
        xhat_k(2) = VELOCITY_EST_MIN;
    }
    if(xhat_k(3) > VELOCITY_EST_MAX) {
        xhat_k_bar(3) = VELOCITY_EST_MAX;
        xhat_k(3) = VELOCITY_EST_MAX;
    }
    if(xhat_k(3) < VELOCITY_EST_MIN) {
        xhat_k_bar(3) = VELOCITY_EST_MIN;
        xhat_k(3) = VELOCITY_EST_MIN;
    }
}

/**
 * Method to ensure that uncertainty does not grow without bound
 */
void BallEKF::limitAPrioriUncert()
{

    // Check x uncertainty
    if(P_k_bar(0,0) > X_UNCERT_MAX) {
        P_k_bar(0,0) = X_UNCERT_MAX;
    }
    // Check y uncertainty
    if(P_k_bar(1,1) > Y_UNCERT_MAX) {
        P_k_bar(1,1) = Y_UNCERT_MAX;
    }
    // Check x veolcity uncertainty
    if(P_k_bar(2,2) > VELOCITY_UNCERT_MAX) {
        P_k_bar(2,2) = VELOCITY_UNCERT_MAX;
    }
    // Check y veolcity uncertainty
    if(P_k_bar(3,3) > VELOCITY_UNCERT_MAX) {
        P_k_bar(3,3) = VELOCITY_UNCERT_MAX;
    }
    // Check x uncertainty
    if(P_k_bar(0,0) < X_UNCERT_MIN) {
        P_k_bar(0,0) = X_UNCERT_MIN;
    }
    // Check y uncertainty
    if(P_k_bar(1,1) < Y_UNCERT_MIN) {
        P_k_bar(1,1) = Y_UNCERT_MIN;
    }
    // Check x veolcity uncertainty
    if(P_k_bar(2,2) < VELOCITY_UNCERT_MIN) {
        P_k_bar(2,2) = VELOCITY_UNCERT_MIN;
    }
    // Check y veolcity uncertainty
    if(P_k_bar(3,3) < VELOCITY_UNCERT_MIN) {
        P_k_bar(3,3) = VELOCITY_UNCERT_MIN;
    }
}

/**
 * Method to ensure that uncertainty does not grow or shrink without bound
 */
void BallEKF::limitPosteriorUncert()
{
    // Check x uncertainty
    if(P_k(0,0) < X_UNCERT_MIN) {
        P_k(0,0) = X_UNCERT_MIN;
        P_k_bar(0,0) = X_UNCERT_MIN;
    }
    // Check y uncertainty
    if(P_k(1,1) < Y_UNCERT_MIN) {
        P_k(1,1) = Y_UNCERT_MIN;
        P_k_bar(1,1) = Y_UNCERT_MIN;
    }
    // Check x veolcity uncertainty
    if(P_k(2,2) < VELOCITY_UNCERT_MIN) {
        P_k(2,2) = VELOCITY_UNCERT_MIN;
        P_k_bar(2,2) = VELOCITY_UNCERT_MIN;
    }
    // Check y veolcity uncertainty
    if(P_k(3,3) < VELOCITY_UNCERT_MIN) {
        P_k(3,3) = VELOCITY_UNCERT_MIN;
        P_k_bar(3,3) = VELOCITY_UNCERT_MIN;
    }
    // Check x uncertainty
    if(P_k(0,0) > X_UNCERT_MAX) {
        P_k(0,0) = X_UNCERT_MAX;
        P_k_bar(0,0) = X_UNCERT_MAX;
    }
    // Check y uncertainty
    if(P_k(1,1) > Y_UNCERT_MAX) {
        P_k(1,1) = Y_UNCERT_MAX;
        P_k_bar(1,1) = Y_UNCERT_MAX;
    }
    // Check x veolcity uncertainty
    if(P_k(2,2) > VELOCITY_UNCERT_MAX) {
        P_k(2,2) = VELOCITY_UNCERT_MAX;
        P_k_bar(2,2) = VELOCITY_UNCERT_MAX;
    }
    // Check y veolcity uncertainty
    if(P_k(3,3) > VELOCITY_UNCERT_MAX) {
        P_k(3,3) = VELOCITY_UNCERT_MAX;
        P_k_bar(3,3) = VELOCITY_UNCERT_MAX;
    }
}
