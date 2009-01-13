#include "BallEKF.h"

BallEKF::BallEKF() : EKF(BALL_EKF_DIMENSION)
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

BallEKF::BallEKF(float initX, float initY,
                 float initVelX, float initVelY,
                 float initXUncert,float initYUncert,
                 float initVelXUncert, float initVelYUncert)
    : EKF(BALL_EKF_DIMENSION)
{
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

void BallEKF::incorporateCorrectionMeasurement(Observation z)
{
}
