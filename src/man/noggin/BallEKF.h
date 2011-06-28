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
 * @brief Class for tracking of ball position and velocity. Tracks
 * the ball in a totally relative frame of reference. Robot pose
 * estimate (from localization) is used only when a global ball
 * position is required, as it may be for communicating with other
 * robots, for example.
 */
class BallEKF : public ekf::EKF<RangeBearingMeasurement,
                                MotionModel,
                                ekf::ball_ekf_dimension,
                                ekf::ball_ekf_meas_dim>
{
    enum StateIndices {
        x_index = 0,
        y_index,
        vel_x_index,
        vel_y_index,
        acc_x_index,
        acc_y_index
    };

public:
    // Constructors & Destructors
    BallEKF();
    BallEKF(float initX, float initY,
            float initVelX, float initVelY,
            float initXUncert, float initYUncert,
            float initVelXUncert, float initVelYUncert);
    virtual ~BallEKF() {}

    // Update functions
    void updateModel(const MotionModel& odo,
                     const RangeBearingMeasurement& ball,
                     const PoseEst& p);
    void reset();

    ///////////////////////////////
    // Getters
    ///////////////////////////////

    /**
     * Relative ball location getters: ball is in our relative x,y
     *
     * Velocity is in cm/s
     */
    const float getRelativeX() const             { return xhat_k(x_index);     }
    const float getRelativeY() const             { return xhat_k(y_index);     }
    const float getRelativeXVelocity() const     { return xhat_k(vel_x_index); }
    const float getRelativeYVelocity() const     { return xhat_k(vel_y_index); }
    const float getRelativeXAcceleration() const { return xhat_k(acc_x_index); }
    const float getRelativeYAcceleration() const { return xhat_k(acc_y_index); }



    /**
     * Relative uncertainties
     */
    const float getRelativeXUncert() const { return P_k(x_index,x_index); }
    const float getRelativeYUncert() const { return P_k(y_index,y_index); }
    const float getRelativeXVelocityUncert() const {
        return P_k(vel_x_index,vel_x_index);
    }
    const float getRelativeYVelocityUncert() const {
        return P_k(vel_y_index,vel_y_index);
    }
    const float getRelativeXAccelerationUncert() const {
        return P_k(acc_x_index,acc_x_index);
    }
    const float getRelativeYAccelerationUncert() const {
        return P_k(acc_y_index,acc_y_index);
    }

    /**
     * Global ball location and velocities calculated using current
     * robot position estimate
     */
    const float getGlobalX() const;
    const float getGlobalY() const;
    const float getGlobalXVelocity() const;
    const float getGlobalYVelocity() const;
    const float getGlobalXAcceleration() const;
    const float getGlobalYAcceleration() const;

    /**
     * Global uncertainties
     */
    const float getGlobalXUncert() const;
    const float getGlobalYUncert() const;
    const float getGlobalXVelocityUncert() const;
    const float getGlobalYVelocityUncert() const;
    const float getGlobalXAccelerationUncert() const;
    const float getGlobalYAccelerationUncert() const;

    /**
     * Distance, bearing getters
     */
    float getDistance() { return hypotf(getRelativeX(), getRelativeY());      }
    float getBearing()  { return safe_atan2(getRelativeY(), getRelativeX()); }
    float getBearingDeg() {
        return TO_DEG * safe_atan2(getRelativeY(), getRelativeX());
    }

    ///////////////////////////////
    // Setters
    ///////////////////////////////

    /**
     * Set relative position, velocity and uncertainty values
     */
    void setRelativeX               (float val) { xhat_k(x_index)      = val; }
    void setRelativeY               (float val) { xhat_k(y_index)      = val; }

    void setRelativeXVelocity       (float val) { xhat_k(vel_x_index)  = val; }
    void setRelativeYVelocity       (float val) { xhat_k(vel_y_index)  = val; }

    void setRelativeXUncert         (float val) { P_k(x_index,x_index) = val; }
    void setRelativeYUncert         (float val) { P_k(y_index,y_index) = val; }

    void setRelativeXVelocityUncert (float val) { P_k(vel_x_index,
                                                      vel_x_index) = val;     }
    void setRelativeYVelocityUncert (float val) { P_k(vel_y_index,
                                                      vel_y_index) = val;     }

    /**
     * Set global position, velocity and uncertainty values
     */
    void setGlobalX(float val);
    void setGlobalY(float val);

    void setGlobalXVelocity(float val);
    void setGlobalYVelocity(float val);

    void setGlobalXUncert(float val);
    void setGlobalYUncert(float val);

    void setGlobalXVelocityUncert(float val);
    void setGlobalYVelocityUncert(float val);

    // Output methods
    friend std::ostream& operator<< (std::ostream &o, const BallEKF &c) {
        return o << "Est: (" << c.getRelativeX() << ", "
                 << c.getRelativeY() << ", "
                 << c.getRelativeXVelocity() << ", "
                 << c.getRelativeYVelocity() << ")\t"
                 << "Uncert: ("
                 << c.getRelativeXUncert() << ", "
                 << c.getRelativeYUncert() << ", "
                 << c.getRelativeXVelocityUncert() << ", "
                 << c.getRelativeYVelocityUncert() << ")";
    }
private:
    // Core Functions
    virtual StateVector associateTimeUpdate(MotionModel u_k);
    virtual void incorporateMeasurement(const RangeBearingMeasurement& z,
                                        StateMeasurementMatrix &H_k,
                                        MeasurementMatrix &R_k,
                                        MeasurementVector &V_k);

    inline void updateFrameLength();
    inline void updatePosition(const MotionModel& odo,
                               StateVector& deltaBall);
    inline void updateVelocity(const MotionModel& odo,
                               StateVector& deltaBall);
    inline void updateAcceleration(const MotionModel& odo,
                                   StateVector& deltaBall);
    inline void calculateTimeUpdateJacobian(const MotionModel& odo,
                                            StateVector& deltaBall);

    virtual void beforeCorrectionFinish();
    void limitPosteriorEst();
    void limitPosteriorUncert();

    void initMatrices();
    float applyFriction(float vel);
    StateVector transformStateWithOdometry(const StateVector& x,
                                           const MotionModel& odo);
    MeasurementVector
    calculateObservedState(const RangeBearingMeasurement& z,
                           const StateVector& xhat_k_prev);

    static inline float transformToGlobalX(float x, float y, float theta);
    static inline float transformToGlobalY(float x, float y, float theta);

    PoseEst robotPose;
    long long int lastUpdateTime;
    float dt;
    MotionModel curOdo;

    const static float BETA_BALL;
    const static float BETA_BALL_VEL;
    const static float GAMMA_BALL;
    const static float GAMMA_BALL_VEL;
    const static float CARPET_FRICTION;

    const static float INIT_BALL_X, INIT_BALL_Y;
    const static float INIT_BALL_X_VEL, INIT_BALL_Y_VEL;

    const static float INIT_X_UNCERT, INIT_Y_UNCERT;
    const static float INIT_X_VEL_UNCERT, INIT_Y_VEL_UNCERT;

    const static float X_EST_MIN, X_EST_MAX;
    const static float Y_EST_MIN, Y_EST_MAX;

    const static float VELOCITY_EST_MAX, VELOCITY_EST_MIN;
    const static float ACC_EST_MAX, ACC_EST_MIN;

    const static float VELOCITY_EST_MIN_SPEED;
};
#endif // File
