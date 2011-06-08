#include "BallEKF.h"
#include "FieldConstants.h"
using namespace boost::numeric;
using namespace boost;
using namespace NBMath;
using namespace std;
using namespace ekf;

// Parameters
const float BallEKF::ASSUMED_FPS = 30.0f;
// Currently we always use Cartesian measurements
const float BallEKF::USE_CARTESIAN_BALL_DIST = 50000.0f;

// How much uncertainty naturally grows per update
const float BallEKF::BETA_BALL = 5.0f;
const float BallEKF::BETA_BALL_VEL = 50.0f;
// How much ball velocity should effect uncertainty
const float BallEKF::GAMMA_BALL = 0.4f;
const float BallEKF::GAMMA_BALL_VEL = 1.0f;
const float BallEKF::CARPET_FRICTION = -(1.0f / 4.0f);//25.0f; // 25 cm/s^2
const float BallEKF::BALL_DECAY_PERCENT = 0.25f;

// Default initialization values
const float BallEKF::INIT_BALL_X = CENTER_FIELD_X;
const float BallEKF::INIT_BALL_Y = CENTER_FIELD_Y;
const float BallEKF::INIT_BALL_X_VEL = 0.0f;
const float BallEKF::INIT_BALL_Y_VEL = 0.0f;
const float BallEKF::X_UNCERT_MAX = FIELD_WIDTH / 2.0f;
const float BallEKF::Y_UNCERT_MAX = FIELD_HEIGHT / 2.0f;
const float BallEKF::VELOCITY_UNCERT_MAX = 150.0f;
const float BallEKF::X_UNCERT_MIN = 1.0e-6f;
const float BallEKF::Y_UNCERT_MIN = 1.0e-6f;
const float BallEKF::VELOCITY_UNCERT_MIN = 1.0e-6f;
const float BallEKF::INIT_X_UNCERT = FIELD_WIDTH / 4.0f;
const float BallEKF::INIT_Y_UNCERT = FIELD_HEIGHT / 4.0f;
const float BallEKF::INIT_X_VEL_UNCERT = 300.0f;
const float BallEKF::INIT_Y_VEL_UNCERT = 300.0f;
const float BallEKF::X_EST_MIN = 0.0f;
const float BallEKF::Y_EST_MIN = 0.0f;
const float BallEKF::X_EST_MAX = FIELD_WIDTH;
const float BallEKF::Y_EST_MAX = FIELD_HEIGHT;
const float BallEKF::VELOCITY_EST_MAX = 150.0f;
const float BallEKF::VELOCITY_EST_MIN = -150.0f;
const float BallEKF::VELOCITY_EST_MIN_SPEED = 0.01f;
// Distance to see a ball "jump" at which we reset velocity to 0
const float BallEKF::BALL_JUMP_VEL_THRESH = 250.0f;
static const bool USE_BALL_JUMP_RESET = true;

BallEKF::BallEKF()
    : EKF<RangeBearingMeasurement, MotionModel, ball_ekf_dimension,
          dist_bearing_meas_dim>(BETA_BALL,GAMMA_BALL)
{
    // ones on the diagonal
    A_k(0,0) = 1.0;
    A_k(1,1) = 1.0;
    A_k(2,2) = 1.0;
    A_k(3,3) = 1.0;

    // Assummed change in position necessary for velocity to work correctly
    A_k(0,2) = 1.0f / ASSUMED_FPS;
    A_k(1,3) = 1.0f / ASSUMED_FPS;

    // Set velocity uncertainty parameters
    // betas(2) = BETA_BALL_VEL;
    // betas(3) = BETA_BALL_VEL;
    // gammas(2) = GAMMA_BALL_VEL;
    // gammas(3) = GAMMA_BALL_VEL;

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
    : EKF<RangeBearingMeasurement, MotionModel, ball_ekf_dimension,
          dist_bearing_meas_dim>(BETA_BALL,GAMMA_BALL)
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
    }
    // Set the initial values
    setXEst(INIT_BALL_X);
    setYEst(INIT_BALL_Y);
    setXVelocityEst(INIT_BALL_X_VEL);
    setYVelocityEst(INIT_BALL_Y_VEL);
    setXUncert(INIT_X_UNCERT);
    P_k_bar(0,0) = INIT_X_UNCERT;
    setYUncert(INIT_Y_UNCERT);
    P_k_bar(1,1) = INIT_Y_UNCERT;
    setXVelocityUncert(INIT_X_VEL_UNCERT);
    P_k_bar(2,2) = INIT_X_VEL_UNCERT;
    setYVelocityUncert(INIT_X_VEL_UNCERT);
    P_k_bar(3,3) = INIT_Y_VEL_UNCERT;
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
        vector<RangeBearingMeasurement> z;
        z.push_back(ball);
        correctionStep(z);

    } else { // No ball seen
        noCorrectionStep();
    }
    limitPosteriorUncert();
    limitPosteriorEst();
    if (testForNaNReset()) {
        cout << "\tBallEKF reset to " << *this << endl;
        cout << "\tObservation was: " << ball << endl;
    }
}

/**
 * Method incorporate the expected change in ball position from the last
 * frame.  Updates the values of the covariance matrix Q_k and the jacobian
 * A_k.
 *
 * @param u The motion model of the last frame.  Ignored for the ball.
 * @return The expected change in ball position (x,y, xVelocity, yVelocity)
 */
EKF<RangeBearingMeasurement, MotionModel, ball_ekf_dimension,
    dist_bearing_meas_dim>::StateVector BallEKF::associateTimeUpdate(
        MotionModel u)
{
    StateVector deltaBall(ball_ekf_dimension);

    float dt = 1.0f / ASSUMED_FPS;
    // Calculate the assumed change in ball position
    deltaBall(0) = getXVelocityEst() * dt;
    deltaBall(1) = getYVelocityEst() * dt;
    // Decrease in velocity with respect to velocity and friction
    deltaBall(2) = CARPET_FRICTION * getXVelocityEst() *dt;
    deltaBall(3) = CARPET_FRICTION * getYVelocityEst() *dt;

    A_k(0,2) = dt;
    A_k(1,3) = dt;

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
void BallEKF::incorporateMeasurement(const RangeBearingMeasurement& z,
                                     StateMeasurementMatrix &H_k,
                                     MeasurementMatrix &R_k,
                                     MeasurementVector &V_k)
{
    // Currently we always use Cartesian measurements
    // We originally only used this for ball's closer than 50cm,
    // However it was found to make velocity estimates much less noisy to
    // always use Cartesian coordinates
    if (z.distance < USE_CARTESIAN_BALL_DIST) {
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

        // This could be done better to create an ellipse that more accurately
        // measures uncertainty. Currently uncertainty is over-sized as a circle
        // about the ball estimate.  The more correct implementation is the
        // commented out code bellow the next 2 lines of code.  It should be
        // tested and the beta and gamma parameters possibly tuned before being
        // used.
		const float sd_dist_sq = pow(z.distanceSD, 2);
		const float var = sd_dist_sq * sin(z.bearing) * cos(z.bearing);
        R_k(0,0) = sd_dist_sq * pow(cos(z.bearing), 2);
		R_k(0,1) = var;
		R_k(1,0) = var;
        R_k(1,1) = sd_dist_sq * pow(sin(z.bearing), 2);

    } else {
        // Convert our sighting to cartesian coordinates
        MeasurementVector z_x(2);
        z_x(0) = z.distance;
        z_x(1) = z.bearing;

        // Get expected values of ball
        const float x_b = getXEst();
        const float y_b = getYEst();

        MeasurementVector d_x(2);

        d_x(0) = static_cast<float>(hypot(x_b - robotPose.x,
                                          y_b - robotPose.y));
        d_x(1) = subPIAngle(safe_atan2(y_b - robotPose.y,
                                       x_b - robotPose.x) - robotPose.h);

        // Calculate invariance
        V_k = z_x - d_x;
        // cout << "\tExpected was: " << d_x << endl;
        // cout << "\tObserved was: " << z_x << endl;

        // Calculate jacobians
        H_k(0,0) = (x_b - robotPose.x) / d_x(0);
        H_k(0,1) = (y_b - robotPose.y) / d_x(0);
        H_k(1,0) = (robotPose.y - y_b) / (d_x(0)*d_x(0));
        H_k(1,1) = (x_b - robotPose.x) / (d_x(0)*d_x(0));

        // Update the measurement covariance matrix
        R_k(0,0) = z.distanceSD * z.distanceSD;
        R_k(1,1) = z.bearingSD * z.bearingSD;
    }

}

void BallEKF::beforeCorrectionFinish(void)
{
    // We set velocity to 0, if the ball jumps too much
    // The ball was likely moved by a referee or hasn't been seen in a while
    // If the ball IS moving, then we'll pick it up in the next frame
    if (USE_BALL_JUMP_RESET &&
        (abs(xhat_k(2) - xhat_k_bar(2)) > BALL_JUMP_VEL_THRESH ||
         abs(xhat_k(3) - xhat_k_bar(3)) > BALL_JUMP_VEL_THRESH) ) {
        xhat_k_bar(2) = 0.0f;
        xhat_k(2) = 0.0f;
        xhat_k_bar(3) = 0.0f;
        xhat_k(3) = 0.0f;
        P_k(2,2) = VELOCITY_UNCERT_MAX;
        P_k_bar(2,2) = VELOCITY_UNCERT_MAX;
        P_k(3,3) = VELOCITY_UNCERT_MAX;
        P_k_bar(3,3) = VELOCITY_UNCERT_MAX;
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
    // Clip the ball position estimate if it goes off of the field
    // Reset the velocity to zero if we do this
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

    if(std::abs(xhat_k(2)) < VELOCITY_EST_MIN_SPEED) {
        xhat_k_bar(2) = 0.0f;
        xhat_k(2) = 0.0f;
    }
    if(std::abs(xhat_k(3)) < VELOCITY_EST_MIN_SPEED) {
        xhat_k_bar(3) = 0.0f;
        xhat_k(3) = 0.0f;
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

    // We don't want any covariance values getting too large
    for (unsigned int i = 0; i < numStates; ++i) {
        for (unsigned int j = 0; j < numStates; ++j) {
            if(P_k(i,j) > X_UNCERT_MAX) {
                P_k(i,j) = X_UNCERT_MAX;
                P_k_bar(i,j) = X_UNCERT_MAX;
            }
        }
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

    if (std::abs(xhat_k(2)) < VELOCITY_EST_MIN_SPEED) {
        xhat_k(2) = 0.0f;
    }
    if (std::abs(xhat_k(3)) < VELOCITY_EST_MIN_SPEED) {
        xhat_k(3) = 0.0f;
    }

}
