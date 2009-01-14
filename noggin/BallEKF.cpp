#include "BallEKF.h"

BallEKF::BallEKF(MCL _mcl,
                 float initX = INIT_BALL_X, float initY = INIT_BALL_Y,
                 float initVelX = INIT_BALL_X_VEL,
                 float initVelY = INIT_BALL_Y_VEL,
                 float initXUncert = INIT_X_UNCERT,
                 float initYUncert = INIT_Y_UNCERT,
                 float initVelXUncert = INIT_X_VEL_UNCERT,
                 float initVelYUncert = INIT_Y_VEL_UNCERT)
    : EKF(BALL_EKF_DIMENSION), robotLoc(_mcl)
{
    // ones on the diagonal
    A_k(0,0) = 1.0f;
    A_k(1,1) = 1.0f;
    A_k(2,2) = 1.0f;
    A_k(3,3) = 1.0f;

    // Assummed change in position necessary for velocity to work correctly
    A_k(0,2) = 1. / ASSUMED_FPS;
    A_k(1,3) = 1. / ASSUMED_FPS;

    beta = BETA_BALL;
    gamma = GAMMA_BALL;
}


/**
 * Method incorporate the expected change in ball position from the last
 * frame.  Updates the values of the covariance matrix Q_k and the jacobian
 * A_k.
 *
 * @param u The motion model of the last frame.  Ignored for the ball.
 * @param A the Jacobian associated with the update model
 * @param Q the input covariance matrix
 * @return The expected change in ball position (x,y, xVelocity, yVelocity)
 */
ublas::vector<float> BallEKF::associateTimeUpdate(MotionModel u)
{
    // Calculate the assumed change in ball position
    // Assume no decrease in ball velocity
    ublas::vector<float> deltaBall(BALL_EKF_DIMENSION);
    deltaBall(0) = getXVelocityEst() * (1. / ASSUMED_FPS);
    deltaBall(1) = getYVelocityEst() * (1. / ASSUMED_FPS);
    deltaBall(2) = 0;
    deltaBall(3) = 0;

    return deltaBall;
}

ublas::vector<float> BallEKF::incorporateMeasurement(Measurement z,
                                                     ublas::matrix<float> &H_k,
                                                     ublas::matrix<float> &R_k)
{
    ublas::vector<float> v_k(2);
    // Convert our siting to cartesian coordinates
    float x_b_r = z.distance * cos(z.bearing + QUART_CIRC_RAD);
    float y_b_r = z.distance * sin(z.bearing + QUART_CIRC_RAD);
    ublas::vector<float> z_x(2);
    z_x(0) = x_b_r;
    z_x(1) = y_b_r;

    // Get expected values of ball
    float h = -robotLoc.getHEst();
    float x = robotLoc.getXEst();
    float y = robotLoc.getYEst();
    float x_b = getXEst();
    float y_b = getYEst();
    ublas::vector<float> d_x(2);
    d_x(0) = (x_b - x)*cos(h) - (y_b - y)*sin(h);
    d_x(1) = (x_b - x)*sin(h) + (y_b - y)*cos(h);

    // Calculate invariance
    v_k = z_x - d_x;

    // Calculate jacobians
    H_k(0,0) = cos(h);
    H_k(0,1) = -sin(h);
    H_k(1,0) = sin(h);
    H_k(1,1) = cos(h);

    // Update the measurement covariance matrix
    R_k(0,0) = z.distanceSD;
    R_k(1,1) = z.bearingSD;

    return v_k;
}
