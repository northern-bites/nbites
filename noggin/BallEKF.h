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
    BallEKF(void);
    BallEKF(float initX, float initY,
            float initVelX, float initVelY,
            float initXUncert,float initYUncert,
            float initVelXUncert, float initVelYUncert);

    virtual ~BallEKF();

    // Getters

    /**
     * @return The current estimate of the ball x position
     */
    float getXEst() { return xhat_k[0]; }

    /**
     * @return The current estimate of the ball y position
     */
    float getYEst() { return xhat_k[1]; }

    /**
     * @return The current estimate of the ball x velocity
     */
    float getXVelocityEst() { return xhat_k[2]; }

    /**
     * @return The current estimate of the ball y velocity
     */
    float getYVelocityEst() { return xhat_k[3]; }

    /**
     * @return The current uncertainty for ball x position
     */
    float getXUncert() { return P_k(0,0); }

    /**
     * @return The current uncertainty for ball y position
     */
    float getYUncert() { return P_k(1,1); }

    /**
     * @return The current uncertainty for ball x velocity
     */
    float getXVelocityUncert() { return P_k(2,2); }

    /**
     * @return The current uncertainty for ball y velocity
     */
    float getYVelocityUncert() { return P_k(3,3); }

private:
    // Core Functions
    virtual ublas::vector<float> associateTimeUpdate(MotionModel u_k);
    virtual void incorporateCorrectionMeasurement(Observation z);

public:
    //const ublas::matrix<float> BALL_TIME_UPDATE_JACOBIAN;
};
#endif // File
