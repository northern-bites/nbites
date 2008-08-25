// Internal constants


BallEKF::BallEKF() : EKF(BALL_EKF_DIMENSION)
{
    ublas::matrix<double> A_K(BALL_EKF_DIMENSION, BALL_EKF_DIMENSION);
    // ones on the diagonal
    A_K(0,0) = 1.;
    A_K(1,1) = 1.;
    A_K(2,2) = 1.;
    A_K(3,3) = 1.;

    // Assummed change in position necessary for velocity to work correctly
    A_K(0,2) = 1. / ASSUMED_FPS;
    A_K(1,3) = 1. / ASSUMED_FPS;
    const ublas::vector<double> BallEKF::BALL_TIME_UPDATE_JACOBIAN = A_K;
}

/**
 * Method incorporate the expected change in ball position from the last
 * frame.  Updates the values of the covariance matrix Q_k and the jacobian
 * A_K.
 *
 * @param u The motion model of the last frame.  Ignored for the ball.
 * @param Q the input covariance matrix
 * @param A the Jacobian associated with the update model
 * @return The expected change in ball position (x,y, xVelocity, yVelocity)
 */
ublas::vector<double> BallEKF::associateTimeUpdate(MotionModel u,
                                                   ublas::matrix<double> *A,
                                                   ublas::matrix<double> *Q);
{
    // Calculate the assumed change in ball position
    // Assume no decrease in ball velocity
    ublas::vector<double> deltaBall(BALL_EKF_DIMENSION);
    deltaBall(0) = getBallXVelocityEst() * (1. / ASSUMED_FPS);
    deltaBall(1) = getBallYVelocityEst() * (1. / ASSUMED_FPS);
    deltaBall(2) = 0;
    deltaBall(3) = 0;

    // Update the jacobian A_k
    A = &BALL_TIME_UPDATE_JACOBIAN;

    // Update the input noise covariance matrix
    Q = 
    return deltaBall;
}
