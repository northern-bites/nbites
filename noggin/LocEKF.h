/**
 * BallEKF.h - Header file for the BallEKF class
 * All coordinates in C++ are in the local coordinate frame
 * We convert to the global frame when dealing with python
 *
 * @author Tucker Hermans
 */

#ifndef LocEKF_h_DEFINED
#define LocEKF_h_DEFINED
#include <boost/shared_ptr.hpp>

#include "EKF.h"
#include "EKFStructs.h"
#include "NBMath.h"
#include "NogginStructs.h"
#include "Observation.h"
#include "LocSystem.h"

/**
 * Class for tracking of loc position and velocity.  Extends the abstract
 * EKF class.
 */
class LocEKF : public EKF<Observation,
                          MotionModel, LOC_EKF_DIMENSION,
                          LOC_MEASUREMENT_DIMENSION>,
               public LocSystem
{
public:

    // Constructors & Destructors
    LocEKF(float initX = INIT_LOC_X,
           float initY = INIT_LOC_Y,
           float initH = INIT_LOC_H,
           float initXUncert = INIT_X_UNCERT,
           float initYUncert = INIT_Y_UNCERT,
           float initHUncert = INIT_H_UNCERT);
    virtual ~LocEKF() {}

    // Update functions
    virtual void updateLocalization(MotionModel u, std::vector<Observation> Z);
    virtual void reset();

    // Getters
    /**
     * @return The current estimate of the loc x position
     */
    virtual const float getXEst() const { return xhat_k(0); }

    /**
     * @return The current estimate of the loc y position
     */
    virtual const float getYEst() const { return xhat_k(1); }

    /**
     * @return The current estimate of the loc heading
     */
    virtual const float getHEst() const { return subPIAngle(xhat_k(2)); }

    /**
     * @return The current estimate of the loc heading in degrees
     */
    virtual const float getHEstDeg() const {
        return subPIAngle(xhat_k(2)) * TO_DEG; }


    /**
     * @return The current pose estimate of the robot
     */
    virtual const PoseEst getCurrentEstimate() const { return PoseEst(xhat_k(0),
                                                              xhat_k(1),
                                                              xhat_k(2)); }

    /**
     * @return The current uncertainty for loc x position
     */
    virtual const float getXUncert() const { return P_k(0,0); }

    /**
     * @return The current uncertainty for loc y position
     */
    virtual const float getYUncert() const { return P_k(1,1); }

    /**
     * @return The current uncertainty for loc heading
     */
    virtual const float getHUncert() const { return P_k(2,2); }

    /**
     * @return The current uncertainty for loc heading in degrees
     */
    virtual const float getHUncertDeg() const { return P_k(2,2) * TO_DEG; }

    /**
     * @return The current pose estimate of the robot
     */
    virtual const PoseEst getCurrentUncertainty() const {
        return PoseEst(P_k(0,0), P_k(1,1), P_k(2,2));
    }

    virtual const MotionModel getLastOdo() const {
        return lastOdo;
    }
    // Setters
    /**
     * @param val The new estimate of the loc x position
     */
    void setXEst(float val) { xhat_k(0) = val; }

    /**
     * @param val The new estimate of the loc y position
     */
    void setYEst(float val) { xhat_k(1) = val; }

    /**
     * @param val The new estimate of the loc y position
     */
    void setHEst(float val) { xhat_k(2) = val; }

    /**
     * @param val The new uncertainty for loc x position
     */
    void setXUncert(float val) { P_k(0,0) = val; }

    /**
     * @param val The new uncertainty for loc y position
     */
    void setYUncert(float val) { P_k(1,1) = val; }

    /**
     * @param val The new uncertainty for loc h position
     */
    void setHUncert(float val) { P_k(2,2) = val; }

    /**
     * @param _use True if we are to use ambiguous landmark observations
     */
    void setUseAmbiguous(bool _use) { useAmbiguous = _use; }
private:
    // Core Functions
    virtual StateVector associateTimeUpdate(MotionModel u_k);
    virtual void incorporateMeasurement(Observation z,
                                        StateMeasurementMatrix &H_k,
                                        MeasurementMatrix &R_k,
                                        MeasurementVector &V_k);
    int findBestLandmark(Observation * z);
    float getDivergence(Observation * z, PointLandmark pt);

    void limitAPrioriUncert();
    void limitPosteriorUncert();
    void clipRobotPose();

    // Last odometry update
    MotionModel lastOdo;
    bool useAmbiguous;

    // Parameters
    const static float USE_CARTESIAN_DIST;
    const static float BETA_LOC;
    const static float GAMMA_LOC;
    const static float BETA_ROT;
    const static float GAMMA_ROT;

    // Default initialization values
    const static float INIT_LOC_X;
    const static float INIT_LOC_Y;
    const static float INIT_LOC_H;
    const static float X_UNCERT_MAX;
    const static float Y_UNCERT_MAX;
    const static float H_UNCERT_MAX;
    const static float X_UNCERT_MIN;
    const static float Y_UNCERT_MIN;
    const static float H_UNCERT_MIN;
    const static float INIT_X_UNCERT;
    const static float INIT_Y_UNCERT;
    const static float INIT_H_UNCERT;
    const static float X_EST_MIN;
    const static float Y_EST_MIN;
    const static float X_EST_MAX;
    const static float Y_EST_MAX;
};
#endif // File
