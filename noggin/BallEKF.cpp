#include "BallEKF.h"

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
BallEKF::BallEKF(MCL * _mcl,
                 float initX = INIT_BALL_X, float initY = INIT_BALL_Y,
                 float initVelX = INIT_BALL_X_VEL,
                 float initVelY = INIT_BALL_Y_VEL,
                 float initXUncert = INIT_X_UNCERT,
                 float initYUncert = INIT_Y_UNCERT,
                 float initVelXUncert = INIT_X_VEL_UNCERT,
                 float initVelYUncert = INIT_Y_VEL_UNCERT)
    : EKF(BALL_EKF_DIMENSION, BETA_BALL, GAMMA_BALL), robotLoc(_mcl)
{
    // ones on the diagonal
    A_k(0,0) = 1.0f;
    A_k(1,1) = 1.0f;
    A_k(2,2) = 1.0f;
    A_k(3,3) = 1.0f;

    // Assummed change in position necessary for velocity to work correctly
    A_k(0,2) = 1. / ASSUMED_FPS;
    A_k(1,3) = 1. / ASSUMED_FPS;
}

/**
 * Method to deal with updating the entire ball model
 * @param ball the ball seen this frame.
 */
void BallEKF::updateModel(Ball ball)
{
    // Update expected ball movement
    timeUpdate(MotionModel());

    // We've seen a ball
    if (ball.getDist() > 0) {
        sawBall(ball);
        // } else if (TEAMMATE BALL REPORT) { // A teammate has seen a ball
    } else { // No ball seen
        setXVelocityEst(getXVelocityEst() * (1.0f - BALL_DECAY_PERCENT));
        setYVelocityEst(getYVelocityEst() * (1.0f - BALL_DECAY_PERCENT));
        noCorrectionStep();
    }
}

/**
 * Method to deal with sighting of a ball by the robot
 * @param ball a copy of the ball seen by the robot
 */
void BallEKF::sawBall(Ball ball)
{
    Measurement m;
    m.distance = ball.getDist();
    m.bearing = ball.getBearing();
    m.distanceSD = ball.getDistanceSD();
    m.bearingSD = ball.getBearingSD();
    std::vector<Measurement> z;
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
ublas::vector<float> BallEKF::associateTimeUpdate(MotionModel u)
{
    // Calculate the assumed change in ball position
    // Assume no decrease in ball velocity
    ublas::vector<float> deltaBall(BALL_EKF_DIMENSION);
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
void BallEKF::incorporateMeasurement(Measurement z,
                                     ublas::matrix<float> &H_k,
                                     ublas::matrix<float> &R_k,
                                     ublas::vector<float> &V_k)
{
    // Convert our siting to cartesian coordinates
    float x_b_r = z.distance * cos(z.bearing + QUART_CIRC_RAD);
    float y_b_r = z.distance * sin(z.bearing + QUART_CIRC_RAD);
    ublas::vector<float> z_x(2);
    z_x(0) = x_b_r;
    z_x(1) = y_b_r;

    // Get expected values of ball
    float h = robotLoc->getHEst();
    float x = robotLoc->getXEst();
    float y = robotLoc->getYEst();
    float x_b = getXEst();
    float y_b = getYEst();
    ublas::vector<float> d_x(2);
    d_x(0) = (x_b - x)*cos(-h) - (y_b - y)*sin(-h);
    d_x(1) = (x_b - x)*sin(-h) + (y_b - y)*cos(-h);

    // Calculate invariance
    V_k = z_x - d_x;

    // Calculate jacobians
    H_k(0,0) = cos(h);
    H_k(0,1) = -sin(h);
    H_k(1,0) = sin(h);
    H_k(1,1) = cos(h);

    // Update the measurement covariance matrix
    R_k(0,0) = z.distanceSD;
    R_k(1,1) = z.bearingSD;
}
