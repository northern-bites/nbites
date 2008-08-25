/**
 * BallEKF.h - Header file for the BallEKF class
 *
 * @author Tucker Hermans
 */

#ifndef BallEKF_h_DEFINED
#define BallEKF_h_DEFINED

#include "EKF.h"

// Parameters
#define ASSUMED_FPS 30.0
#define BETA_BALL 5. // How much uncertainty naturally grows per update
#define GAMMA_BALL .1 // How much ball velocity should effect uncertainty
#define BALL_EKF_DIMENSION 4 // Number of states in Ball EKF

/**
 * Class for tracking of ball position and velocity.  Extends the abstract
 * EKF class.
 */
class BallEKF : public EKF
{
public:
    // Constructors & Destructors
    BallEKF(float initX, float initY,
            float initVelX, float initVelY,
            float initXUncert,float initYUncert,
            float initVelXUncert, float initVelYUncert);

    virtual ~BallEKF();

    // Core functions
    //virtual void timeUpdate(MotionModel u_t);

    // Getters

    /**
     * @return The current estimate of the ball x position
     */
    getXEst() { return xhat_k[0]; }

    /**
     * @return The current estimate of the ball y position
     */
    getYEst() { return xhat_k[1]; }

    /**
     * @return The current estimate of the ball x velocity
     */
    getXVelocityEst() { return xhat_k[2]; }

    /**
     * @return The current estimate of the ball y velocity
     */
    getYVelocityEst() { return xhat_k[3]; }

    /**
     * @return The current uncertainty for ball x position
     */
    getXUncert() { return P_k[0][0]; }

    /**
     * @return The current uncertainty for ball y position
     */
    getYUncert() { return P_k[1][1]; }

    /**
     * @return The current uncertainty for ball x velocity
     */
    getXVelocityUncert() { return P_k[2][2]; }

    /**
     * @return The current uncertainty for ball y velocity
     */
    getYVelocityUncert() { return P_k[3][3]; }

private:
    ublas::vector<double> incorporateTimeUpdate(MotionModel u_k);
}
#endif // File
