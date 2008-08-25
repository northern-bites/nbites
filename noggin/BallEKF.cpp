BallEKF::BallEKF() : EKF(BALL_EKF_DIMENSION)
{
}

/**
 * Method incorporate the expected change in ball position from the last
 * frame.  Updates the values of the covariance matrix Q_k and the jacobian
 * A_K.
 *
 * @return The expected change in ball position (x,y, xVelocity, yVelocity)
 */
ublas::vector<double> BallEKF::incorporateTimeUpdate(MotionModel u_k)
{
    // Calculate the assumed change in ball position
    // Assume no decrease in ball velocity
    ublas::vector<double> deltaBall = vector4D(
        deltaBall[0] = getBallXVelocityEst() * (1. / ASSUMED_FPS),
        deltaBall[1] = getBallYVelocityEst() * (1. / ASSUMED_FPS),
        deltaBall[2] = 0,
        deltaBall[3] = 0);

    // Update the jacobian A_k

    // Update the input noise covariance matrix

    return deltaBall;
}
