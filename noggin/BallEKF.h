/**
 * BallEKF.h - Header file for the BallEKF class
 *
 * @author Tucker Hermans
 */

#ifndef BallEKF_h_DEFINED
#define BallEKF_h_DEFINED

#include "EKF.h"
#include "MCL.h"
#include "Ball.h"

// Parameters
#define ASSUMED_FPS 30.0
#define BETA_BALL 5. // How much uncertainty naturally grows per update
#define GAMMA_BALL .1 // How much ball velocity should effect uncertainty
#define BALL_EKF_DIMENSION 4 // Number of states in Ball EKF
#define INIT_BALL_X 50.0f
#define INIT_BALL_Y 50.0f
#define INIT_BALL_X_VEL 0.0f
#define INIT_BALL_Y_VEL 0.0f
#define INIT_X_UNCERT 100.0f
#define INIT_Y_UNCERT 100.0f
#define INIT_X_VEL_UNCERT 100.0f
#define INIT_Y_VEL_UNCERT 100.0f
#define BALL_DECAY_PERCENT 0.05
#define CARPET_FRICTION -25 // 25 cm/s^2
/**
 * Class for tracking of ball position and velocity.  Extends the abstract
 * EKF class.
 */
class BallEKF : public EKF
{
public:
    // Constructors & Destructors
    BallEKF(MCL _mcl, float initX, float initY,
            float initVelX, float initVelY,
            float initXUncert,float initYUncert,
            float initVelXUncert, float initVelYUncert);

    virtual ~BallEKF();

    // Update functions
    void updateModel(Ball ball);
    void sawTeammateBall(Measurement m);
    void sawBall(Ball ball);

    // Getters
    /**
     * @return The current estimate of the ball x position
     */
    const float getXEst() const { return xhat_k(0); }

    /**
     * @return The current estimate of the ball y position
     */
    const float getYEst() const { return xhat_k(1); }

    /**
     * @return The current estimate of the ball x velocity
     */
    const float getXVelocityEst() const { return xhat_k(2); }

    /**
     * @return The current estimate of the ball y velocity
     */
    const float getYVelocityEst() const { return xhat_k(3); }

    /**
     * @return The current uncertainty for ball x position
     */
    const float getXUncert() const { return P_k(0,0); }

    /**
     * @return The current uncertainty for ball y position
     */
    const float getYUncert() const { return P_k(1,1); }

    /**
     * @return The current uncertainty for ball x velocity
     */
    const float getXVelocityUncert() const { return P_k(2,2); }

    /**
     * @return The current uncertainty for ball y velocity
     */
    const float getYVelocityUncert() const { return P_k(3,3); }

    // Setters
    /**
     * @return The current estimate of the ball x position
     */
    void setXEst(float val) { xhat_k(0) = val; }

    /**
     * @return The current estimate of the ball y position
     */
    void setYEst(float val) { xhat_k(1) = val; }

    /**
     * @return The current estimate of the ball x velocity
     */
    void setXVelocityEst(float val) { xhat_k(2) = val; }

    /**
     * @return The current estimate of the ball y velocity
     */
    void setYVelocityEst(float val) { xhat_k(3) = val; }

    /**
     * @return The current uncertainty for ball x position
     */
    void setXUncert(float val) { P_k(0,0) = val; }

    /**
     * @return The current uncertainty for ball y position
     */
    void setYUncert(float val) { P_k(1,1) = val; }

    /**
     * @return The current uncertainty for ball x velocity
     */
    void setXVelocityUncert(float val) { P_k(2,2) = val; }

    /**
     * @return The current uncertainty for ball y velocity
     */
    void setYVelocityUncert(float val) { P_k(3,3) = val; }

private:
    // Core Functions
    virtual ublas::vector<float> associateTimeUpdate(MotionModel u_k);
    virtual ublas::vector<float> incorporateMeasurement(Measurement z,
                                                        ublas::
                                                        matrix<float> &H_k,
                                                        ublas::
                                                        matrix<float> &R_k);
    // Members
    MCL robotLoc;
};
#endif // File

float sign(float f)
{
    if (f < 0.0f) {
        return -1.0f;
    } else {
        return 1.0f;
    }
}
