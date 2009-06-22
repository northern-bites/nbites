#include "BallEKF.h"
using namespace boost::numeric;
using namespace boost;

using namespace NBMath;
#include "FieldConstants.h"
// Parameters
const float BallEKF::ASSUMED_FPS = 30.0f;
const float BallEKF::USE_CARTESIAN_BALL_DIST = 5000.0f;

// How much uncertainty naturally grows per update
const float BallEKF::BETA_BALL = 5.0f;
// How much ball velocity should effect uncertainty
const float BallEKF::GAMMA_BALL = 0.4f;
const float BallEKF::CARPET_FRICTION = -(1.0f / 4.0f);//25.0f; // 25 cm/s^2
const float BallEKF::BALL_DECAY_PERCENT = 0.25f;

// Default initialization values
const float BallEKF::INIT_BALL_X = 370.0f;
const float BallEKF::INIT_BALL_Y = 270.0f;
const float BallEKF::INIT_BALL_X_VEL = 0.0f;
const float BallEKF::INIT_BALL_Y_VEL = 0.0f;
const float BallEKF::X_UNCERT_MAX = 740.0f;
const float BallEKF::Y_UNCERT_MAX = 270.0f;
const float BallEKF::VELOCITY_UNCERT_MAX = 150.0f;
const float BallEKF::X_UNCERT_MIN = 1.0e-6f;
const float BallEKF::Y_UNCERT_MIN = 1.0e-6f;
const float BallEKF::VELOCITY_UNCERT_MIN = 1.0e-6f;
const float BallEKF::INIT_X_UNCERT = 740.0f;
const float BallEKF::INIT_Y_UNCERT = 270.0f;
const float BallEKF::INIT_X_VEL_UNCERT = 150.0f;
const float BallEKF::INIT_Y_VEL_UNCERT = 150.0f;
const float BallEKF::X_EST_MIN = 0.0f;
const float BallEKF::Y_EST_MIN = 0.0f;
const float BallEKF::X_EST_MAX = FIELD_WIDTH;
const float BallEKF::Y_EST_MAX = FIELD_HEIGHT;
const float BallEKF::VELOCITY_EST_MAX = 150.0f;
const float BallEKF::VELOCITY_EST_MIN = -150.0f;

BallEKF::BallEKF()
    : EKF<RangeBearingMeasurement, MotionModel, BALL_EKF_DIMENSION,
          BALL_MEASUREMENT_DIMENSION>(BETA_BALL,GAMMA_BALL)
{
    // ones on the diagonal
    A_k(0,0) = 1.0;
    A_k(1,1) = 1.0;
    A_k(2,2) = 1.0;
    A_k(3,3) = 1.0;

    // Assummed change in position necessary for velocity to work correctly
    A_k(0,2) = 1.0f / ASSUMED_FPS;
    A_k(1,3) = 1.0f / ASSUMED_FPS;

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
          BALL_MEASUREMENT_DIMENSION>(BETA_BALL,GAMMA_BALL)
{
    // ones on the diagonal
    A_k(0,0) = 1.0;
    A_k(1,1) = 1.0;
    A_k(2,2) = 1.0;
    A_k(3,3) = 1.0;

    // Assummed change in position necessary for velocity to work correctly
    A_k(0,2) = 1.0f / ASSUMED_FPS;
    A_k(1,3) = 1.0f / ASSUMED_FPS;

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


void BallEKF::reset()
{
    // Reset all of the matrices to zeros
    for(unsigned i = 0; i < numStates; ++i) {
        for(unsigned j = 0; j < numStates; ++j) {
            Q_k(i,j) = 0.0f;
            P_k(i,j) = 0.0f;
            P_k_bar(i,j) = 0.0f;
        }
        // xhat_k(i) = 0.0f;
        // xhat_k_bar(i) = 0.0f;
    }
    // Set the initial values
    setXEst(INIT_BALL_X);
    setYEst(INIT_BALL_Y);
    setXVelocityEst(INIT_BALL_X_VEL);
    setYVelocityEst(INIT_BALL_Y_VEL);
    setXUncert(INIT_X_UNCERT);
    setYUncert(INIT_Y_UNCERT);
    setXVelocityUncert(INIT_X_VEL_UNCERT);
    setYVelocityUncert(INIT_X_VEL_UNCERT);
}

/**
 * Method to deal with updating the entire ball model
 *
 * @param ball the ball seen this frame.
 */
void BallEKF::updateModel(RangeBearingMeasurement  ball, PoseEst p)
{
    robotPose = p;
    // Update expected ball movement
    timeUpdate(MotionModel());
    limitAPrioriUncert();

    // We've seen a ball
    if (ball.distance > 0.0) {
        std::vector<RangeBearingMeasurement> z;
        z.push_back(ball);
        correctionStep(z);

    } else { // No ball seen
        noCorrectionStep();
        // setXVelocityEst(getXVelocityEst() * (1.0f - BALL_DECAY_PERCENT));
        // setYVelocityEst(getYVelocityEst() * (1.0f - BALL_DECAY_PERCENT));
    }
    limitPosteriorUncert();
    //clipBallEstimate();
    limitPosteriorEst();
    testForNaNReset();
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
    StateVector deltaBall(BALL_EKF_DIMENSION);

    float dt = 1.0f / ASSUMED_FPS;
    // Calculate the assumed change in ball position
    deltaBall(0) = getXVelocityEst() * dt;
    deltaBall(1) = getYVelocityEst() * dt;
    // Decrease in velocity with respect to velocity and friction
    deltaBall(2) = CARPET_FRICTION * getXVelocityEst() *dt;
    deltaBall(3) = CARPET_FRICTION * getYVelocityEst() *dt;

    A_k(0,2) = dt;
    A_k(1,3) = dt;
    // I believe these are the derivatives with respect to x and y velocities
    // If you turn them on, then velocities are always 0 - Tucker
    // A_k(2,2) = CARPET_FRICTION * sign(getXVelocityEst()) * dt;
    // A_k(3,3) = CARPET_FRICTION * sign(getYVelocityEst()) *dt;

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
    if (z.distance < USE_CARTESIAN_BALL_DIST) {
        // Convert our sighting to cartesian coordinates
        const float x_b_r = z.distance * std::cos(z.bearing);
        const float y_b_r = z.distance * std::sin(z.bearing);
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
    } else {
        // Convert our sighting to cartesian coordinates
        MeasurementVector z_x(2);
        z_x(0) = z.distance;
        z_x(1) = z.bearing;

        // Get expected values of ball
        const float x_b = getXEst();
        const float y_b = getYEst();

        MeasurementVector d_x(2);

        d_x(0) = static_cast<float>(hypot(x_b - robotPose.x, y_b - robotPose.y));
        d_x(1) = std::atan2(y_b - robotPose.y, x_b - robotPose.x) - robotPose.h;

        // Calculate invariance
        V_k = z_x - d_x;

        // Calculate jacobians
        H_k(0,0) = (x_b - robotPose.x) / d_x(0);
        H_k(0,1) = (y_b - robotPose.y) / d_x(0);
        H_k(1,0) = (robotPose.y - y_b) / (d_x(0)*d_x(0));
        H_k(1,1) = (x_b - robotPose.x) / (d_x(0)*d_x(0));

        // Update the measurement covariance matrix
        R_k(0,0) = z.distanceSD;
        R_k(1,1) = z.bearingSD;
    }
}

/**
 * Method to ensure that the ball estimate does have any unrealistic values
 */
void BallEKF::limitAPrioriEst()
{
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
        xhat_k_bar(2) = 0.0f;
        xhat_k(2) = 0.0f;
        xhat_k_bar(3) = 0.0f;
        xhat_k(3) = 0.0f;

    }
    if(xhat_k(0) < X_EST_MIN) {
        xhat_k_bar(0) = X_EST_MIN;
        xhat_k(0) = X_EST_MIN;
        xhat_k_bar(2) = 0.0f;
        xhat_k(2) = 0.0f;
        xhat_k_bar(3) = 0.0f;
        xhat_k(3) = 0.0f;

    }
    if(xhat_k(1) > Y_EST_MAX) {
        xhat_k_bar(1) = Y_EST_MAX;
        xhat_k(1) = Y_EST_MAX;
        xhat_k_bar(2) = 0.0f;
        xhat_k(2) = 0.0f;
        xhat_k_bar(3) = 0.0f;
        xhat_k(3) = 0.0f;
    }
    if(xhat_k(1) < Y_EST_MIN) {
        xhat_k_bar(1) = Y_EST_MIN;
        xhat_k(1) = Y_EST_MIN;
        xhat_k_bar(2) = 0.0f;
        xhat_k(2) = 0.0f;
        xhat_k_bar(3) = 0.0f;
        xhat_k(3) = 0.0f;
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

/**
 * Method to use the estimate ellipse to intelligently clip the ball estimate
 */
void BallEKF::clipBallEstimate()
{
    // Limit our X estimate
    if (xhat_k(0) > X_EST_MAX) {
        StateVector v(numStates);
        v(0) = 1.0f;
        xhat_k = xhat_k - prod(P_k,v)* (inner_prod(v,xhat_k) - X_EST_MAX) /
            inner_prod(v, prod(P_k,v));
    }
    else if (xhat_k(0) < X_EST_MIN) {
        StateVector v(numStates);
        v(0) = 1.0f;
        xhat_k = xhat_k - prod(P_k,v)* (inner_prod(v,xhat_k)) /
            inner_prod(v, prod(P_k,v));
    }

    // Limit our Y estimate
    if (xhat_k(1) < Y_EST_MIN) {
        StateVector v(numStates);
        v(1) = 1.0f;
        xhat_k = xhat_k - prod(P_k,v)* (inner_prod(v,xhat_k)) /
            inner_prod(v, prod(P_k,v));
    }
    else if (xhat_k(1) > Y_EST_MAX) {
        StateVector v(numStates);
        v(1) = 1.0f;
        xhat_k = xhat_k - prod(P_k,v)* (inner_prod(v,xhat_k) - Y_EST_MAX) /
            inner_prod(v, prod(P_k,v));

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

    if (std::abs(xhat_k(2)) < 0.2f) {
        xhat_k(2) = 0.0f;
    }
    if (std::abs(xhat_k(3)) < 0.2f) {
        xhat_k(3) = 0.0f;
    }

}

void BallEKF::testForNaNReset()
{
    if (isnan(getXEst()) || isnan(getYEst()) ||
        isnan(getXVelocityEst()) || isnan(getYVelocityEst())) {
        std::cout << "Reseting BallEKF do to nan value." << std::endl;
        std::cout << "Current values are " << *this << std::endl;
        reset();
    }
    if (isinf(getXEst()) || isinf(getYEst()) ||
        isinf(getXVelocityEst()) || isinf(getYVelocityEst())) {
        std::cout << "Reseting BallEKF do to inf value." << std::endl;
        std::cout << "Current values are " << *this << std::endl;
        reset();
    }

}
