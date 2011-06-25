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
          dist_bearing_meas_dim>(BETA_BALL,GAMMA_BALL), robotPose(),
      lastUpdateTime(0), frameLength(0.0f)
{
    // ones on the diagonal
    A_k(x_index,x_index) = 1.0;
    A_k(y_index,y_index) = 1.0;
    A_k(2,2) = 1.0;
    A_k(3,3) = 1.0;

    // Assummed change in position necessary for velocity to work correctly
    A_k(0,vel_x_index) = 1.0f / ASSUMED_FPS;
    A_k(1,vel_y_index) = 1.0f / ASSUMED_FPS;

    // Set velocity uncertainty parameters
    // betas(2) = BETA_BALL_VEL;
    // betas(3) = BETA_BALL_VEL;
    // gammas(2) = GAMMA_BALL_VEL;
    // gammas(3) = GAMMA_BALL_VEL;

    // Setup initial values
    setGlobalXEst(INIT_BALL_X);
    setGlobalYEst(INIT_BALL_Y);
    setGlobalXVelocityEst(INIT_BALL_X_VEL);
    setGlobalYVelocityEst(INIT_BALL_Y_VEL);
    setGlobalXUncert(INIT_X_UNCERT);
    setGlobalYUncert(INIT_Y_UNCERT);
    setGlobalXVelocityUncert(INIT_X_VEL_UNCERT);
    setGlobalYVelocityUncert(INIT_Y_VEL_UNCERT);
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
    A_k(x_index,x_index) = 1.0;
    A_k(y_index,y_index) = 1.0;
    A_k(vel_x_index,vel_x_index) = 1.0;
    A_k(vel_y_index,vel_y_index) = 1.0;

    // Assummed change in position necessary for velocity to work correctly
    A_k(0,vel_x_index) = 1.0f / ASSUMED_FPS;
    A_k(1,vel_y_index) = 1.0f / ASSUMED_FPS;

    // Setup initial values
    setGlobalXEst(initX);
    setGlobalYEst(initY);
    setGlobalXVelocityEst(initVelX);
    setGlobalYVelocityEst(initVelY);
    setGlobalXUncert(initXUncert);
    setGlobalYUncert(initYUncert);
    setGlobalXVelocityUncert(initVelXUncert);
    setGlobalYVelocityUncert(initVelYUncert);
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
    setGlobalXEst(INIT_BALL_X);
    setGlobalYEst(INIT_BALL_Y);
    setGlobalXVelocityEst(INIT_BALL_X_VEL);
    setGlobalYVelocityEst(INIT_BALL_Y_VEL);
    setGlobalXUncert(INIT_X_UNCERT);
    P_k_bar(x_index,x_index) = INIT_X_UNCERT;
    setGlobalYUncert(INIT_Y_UNCERT);
    P_k_bar(y_index,y_index) = INIT_Y_UNCERT;
    setGlobalXVelocityUncert(INIT_X_VEL_UNCERT);
    P_k_bar(vel_x_index,vel_x_index) = INIT_X_VEL_UNCERT;
    setGlobalYVelocityUncert(INIT_X_VEL_UNCERT);
    P_k_bar(vel_y_index,vel_y_index) = INIT_Y_VEL_UNCERT;
}

/**
 * Method to deal with updating the entire ball model
 *
 * @param ball the ball seen this frame.
 */
void BallEKF::updateModel(const MotionModel& odo,
                          const RangeBearingMeasurement& ball,
                          const PoseEst& p)
{
    robotPose = p;

    // if (ball.distance > 0.0 &&
    //     resetFlag){
    //     resetState();
    //     resetFlag = false;
    // }

    // Update expected ball movement
    timeUpdate(odo);
    limitAPrioriUncert();

    // We've seen a ball
    if (ball.distance > 0.0) {
        vector<RangeBearingMeasurement> z;
        z.push_back(ball);
        correctionStep(z);

    } else { // No ball seen
        noCorrectionStep();
    }

    // limitPosteriorUncert();
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
 * @param u The motion model of the last frame.
 * @return The expected change in ball position (x,y, xVelocity, yVelocity)
 */
EKF<RangeBearingMeasurement,
    MotionModel, ball_ekf_dimension,
    dist_bearing_meas_dim>::StateVector
BallEKF::associateTimeUpdate(MotionModel odo)
{
    // Get time since last update
    long long int time = monotonic_micro_time();
    frameLength = static_cast<float>(time - lastUpdateTime)/
        1000000; // u_s to sec
    lastUpdateTime = time;


    StateVector deltaBall(ball_ekf_dimension);

    // Changes to ball position from odometry
    deltaBall(x_index) = sin(odo.deltaR) * odo.deltaL -
        cos(odo.deltaR) * odo.deltaF;

    deltaBall(y_index) = -sin(odo.deltaR) * odo.deltaF -
        cos(odo.deltaR) * odo.deltaL;

    const float velX = getRelativeXVelocity();
    const float velY = getRelativeYVelocity();

    // Rotate the velocity components into the new coordinate frame
    float newVelX = cos(odo.deltaR) * velX - sin(odo.deltaR) * velY;
    float newVelY = cos(odo.deltaR) * velY + sin(odo.deltaR) * velX;

    // Add position changes from velocity
    deltaBall(x_index) += newVelX * frameLength;
    deltaBall(y_index) += newVelY * frameLength;

    // Change in velocity direction from odometry
    deltaBall(vel_x_index) = newVelX - getRelativeXVelocity();
    deltaBall(vel_y_index) = newVelY - getRelativeYVelocity();

    // Deceleration of ball due to friction (physics!)
    deltaBall(vel_x_index) -= (copysignf(1.0f, newVelX) *
                               CARPET_FRICTION * newVelX * frameLength);
    deltaBall(vel_y_index) -= (copysignf(1.0f, newVelY) *
                               CARPET_FRICTION * newVelY * frameLength);

    A_k(0,vel_x_index) = frameLength;
    A_k(1,vel_y_index) = frameLength;

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

    // Convert our sighting to cartesian coordinates
    const float x_b_r = z.distance * cos(z.bearing);
    const float y_b_r = z.distance * sin(z.bearing);

    // Measured values
    MeasurementVector z_x(2);
    z_x(x_index) = x_b_r;
    z_x(y_index) = y_b_r;

    z_x(vel_x_index) = (x_b_r - xhat_k_bar(x_index)) / frameLength;
    z_x(vel_y_index) = (y_b_r - xhat_k_bar(y_index)) / frameLength;

    // Calculate invariance
    V_k = z_x - xhat_k_bar;

    // Calculate jacobians
    float sinB = sin(z.bearing);
    float cosB = cos(z.bearing);

    // Derivatives with respect to distance
    H_k(0,x_index)     = cosB;
    H_k(0,y_index)     = sinB;
    H_k(0,vel_x_index) = cosB/frameLength;
    H_k(0,vel_y_index) = sinB/frameLength;

    // Derivatives with respect to bearing
    H_k(1,x_index)     = -sinB             * z.distance;
    H_k(1,y_index)     =  cosB             * z.distance;
    H_k(1,vel_x_index) = -sinB/frameLength * z.distance;
    H_k(1,vel_y_index) =  cosB/frameLength * z.distance;

    // Update the measurement covariance matrix

    const float sd_dist_sq = z.distanceSD * z.distanceSD;
    const float var = sd_dist_sq * sin(z.bearing) * cos(z.bearing);

    R_k(0,x_index) = sd_dist_sq * cosB;
    R_k(0,y_index) = sd_dist_sq * sinB;
    R_k(0,vel_x_index) = sd_dist_sq * cosB / frameLength;
    R_k(0,vel_y_index) = sd_dist_sq * sinB / frameLength;

    R_k(1,x_index) = z.bearingSD * z.bearingSD;
    R_k(1,y_index) = z.bearingSD * z.bearingSD;
    R_k(1,vel_x_index) = z.bearingSD * z.bearingSD / frameLength;
    R_k(1,vel_y_index) = z.bearingSD * z.bearingSD / frameLength;
}

void BallEKF::beforeCorrectionFinish(void)
{

}

/**
 * Method to ensure that the ball estimate does have any unrealistic values
 */
void BallEKF::limitAPrioriEst()
{
    xhat_k_bar(vel_x_index) =
        NBMath::clip(xhat_k_bar(vel_x_index), VELOCITY_EST_MIN, VELOCITY_EST_MAX);
    xhat_k_bar(vel_y_index) =
        NBMath::clip(xhat_k_bar(vel_y_index), VELOCITY_EST_MIN, VELOCITY_EST_MAX);
}

/**
 * Method to ensure that the ball estimate does have any unrealistic values
 */
void BallEKF::limitPosteriorEst()
{
    // Clip the ball position estimate if it goes off of the field
    // Reset the velocity to zero if we do this
    if(xhat_k(x_index) > X_EST_MAX) {
        xhat_k_bar(x_index) = X_EST_MAX;
        xhat_k(x_index) = X_EST_MAX;
        xhat_k_bar(vel_x_index) = 0.0f;
        xhat_k(vel_x_index) = 0.0f;
        xhat_k_bar(vel_y_index) = 0.0f;
        xhat_k(vel_y_index) = 0.0f;

    }
    if(xhat_k(x_index) < X_EST_MIN) {
        xhat_k_bar(x_index) = X_EST_MIN;
        xhat_k(x_index) = X_EST_MIN;
        xhat_k_bar(vel_x_index) = 0.0f;
        xhat_k(vel_x_index) = 0.0f;
        xhat_k_bar(vel_y_index) = 0.0f;
        xhat_k(vel_y_index) = 0.0f;

    }
    if(xhat_k(y_index) > Y_EST_MAX) {
        xhat_k_bar(y_index) = Y_EST_MAX;
        xhat_k(y_index) = Y_EST_MAX;
        xhat_k_bar(vel_x_index) = 0.0f;
        xhat_k(vel_x_index) = 0.0f;
        xhat_k_bar(vel_y_index) = 0.0f;
        xhat_k(vel_y_index) = 0.0f;
    }
    if(xhat_k(y_index) < Y_EST_MIN) {
        xhat_k_bar(y_index) = Y_EST_MIN;
        xhat_k(y_index) = Y_EST_MIN;
        xhat_k_bar(vel_x_index) = 0.0f;
        xhat_k(vel_x_index) = 0.0f;
        xhat_k_bar(vel_y_index) = 0.0f;
        xhat_k(vel_y_index) = 0.0f;
    }
    if(xhat_k(vel_x_index) > VELOCITY_EST_MAX) {
        xhat_k_bar(vel_x_index) = VELOCITY_EST_MAX;
        xhat_k(vel_x_index) = VELOCITY_EST_MAX;
    }
    if(xhat_k(vel_x_index) < VELOCITY_EST_MIN) {
        xhat_k_bar(vel_x_index) = VELOCITY_EST_MIN;
        xhat_k(vel_x_index) = VELOCITY_EST_MIN;
    }
    if(xhat_k(vel_y_index) > VELOCITY_EST_MAX) {
        xhat_k_bar(vel_y_index) = VELOCITY_EST_MAX;
        xhat_k(vel_y_index) = VELOCITY_EST_MAX;
    }
    if(xhat_k(vel_y_index) < VELOCITY_EST_MIN) {
        xhat_k_bar(vel_y_index) = VELOCITY_EST_MIN;
        xhat_k(vel_y_index) = VELOCITY_EST_MIN;
    }

    if(std::abs(xhat_k(vel_x_index)) < VELOCITY_EST_MIN_SPEED) {
        xhat_k_bar(vel_x_index) = 0.0f;
        xhat_k(vel_x_index) = 0.0f;
    }
    if(std::abs(xhat_k(vel_y_index)) < VELOCITY_EST_MIN_SPEED) {
        xhat_k_bar(vel_y_index) = 0.0f;
        xhat_k(vel_y_index) = 0.0f;
    }
}

/**
 * Method to ensure that uncertainty does not grow without bound
 */
void BallEKF::limitAPrioriUncert()
{

    // Check x uncertainty
    if(P_k_bar(x_index,x_index) > X_UNCERT_MAX) {
        P_k_bar(x_index,x_index) = X_UNCERT_MAX;
    }
    // Check y uncertainty
    if(P_k_bar(y_index,y_index) > Y_UNCERT_MAX) {
        P_k_bar(y_index,y_index) = Y_UNCERT_MAX;
    }
    // Check x veolcity uncertainty
    if(P_k_bar(vel_x_index,vel_x_index) > VELOCITY_UNCERT_MAX) {
        P_k_bar(vel_x_index,vel_x_index) = VELOCITY_UNCERT_MAX;
    }
    // Check y veolcity uncertainty
    if(P_k_bar(vel_y_index,vel_y_index) > VELOCITY_UNCERT_MAX) {
        P_k_bar(vel_y_index,vel_y_index) = VELOCITY_UNCERT_MAX;
    }
    // Check x uncertainty
    if(P_k_bar(x_index,x_index) < X_UNCERT_MIN) {
        P_k_bar(x_index,x_index) = X_UNCERT_MIN;
    }
    // Check y uncertainty
    if(P_k_bar(y_index,y_index) < Y_UNCERT_MIN) {
        P_k_bar(y_index,y_index) = Y_UNCERT_MIN;
    }
    // Check x veolcity uncertainty
    if(P_k_bar(vel_x_index,vel_x_index) < VELOCITY_UNCERT_MIN) {
        P_k_bar(vel_x_index,vel_x_index) = VELOCITY_UNCERT_MIN;
    }
    // Check y veolcity uncertainty
    if(P_k_bar(vel_y_index,vel_y_index) < VELOCITY_UNCERT_MIN) {
        P_k_bar(vel_y_index,vel_y_index) = VELOCITY_UNCERT_MIN;
    }
}

/**
 * Method to ensure that uncertainty does not grow or shrink without bound
 */
void BallEKF::limitPosteriorUncert()
{
    P_k(x_index,x_index) = P_k_bar(x_index,x_index) =
        NBMath::clip(P_k(x_index,x_index), X_UNCERT_MIN, X_UNCERT_MAX);
    P_k(y_index,y_index) = P_k_bar(y_index,y_index) =
        NBMath::clip(P_k(y_index,y_index), Y_UNCERT_MIN, Y_UNCERT_MAX);
    P_k(vel_x_index,vel_x_index) = P_k_bar(vel_x_index,vel_x_index) =
        NBMath::clip(P_k(vel_x_index,vel_x_index),
                     VELOCITY_UNCERT_MIN,
                     VELOCITY_UNCERT_MAX);
    P_k(vel_y_index,vel_y_index) = P_k_bar(vel_y_index,vel_y_index) =
        NBMath::clip(P_k(vel_y_index,vel_y_index),
                     VELOCITY_UNCERT_MIN,
                     VELOCITY_UNCERT_MAX);

    // We don't want any covariance values getting too large
    for (unsigned int i = 0; i < numStates; ++i) {
        for (unsigned int j = 0; j < numStates; ++j) {
            if(P_k(i,j) > X_UNCERT_MAX) {
                P_k(i,j) = X_UNCERT_MAX;
                P_k_bar(i,j) = X_UNCERT_MAX;
            }
        }
    }
}

/**
 * Method to use the estimate ellipse to intelligently clip the ball estimate
 */
void BallEKF::clipBallEstimate()
{
    // Limit our X estimate
    if (xhat_k(x_index) > X_EST_MAX) {
        StateVector v(numStates);
        v(x_index) = 1.0f;
        xhat_k = xhat_k - prod(P_k,v)* (inner_prod(v,xhat_k) - X_EST_MAX) /
            inner_prod(v, prod(P_k,v));
    }
    else if (xhat_k(x_index) < X_EST_MIN) {
        StateVector v(numStates);
        v(x_index) = 1.0f;
        xhat_k = xhat_k - prod(P_k,v)* (inner_prod(v,xhat_k)) /
            inner_prod(v, prod(P_k,v));
    }

    // Limit our Y estimate
    if (xhat_k(y_index) < Y_EST_MIN) {
        StateVector v(numStates);
        v(y_index) = 1.0f;
        xhat_k = xhat_k - prod(P_k,v)* (inner_prod(v,xhat_k)) /
            inner_prod(v, prod(P_k,v));
    }
    else if (xhat_k(y_index) > Y_EST_MAX) {
        StateVector v(numStates);
        v(y_index) = 1.0f;
        xhat_k = xhat_k - prod(P_k,v)* (inner_prod(v,xhat_k) - Y_EST_MAX) /
            inner_prod(v, prod(P_k,v));

    }

    if(xhat_k(vel_x_index) > VELOCITY_EST_MAX) {
        xhat_k_bar(vel_x_index) = VELOCITY_EST_MAX;
        xhat_k(vel_x_index) = VELOCITY_EST_MAX;
    }
    if(xhat_k(vel_x_index) < VELOCITY_EST_MIN) {
        xhat_k_bar(vel_x_index) = VELOCITY_EST_MIN;
        xhat_k(vel_x_index) = VELOCITY_EST_MIN;
    }
    if(xhat_k(vel_y_index) > VELOCITY_EST_MAX) {
        xhat_k_bar(vel_y_index) = VELOCITY_EST_MAX;
        xhat_k(vel_y_index) = VELOCITY_EST_MAX;
    }
    if(xhat_k(vel_y_index) < VELOCITY_EST_MIN) {
        xhat_k_bar(vel_y_index) = VELOCITY_EST_MIN;
        xhat_k(vel_y_index) = VELOCITY_EST_MIN;
    }

    if (std::abs(xhat_k(vel_x_index)) < VELOCITY_EST_MIN_SPEED) {
        xhat_k(vel_x_index) = 0.0f;
    }
    if (std::abs(xhat_k(vel_y_index)) < VELOCITY_EST_MIN_SPEED) {
        xhat_k(vel_y_index) = 0.0f;
    }

}

const float BallEKF::getGlobalX() const
{

}
const float BallEKF::getGlobalY() const
{

}
const float BallEKF::getGlobalXVelocity() const
{

}
const float BallEKF::getGlobalYVelocity() const
{

}

/**
 * Global uncertainties
 */
const float BallEKF::getGlobalXUncert() const
{

}
const float BallEKF::getGlobalYUncert() const
{

}
const float BallEKF::getGlobalXVelocityUncert() const
{

}
const float BallEKF::getGlobalYVelocityUncert() const
{

}
