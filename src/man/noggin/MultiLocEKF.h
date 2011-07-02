/**
 * MultiLocEKF.h - Header file for the MultiLocEKF class
 * We use an Extended Kalman Filter for figuring out where we are on the field
 *
 */

#ifndef MultiLocEKF_h_DEFINED
#define MultiLocEKF_h_DEFINED
#include <boost/shared_ptr.hpp>

#include "MultiMeasurementEKF.h"
#include "EKFStructs.h"
#include "NBMath.h"
#include "NogginStructs.h"
#include "Observation.h"
#include "LocSystem.h"
#include "dsp.h"

// #define DEBUG_LOC_EKF_INPUTS
// #define DEBUG_STANDARD_ERROR
// #define DEBUG_DIVERGENCE_CALCULATIONS
// #define DEBUG_ERROR_LOG


/**
 * @brief Class for tracking of loc position and velocity.  Extends the abstract
 * EKF class.
 */
class MultiLocEKF : public ekf::TwoMeasurementEKF<PointObservation,
                                                  ekf::dist_bearing_meas_dim,
                                                  CornerObservation,
                                                  ekf::corner_measurement_dim,
                                                  MotionModel,
                                                  ekf::loc_ekf_dimension>,
                    public LocSystem
{
    enum {
        max_pt_divergence = 6,
        max_corner_divergence = 8
    };

    enum {
        x_index = 0,
        y_index,
        h_index
    };

    enum {
        error_log_width = 20
    };

public:

    // Constructors & Destructors
    explicit MultiLocEKF(float initX = INIT_LOC_X,
                         float initY = INIT_LOC_Y,
                         float initH = INIT_LOC_H,
                         float initXUncert = INIT_X_UNCERT,
                         float initYUncert = INIT_Y_UNCERT,
                         float initHUncert = INIT_H_UNCERT);

    virtual ~MultiLocEKF() {}

    // Update functions
    virtual void updateLocalization(const MotionModel& u,
                                    const std::vector<PointObservation>& pt_z,
                                    const std::vector<CornerObservation>& c_z);
    void odometryUpdate(const MotionModel& u);

    void applyObservations(std::vector<PointObservation> pt_z,
                           std::vector<CornerObservation> c_z);

    void endFrame();

    void printAfterUpdateInfo();
    void printBeforeUpdateInfo();

    virtual void reset();
    virtual void redGoalieReset();
    virtual void blueGoalieReset();
    virtual inline void resetLocTo(float x, float y, float h);

    bool resetLoc(const std::vector<PointObservation> pt_z,
                  const std::vector<CornerObservation>& c_z);

    bool resetLoc(const std::vector<PointObservation>& z);
    bool resetLoc(const std::vector<CornerObservation>& z);

    void resetLoc(const PointObservation* pt1,
                  const PointObservation* pt2);
    void resetLoc(const CornerObservation* c);

    // Getters
    // Get location informations
    virtual float getXEst() const { return xhat_k(0); }
    virtual float getYEst() const { return xhat_k(1); }
    virtual float getHEst() const { return subPIAngle(xhat_k(2)); }
    virtual float getHEstDeg() const {
        return subPIAngle(xhat_k(2)) * TO_DEG;
    }
    virtual PoseEst getCurrentEstimate() const { return PoseEst(xhat_k(0),
                                                                xhat_k(1),
                                                                xhat_k(2)); }

    // Get location uncertanty informations
    virtual float getXUncert() const { return P_k(0,0); }
    virtual float getYUncert() const { return P_k(1,1); }
    virtual float getHUncert() const { return P_k(2,2); }
    virtual float getHUncertDeg() const { return P_k(2,2) * TO_DEG; }
    virtual PoseEst getCurrentUncertainty() const {
        return PoseEst(P_k(0,0), P_k(1,1), P_k(2,2));
    }

    // Get last odomotery update
    virtual MotionModel getLastOdo() const {
        return lastOdo;
    }

    virtual std::vector<PointObservation> getLastPointObservations() const {
        return lastPointObservations;
    }

    virtual std::vector<CornerObservation> getLastCornerObservations() const {
        return lastCornerObservations;
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
    MultiLocEKF(const MultiLocEKF& other);
    MultiLocEKF operator=(const MultiLocEKF& other);

    // Core Functions
    virtual StateVector associateTimeUpdate(MotionModel u_k);

    virtual void incorporateMeasurement(const PointObservation& z,
                                        StateMeasurementMatrix1 &H_k,
                                        MeasurementMatrix1 &R_k,
                                        MeasurementVector1 &V_k);

    virtual void incorporateMeasurement(const CornerObservation& z,
                                        StateMeasurementMatrix2 &H_k,
                                        MeasurementMatrix2 &R_k,
                                        MeasurementVector2 &V_k);

    void incorporateCartesianMeasurement(int obsIndex,
                                         const PointObservation& z,
                                         StateMeasurementMatrix1 &H_k,
                                         MeasurementMatrix1 &R_k,
                                         MeasurementVector1 &V_k);
    void incorporatePolarMeasurement(int obsIndex,
                                     const PointObservation& z,
                                     StateMeasurementMatrix1 &H_k,
                                     MeasurementMatrix1 &R_k,
                                     MeasurementVector1 &V_k);


    void incorporateCorner(int index,
                           const CornerObservation& z,
                           StateMeasurementMatrix2 &H_k,
                           MeasurementMatrix2 &R_k,
                           MeasurementVector2 &V_k);

    template <class T>
    void removeAmbiguous(std::vector<T>& z){
        typename std::vector<T>::iterator i = z.begin();
        while( i != z.end() ) {
            i->isAmbiguous() ? i = z.erase(i) : ++i;
        }
    }

    /**
     * Check the standard error of a measurement update and signal to
     * the system to not process it the standard error is too large.
     */
    template<typename StateMeasMT , typename MeasVT, typename MeasMT >
    void checkStandardError(const StateMatrix& P_k, const StateMeasMT& H_k,
                            const MeasVT& V_k, MeasMT * R_k){

        // Calculate the standard error of the measurement
        const StateMeasMT newP = prod(P_k, trans(H_k));

        // Really this is the square of standard error
        MeasMT se = prod(H_k, newP) + (*R_k);

        // Since we are only comparing to V_k, we square V_k instead
        // of square rooting se(). This is considerably faster. Also
        // means we use the square of our fraction to compare

        // Ignore observations based on standard error
        // we avoid sqrt of
        if ( se(0,0) *
             STANDARD_ERROR_THRESH *
             STANDARD_ERROR_THRESH < V_k(0) * V_k(0) ) {

#ifdef DEBUG_STANDARD_ERROR
            cout << "\t Ignoring measurement " << endl;
            cout << "\t Standard error is " << se << endl;
            cout << "\t Invariance is " << abs(V_k(0))*5 << endl;
#endif

            // Signal system to not use this observation and that
            // there was an observed error during this frame
            (*R_k)(0,0) = DONT_PROCESS_KEY;
        }

        double pushValue =  ((*R_k)(0,0) == DONT_PROCESS_KEY) ? 1.0f : 0.0f;

        // If we've seen to many erroneous frames, let's reset to be safe
        if (errorLog.X(pushValue) > ERROR_RESET_THRESH &&
            errorLog.Steady()){
            resetFlag = true;
        }

#ifdef DEBUG_ERROR_LOG
        cout << "Error log has value of " << errorLog.Y() << endl;
#endif /* DEBUG_ERROR_LOG */
    }

    /**
     * Given an observation with multiple possibilities, we return the
     * observation with the best possibility as the first element of
     * the vector
     *
     * @param z The observation to be fixed
     *
     * @return The index of the landmark which fits best, or -1 if no
     *         acceptable landmarks are found
     */
    template <class ObsT, class LandmarkT>
    int findBestLandmark(const ObsT& z) {

        // Hack in here for if the vision system cannot identify this
        // observation (no possible identities)
        //
        // @TODO this should never happen, so fix vision system to
        // stop it from happening.
        if (z.getNumPossibilities() == 0){
            return -1;
        } else if (z.getNumPossibilities() == 1){
            return 0;
        } else {
            return findNearestNeighbor<ObsT, LandmarkT>(z);
        }
    }


    /**
     * Find the point closest to our observation.
     *
     * @param The observed point
     *
     * @return Index of the nearest neighbor or -1 if none are
     *         acceptably close to the observation
     */
    template <class ObsT, class LandmarkT>
    int findNearestNeighbor(const ObsT& z){

        std::vector<LandmarkT> possiblePoints = z.getPossibilities();
        float minDivergence = getAcceptableDivergence<LandmarkT>();
        int minIndex = -1;

#ifdef DEBUG_DIVERGENCE_CALCULATIONS
        std::cout << std::endl
                  << "Calculating Divergence from " << z << std::endl;
#endif

        for (unsigned int i = 0; i < possiblePoints.size(); ++i) {
            float divergence = getDivergence(z, possiblePoints[i]);

            if (divergence < minDivergence) {
                minDivergence = divergence;
                minIndex = i;
            }

#ifdef DEBUG_DIVERGENCE_CALCULATIONS
            std::cout << "\tTo Landmark " << possiblePoints[i] << std::endl;
            std::cout << "\t\tdivergence =  " << divergence << std::endl;
#endif
        }
        return minIndex;
    }

    float getDivergence(const PointObservation& z, const PointLandmark& pt);
    float getDivergence(const CornerObservation& z, const CornerLandmark& pt);

    // Generic template
    template<class LandmarkT>
    static inline float getAcceptableDivergence() { return 100; } // arbitrary

    virtual void beforeCorrectionFinish(){ }

    void limitAPrioriUncert();
    void limitPosteriorUncert();
    void clipRobotPose();

    // Last odometry update
    MotionModel lastOdo;
    std::vector<PointObservation> lastPointObservations;
    std::vector<CornerObservation> lastCornerObservations;
    bool useAmbiguous;

    /**
     * Members responsible for keeping running totals of recent
     * erroneous (too divergent) observations.
     */
    Boxcar errorLog;
    bool resetFlag;

    // Fraction of frames with an erroneous observation
    const static float ERROR_RESET_THRESH;
    const static float STANDARD_ERROR_THRESH;

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
    const static float INIT_BLUE_GOALIE_LOC_X;
    const static float INIT_BLUE_GOALIE_LOC_Y;
    const static float INIT_BLUE_GOALIE_LOC_H;
    const static float INIT_RED_GOALIE_LOC_X;
    const static float INIT_RED_GOALIE_LOC_Y;
    const static float INIT_RED_GOALIE_LOC_H;
    const static float X_UNCERT_MAX;
    const static float Y_UNCERT_MAX;
    const static float H_UNCERT_MAX;
    const static float X_UNCERT_MIN;
    const static float Y_UNCERT_MIN;
    const static float H_UNCERT_MIN;
    const static float PRETTY_SURE_X_UNCERT;
    const static float PRETTY_SURE_Y_UNCERT;
    const static float INIT_X_UNCERT;
    const static float INIT_Y_UNCERT;
    const static float INIT_H_UNCERT;
    const static float X_EST_MIN;
    const static float Y_EST_MIN;
    const static float H_EST_MIN;
    const static float X_EST_MAX;
    const static float Y_EST_MAX;
    const static float H_EST_MAX;
};
#endif // File
