/**
 * BallEKF.h - Header file for the BallEKF class
 *
 * @author Tucker Hermans
 */

#ifndef BallEKF_h_DEFINED
#define BallEKF_h_DEFINED
#include <boost/shared_ptr.hpp>

#include "EKF.h"
#include "VisualBall.h"
#include "EKFStructs.h"
#include "NBMath.h"
#include "NogginStructs.h"

/**
 * @brief Class for tracking of ball position and velocity.  Extends the abstract
 * EKF class.
 */
class BallEKF : public ekf::EKF<RangeBearingMeasurement,
                                MotionModel,
                                ekf::ball_ekf_dimension,
                                ekf::dist_bearing_meas_dim>
{
public:
    // Constructors & Destructors
    BallEKF();
    BallEKF(float initX, float initY,
            float initVelX, float initVelY,
            float initXUncert, float initYUncert,
            float initVelXUncert, float initVelYUncert);
    virtual ~BallEKF() {}

    // Update functions
    void updateModel(RangeBearingMeasurement  ball, PoseEst p);
    void reset();
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
     * @param val The new estimate of the ball x position
     */
    void setXEst(float val) { xhat_k(0) = val; }

    /**
     * @param val The new estimate of the ball y position
     */
    void setYEst(float val) { xhat_k(1) = val; }

    /**
     * @param val The new estimate of the ball x velocity
     */
    void setXVelocityEst(float val) { xhat_k(2) = val; }

    /**
     * @param val The new estimate of the ball y velocity
     */
    void setYVelocityEst(float val) { xhat_k(3) = val; }

    /**
     * @param val The new uncertainty for ball x position
     */
    void setXUncert(float val) { P_k(0,0) = val; }

    /**
     * @param val The new uncertainty for ball y position
     */
    void setYUncert(float val) { P_k(1,1) = val; }

    /**
     * @param val The new uncertainty for ball x velocity
     */
    void setXVelocityUncert(float val) { P_k(2,2) = val; }

    /**
     * @param val The new uncertainty for ball y velocity
     */
    void setYVelocityUncert(float val) { P_k(3,3) = val; }

    // Output methods
    friend std::ostream& operator<< (std::ostream &o, const BallEKF &c) {
        return o << "Est: (" << c.getXEst() << ", " << c.getYEst() << ", "
                 << c.getXVelocityEst() << ", " << c.getYVelocityEst() << ")\t"
                 << "Uncert: (" << c.getXUncert() << ", " << c.getYUncert()
                 << ", "
                 << c.getXVelocityUncert() << ", " << c.getYVelocityUncert()
                 << ")";
    }
private:
    // Core Functions
    virtual StateVector associateTimeUpdate(MotionModel u_k);
    virtual void incorporateMeasurement(const RangeBearingMeasurement& z,
                                        StateMeasurementMatrix &H_k,
                                        MeasurementMatrix &R_k,
                                        MeasurementVector &V_k);
    virtual void beforeCorrectionFinish(void);
    void limitAPrioriEst(void);
    void limitPosteriorEst(void);
    void limitAPrioriUncert(void);
    void limitPosteriorUncert(void);
    void clipBallEstimate(void);

    bool useCartesian;
    PoseEst robotPose;
    const static float ASSUMED_FPS;
    const static float BETA_BALL;
    const static float BETA_BALL_VEL;
    const static float GAMMA_BALL;
    const static float GAMMA_BALL_VEL;
    const static float CARPET_FRICTION;
    const static float BALL_DECAY_PERCENT;
    const static float INIT_BALL_X;
    const static float INIT_BALL_Y;
    const static float INIT_BALL_X_VEL;
    const static float INIT_BALL_Y_VEL;
    const static float X_UNCERT_MAX;
    const static float Y_UNCERT_MAX;
    const static float VELOCITY_UNCERT_MAX;
    const static float X_UNCERT_MIN;
    const static float Y_UNCERT_MIN;
    const static float VELOCITY_UNCERT_MIN;
    const static float INIT_X_UNCERT;
    const static float INIT_Y_UNCERT;
    const static float INIT_X_VEL_UNCERT;
    const static float INIT_Y_VEL_UNCERT;
    const static float X_EST_MIN;
    const static float Y_EST_MIN;
    const static float X_EST_MAX;
    const static float Y_EST_MAX;
    const static float VELOCITY_EST_MAX;
    const static float VELOCITY_EST_MIN;
    const static float VELOCITY_EST_MIN_SPEED;
    const static float BALL_JUMP_VEL_THRESH;
    const static float USE_CARTESIAN_BALL_DIST;
};
#endif // File
