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
const float BallEKF::CARPET_FRICTION = -(1.0f / 2.0f);//25.0f; // 25 cm/s^2
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
          ball_ekf_meas_dim>(BETA_BALL,GAMMA_BALL), robotPose(),
      lastUpdateTime(0), dt(0.0f)
{

    ///////////////////////
    // Derivatives of ball position update re:x,y,vel_x,vel_y
    //
    // NOTE: These values are constant, so we set them here to avoid
    //       unnecessary resetting every frame
    ///////////////////////

    // derivatives of x

    // derivatives of y

    // derivatives of vel_x

    // derivatives of vel_y

    // Set velocity uncertainty parameters
    // betas(2) = BETA_BALL_VEL;
    // betas(3) = BETA_BALL_VEL;
    // gammas(2) = GAMMA_BALL_VEL;
    // gammas(3) = GAMMA_BALL_VEL;
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
          ball_ekf_meas_dim>(BETA_BALL,GAMMA_BALL)
{

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
    P_k_bar(x_index,x_index) = INIT_X_UNCERT;
    P_k_bar(y_index,y_index) = INIT_Y_UNCERT;
    P_k_bar(vel_x_index,vel_x_index) = INIT_X_VEL_UNCERT;
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
    // limitAPrioriUncert();

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
    curOdo = odo;
    StateVector deltaBall(ball_ekf_dimension);

    updateFrameLength();
    updatePosition              ( odo, deltaBall);
    updateVelocity              ( odo, deltaBall);
    updateAcceleration          ( odo, deltaBall);
    calculateTimeUpdateJacobian ( odo, deltaBall);

    return deltaBall;
}

void BallEKF::updateFrameLength()
{
    // Get time since last update
    const long long int time = monotonic_micro_time();
    dt = static_cast<float>(time - lastUpdateTime)/
        1000000.0f; // u_s to sec
    lastUpdateTime = time;
    cout << "dt: " << dt << endl;
}

void BallEKF::updatePosition(const MotionModel& odo, StateVector& deltaBall)
{

    // Calculate change in position from velocity and acceleration
    // ds = vt + .5*a*t^2 + .5*(friction deceleration)*t^2
    deltaBall(x_index) = (xhat_k(vel_x_index)*dt +
                          .5f * (xhat_k(acc_x_index) +
                                copysignf(1.0f, vel_x_index) * CARPET_FRICTION )
                          *dt*dt ); // Friction needs correct sign

    deltaBall(y_index) = (xhat_k(vel_y_index)*dt +
                          .5f * (xhat_k(acc_y_index) +
                                copysignf(1.0f, vel_y_index) * CARPET_FRICTION )
                          *dt*dt ); // Friction needs correct sign

    // Ensure that the change in direction is in the same direction as
    // the velocity
    deltaBall(x_index) = xhat_k(vel_x_index) > 0 ?
        max(0.0f, deltaBall(x_index)) :
        deltaBall(x_index) = min(0.0f, deltaBall(x_index));
    deltaBall(y_index) = xhat_k(vel_y_index) > 0 ?
        max(0.0f, deltaBall(y_index)) :
        deltaBall(y_index) = min(0.0f, deltaBall(y_index));

    // Rotate the position according to odometry
    deltaBall(x_index) += -sin(odo.deltaR) * odo.deltaL -
        cos(odo.deltaR) * odo.deltaF;
    deltaBall(y_index) += sin(odo.deltaR) * odo.deltaF -
        cos(odo.deltaR) * odo.deltaL;
}

void BallEKF::updateVelocity(const MotionModel& odo, StateVector& deltaBall)
{
    // Velocities
    float velX = xhat_k(vel_x_index) +
        xhat_k(acc_x_index) * dt;
    float velY = xhat_k(vel_y_index) +
        xhat_k(acc_y_index) * dt;

    // Deceleration of ball due to friction (physics! sort of).  This
    // is not the proper method of slowing an object. The magnitude
    // should be slowed by the friction amount, not the components
    // individually.
    velX += CARPET_FRICTION * velX * dt;
    velY += CARPET_FRICTION * velY * dt;

    // Rotate the velocity components into the new coordinate frame
    float newVelX = cos(odo.deltaR) * velX + sin(odo.deltaR) * velY;
    float newVelY = cos(odo.deltaR) * velY - sin(odo.deltaR) * velX;

    // Change in velocity direction from odometry
    deltaBall(vel_x_index) = newVelX - velX;
    deltaBall(vel_y_index) = newVelY - velY;
}

void BallEKF::updateAcceleration(const MotionModel& odo, StateVector& deltaBall)
{
    // We know no better way of dealing with change in acceleration
    // than leaving it up to the correction step
    deltaBall(acc_x_index) = deltaBall(acc_y_index) = 0.0;
}

void BallEKF::calculateTimeUpdateJacobian(const MotionModel& odo,
                                          StateVector& deltaBall)
{
    const float dt2 = dt*dt;
    ///////////////////////
    // Derivatives of ball position update re:x,y,vel_x,vel_y
    //
    // NOTE: These are the values which change every frame. The
    //       constant values are all set once in the constructor.
    ///////////////////////

    // derivatives of x
    A_k(x_index,x_index)         = 1.0;
    A_k(x_index,y_index)         = 0.0;
    A_k(x_index,vel_x_index)     = dt;
    A_k(x_index,vel_y_index)     = 0.0;
    A_k(x_index,acc_x_index)     = .5f * dt2;
    A_k(x_index,acc_y_index)     = 0.0;

    // derivatives of y
    A_k(y_index,x_index)         = 0.0;
    A_k(y_index,y_index)         = 1.0;
    A_k(y_index,vel_x_index)     = 0;
    A_k(y_index,vel_y_index)     = dt;
    A_k(y_index,acc_x_index)     = 0.0;
    A_k(y_index,acc_y_index)     = .5f * dt2;

    // derivatives of vel_x
    A_k(vel_x_index,x_index)     =  0.0;
    A_k(vel_x_index,y_index)     =  0.0;
    A_k(vel_x_index,vel_x_index) = cos(odo.deltaR);
    A_k(vel_x_index,vel_y_index) = sin(odo.deltaR);
    A_k(vel_x_index,acc_x_index) = cos(odo.deltaR) * dt;
    A_k(vel_x_index,acc_y_index) = sin(odo.deltaR) * dt;

    // derivatives of vel_y
    A_k(vel_y_index,x_index)     = 0.0;
    A_k(vel_y_index,y_index)     = 0.0;
    A_k(vel_y_index,vel_x_index) = -sin(odo.deltaR);
    A_k(vel_y_index,vel_y_index) =  cos(odo.deltaR);
    A_k(vel_y_index,acc_x_index) = -sin(odo.deltaR) * dt;
    A_k(vel_y_index,acc_y_index) =  cos(odo.deltaR) * dt;

    // derivatives of acc_x
    A_k(acc_x_index,x_index)     = 0.0;
    A_k(acc_x_index,y_index)     = 0.0;
    A_k(acc_x_index,vel_x_index) = 0.0;
    A_k(acc_x_index,vel_y_index) = 0.0;
    A_k(acc_x_index,acc_x_index) = 1.0;
    A_k(acc_x_index,acc_y_index) = 0.0;

    // derivatives of acc_y
    A_k(acc_y_index,x_index)     = 0.0;
    A_k(acc_y_index,y_index)     = 0.0;
    A_k(acc_y_index,vel_x_index) = 0.0;
    A_k(acc_y_index,vel_y_index) = 0.0;
    A_k(acc_y_index,acc_x_index) = 0.0;
    A_k(acc_y_index,acc_y_index) = 1.0;
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
    // We need to translate/rotate the old values into our new
    // coordinate frame based on the odometry update
    StateVector xhat_k_prev(ball_ekf_dimension);
    xhat_k_prev(x_index)     = (xhat_k(x_index) -
                                cos(curOdo.deltaR) * curOdo.deltaF -
                                sin(curOdo.deltaR) * curOdo.deltaL);
    xhat_k_prev(y_index)     = (xhat_k(y_index) +
                                sin(curOdo.deltaR) * curOdo.deltaF -
                                cos(curOdo.deltaR) * curOdo.deltaL);

    xhat_k_prev(vel_x_index) = (cos(curOdo.deltaR) * xhat_k(vel_x_index) +
                                sin(curOdo.deltaR) * xhat_k(vel_y_index));
    xhat_k_prev(vel_y_index) = (cos(curOdo.deltaR) * xhat_k(vel_y_index) -
                                sin(curOdo.deltaR) * xhat_k(vel_x_index));

    xhat_k_prev(acc_x_index) = (cos(curOdo.deltaR) * xhat_k(acc_x_index) +
                                sin(curOdo.deltaR) * xhat_k(acc_y_index));
    xhat_k_prev(acc_y_index) = (cos(curOdo.deltaR) * xhat_k(acc_y_index) -
                                sin(curOdo.deltaR) * xhat_k(acc_x_index));

    // Calculate new velocities and accelerations
    MeasurementVector z_x(ball_ekf_meas_dim);
    z_x(x_index)     = z.distance * cos(z.bearing);
    z_x(y_index)     = z.distance * sin(z.bearing);

    z_x(vel_x_index) = (z_x(x_index) - xhat_k_prev(x_index)) / dt;
    z_x(vel_y_index) = (z_x(y_index) - xhat_k_prev(y_index)) / dt;

    z_x(acc_x_index) = (z_x(vel_x_index) -
                        xhat_k_prev(vel_x_index)) / dt;
    z_x(acc_y_index) = (z_x(vel_y_index) -
                        xhat_k_prev(vel_y_index)) / dt;

    // Calculate invariance
    V_k = z_x - xhat_k_bar;

    // Calculate jacobians
    float sinB = sin(z.bearing);
    float cosB = cos(z.bearing);

    // The derivatives of each state are differentiated with respect
    // to themselves only and undergo no further operations, H_k
    // (the Jacobian) is simply the identity matrix.
    H_k = boost::numeric::ublas::identity_matrix<float>(ball_ekf_meas_dim);

    // Update the measurement covariance matrix

    const float sd_dist_sq = z.distanceSD * z.distanceSD;
    const float var = sd_dist_sq * sin(z.bearing) * cos(z.bearing);

    R_k(x_index,x_index)         = sd_dist_sq;
    R_k(x_index,y_index)         = 0.0f;
    R_k(x_index,vel_x_index)     = 0.0f;
    R_k(x_index,vel_y_index)     = 0.0f;
    R_k(x_index,acc_x_index)     = 0.0f;
    R_k(x_index,acc_y_index)     = 0.0f;

    R_k(y_index,x_index)         = 0.0f;
    R_k(y_index,y_index)         = sd_dist_sq;
    R_k(y_index,vel_x_index)     = 0.0f;
    R_k(y_index,vel_y_index)     = 0.0f;
    R_k(y_index,acc_x_index)     = 0.0f;
    R_k(y_index,acc_y_index)     = 0.0f;

    R_k(vel_x_index,x_index)     = 0.0f;
    R_k(vel_x_index,y_index)     = 0.0f;
    R_k(vel_x_index,vel_x_index) = sd_dist_sq * 2/ dt;
    R_k(vel_x_index,vel_y_index) = 0.0f;
    R_k(vel_x_index,acc_x_index) = 0.0f;
    R_k(vel_x_index,acc_y_index) = 0.0f;

    R_k(vel_y_index,x_index)     = 0.0f;
    R_k(vel_y_index,y_index)     = 0.0f;
    R_k(vel_y_index,vel_x_index) = 0.0f;
    R_k(vel_y_index,vel_y_index) = sd_dist_sq * 2 / dt;
    R_k(vel_y_index,acc_x_index) = 0.0f;
    R_k(vel_y_index,acc_y_index) = 0.0f;

    R_k(acc_x_index,x_index)     = 0.0f;
    R_k(acc_x_index,y_index)     = 0.0f;
    R_k(acc_x_index,vel_x_index) = 0.0f;
    R_k(acc_x_index,vel_y_index) = 0.0f;
    R_k(acc_x_index,acc_x_index) = sd_dist_sq * 2 * 2 / (dt * dt);
    R_k(acc_x_index,acc_y_index) = 0.0f;

    R_k(acc_y_index,x_index)     = 0.0f;
    R_k(acc_y_index,y_index)     = 0.0f;
    R_k(acc_y_index,vel_x_index) = 0.0f;
    R_k(acc_y_index,vel_y_index) = 0.0f;
    R_k(acc_y_index,acc_x_index) = 0.0f;
    R_k(acc_y_index,acc_y_index) = sd_dist_sq * 2 * 2 / (dt*dt);

    cout << "odo: " << curOdo << endl
         << "z_x: " << z_x << endl
         << "xhat_k_prev: " << xhat_k_prev << endl
         << "xhat_k_bar: " << xhat_k_bar << endl
         << "V_k: " << V_k << endl
         << "R_k: " << R_k << endl
         << endl;
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
        NBMath::clip(xhat_k_bar(vel_x_index),
                     VELOCITY_EST_MIN,
                     VELOCITY_EST_MAX);
    xhat_k_bar(vel_y_index) =
        NBMath::clip(xhat_k_bar(vel_y_index),
                     VELOCITY_EST_MIN,
                     VELOCITY_EST_MAX);
}

/**
 * Method to ensure that the ball estimate does have any unrealistic values
 */
void BallEKF::limitPosteriorEst()
{
    // Clip the ball position estimate if it goes off of the field
    // Reset the velocity to zero if we do this
    // if(xhat_k(x_index) > X_EST_MAX) {
    //     xhat_k_bar(x_index) = X_EST_MAX;
    //     xhat_k(x_index) = X_EST_MAX;
    //     xhat_k_bar(vel_x_index) = 0.0f;
    //     xhat_k(vel_x_index) = 0.0f;
    //     xhat_k_bar(vel_y_index) = 0.0f;
    //     xhat_k(vel_y_index) = 0.0f;

    // }
    // if(xhat_k(x_index) < X_EST_MIN) {
    //     xhat_k_bar(x_index) = X_EST_MIN;
    //     xhat_k(x_index) = X_EST_MIN;
    //     xhat_k_bar(vel_x_index) = 0.0f;
    //     xhat_k(vel_x_index) = 0.0f;
    //     xhat_k_bar(vel_y_index) = 0.0f;
    //     xhat_k(vel_y_index) = 0.0f;

    // }
    // if(xhat_k(y_index) > Y_EST_MAX) {
    //     xhat_k_bar(y_index) = Y_EST_MAX;
    //     xhat_k(y_index) = Y_EST_MAX;
    //     xhat_k_bar(vel_x_index) = 0.0f;
    //     xhat_k(vel_x_index) = 0.0f;
    //     xhat_k_bar(vel_y_index) = 0.0f;
    //     xhat_k(vel_y_index) = 0.0f;
    // }
    // if(xhat_k(y_index) < Y_EST_MIN) {
    //     xhat_k_bar(y_index) = Y_EST_MIN;
    //     xhat_k(y_index) = Y_EST_MIN;
    //     xhat_k_bar(vel_x_index) = 0.0f;
    //     xhat_k(vel_x_index) = 0.0f;
    //     xhat_k_bar(vel_y_index) = 0.0f;
    //     xhat_k(vel_y_index) = 0.0f;
    // }
    xhat_k_bar(vel_x_index) =
        xhat_k(vel_x_index) = NBMath::clip(xhat_k(vel_x_index),
                                           -70,
                                           70);
    xhat_k_bar(vel_y_index) =
        xhat_k(vel_y_index) = NBMath::clip(xhat_k(vel_y_index),
                                           -70,
                                           70);
    xhat_k_bar(acc_x_index) =
        xhat_k(acc_x_index) = NBMath::clip(xhat_k(acc_x_index),
                                           -150,
                                           150);
    xhat_k_bar(acc_y_index) =
        xhat_k(acc_y_index) = NBMath::clip(xhat_k(acc_y_index),
                                           -150,
                                           150);
    // if(xhat_k(vel_x_index) > VELOCITY_EST_MAX) {
    //     xhat_k_bar(vel_x_index) = VELOCITY_EST_MAX;
    //     xhat_k(vel_x_index) = VELOCITY_EST_MAX;
    // }
    // if(xhat_k(vel_x_index) < VELOCITY_EST_MIN) {
    //     xhat_k_bar(vel_x_index) = VELOCITY_EST_MIN;
    //     xhat_k(vel_x_index) = VELOCITY_EST_MIN;
    // }
    // if(xhat_k(vel_y_index) > VELOCITY_EST_MAX) {
    //     xhat_k_bar(vel_y_index) = VELOCITY_EST_MAX;
    //     xhat_k(vel_y_index) = VELOCITY_EST_MAX;
    // }
    // if(xhat_k(vel_y_index) < VELOCITY_EST_MIN) {
    //     xhat_k_bar(vel_y_index) = VELOCITY_EST_MIN;
    //     xhat_k(vel_y_index) = VELOCITY_EST_MIN;
    // }

    // if(std::abs(xhat_k(vel_x_index)) < VELOCITY_EST_MIN_SPEED) {
    //     xhat_k_bar(vel_x_index) = 0.0f;
    //     xhat_k(vel_x_index) = 0.0f;
    // }
    // if(std::abs(xhat_k(vel_y_index)) < VELOCITY_EST_MIN_SPEED) {
    //     xhat_k_bar(vel_y_index) = 0.0f;
    //     xhat_k(vel_y_index) = 0.0f;
    // }
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


/**
 * Transform relative positions and velocities to global
 */
const float BallEKF::getGlobalX() const
{
    return xhat_k(x_index) * cos(robotPose.h) -
        xhat_k(y_index) * sin(robotPose.h) + robotPose.x;
}
const float BallEKF::getGlobalY() const
{
    return xhat_k(x_index) * sin(robotPose.h) +
        xhat_k(y_index) * cos(robotPose.h) + robotPose.y;
}
const float BallEKF::getGlobalXVelocity() const
{
    return xhat_k(vel_x_index) * cos(robotPose.h) -
        xhat_k(vel_y_index) * sin(robotPose.h);
}
const float BallEKF::getGlobalYVelocity() const
{
    return xhat_k(vel_x_index) * sin(robotPose.h) +
        xhat_k(vel_y_index) * cos(robotPose.h);
}

/**
 * Global uncertainties
 */
const float BallEKF::getGlobalXUncert() const
{
    return P_k(x_index,x_index) * cos(robotPose.h) +
        P_k(y_index,y_index) * sin(robotPose.h);
}
const float BallEKF::getGlobalYUncert() const
{
    return -P_k(x_index,x_index) * sin(robotPose.h) +
        P_k(y_index,y_index) * cos(robotPose.h);
}
const float BallEKF::getGlobalXVelocityUncert() const
{
    return P_k(vel_x_index,vel_x_index) * cos(robotPose.h) +
        P_k(vel_y_index,vel_y_index) * sin(robotPose.h);
}
const float BallEKF::getGlobalYVelocityUncert() const
{
    return -P_k(vel_x_index,vel_x_index) * sin(robotPose.h) +
        P_k(vel_y_index,vel_y_index) * cos(robotPose.h);
}

// /**
//  * Transform relative positions and velocities to global
//  */
// const float BallEKF::setGlobalX(float v)
// {
//     return xhat_k(x_index) * cos(robotPose.h) +
//         xhat_k(y_index) * sin(robotPose.h) + robotPose.x;
// }
// const float BallEKF::setGlobalY(float v)
// {
//     return -xhat_k(x_index) * sin(robotPose.h) +
//         xhat_k(y_index) * cos(robotPose.h) + robotPose.y;
// }
// const float BallEKF::setGlobalXVelocity(float v)
// {
//     return xhat_k(vel_x_index) * cos(robotPose.h) +
//         xhat_k(vel_y_index) * sin(robotPose.h);
// }
// const float BallEKF::setGlobalYVelocity(float v)
// {
//     return -xhat_k(vel_x_index) * sin(robotPose.h) +
//         xhat_k(vel_y_index) * cos(robotPose.h);
// }

// /**
//  * Global uncertainties
//  */
// const float BallEKF::setGlobalXUncert(float v)
// {
//     return P_k(x_index,x_index) * cos(robotPose.h) +
//         P_k(y_index,y_index) * sin(robotPose.h);
// }
// const float BallEKF::setGlobalYUncert(float v)
// {
//     return -P_k(x_index,x_index) * sin(robotPose.h) +
//         P_k(y_index,y_index) * cos(robotPose.h);
// }
// const float BallEKF::setGlobalXVelocityUncert(float v)
// {
//     return P_k(vel_x_index,vel_x_index) * cos(robotPose.h) +
//         P_k(vel_y_index,vel_y_index) * sin(robotPose.h);
// }
// const float BallEKF::setGlobalYVelocityUncert(float v)
// {
//     return -P_k(vel_x_index,vel_x_index) * sin(robotPose.h) +
//         P_k(vel_y_index,vel_y_index) * cos(robotPose.h);
// }
