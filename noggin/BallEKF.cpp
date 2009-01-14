#include "BallEKF.h"

BallEKF::BallEKF(float initX = INIT_BALL_X, float initY = INIT_BALL_Y,
                 float initVelX = INIT_BALL_X_VEL,
                 float initVelY = INIT_BALL_Y_VEL,
                 float initXUncert = INIT_X_UNCERT,
                 float initYUncert = INIT_Y_UNCERT,
                 float initVelXUncert = INIT_X_VEL_UNCERT,
                 float initVelYUncert = INIT_Y_VEL_UNCERT)
    : EKF(BALL_EKF_DIMENSION)
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

    // Assign associated measurement error
    R_k(0,0) = z.distanceSD;
    R_k(1,1) = z.bearingSD;
    return v_k;
}
