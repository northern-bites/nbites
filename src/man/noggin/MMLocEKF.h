/**
 * Multiple model Extend Kalman Filter class for robot self localization.
 *
 */

#ifndef MMLocEKF_h_DEFINED
#define MMLocEKF_h_DEFINED

#include "LocEKF.h"
#include <boost/shared_ptr.hpp>
#include <list>

class MMLocEKF : public LocSystem
{


public:                         // Public interface
    MMLocEKF();
    virtual ~MMLocEKF();

    virtual void updateLocalization(MotionModel u, std::vector<Observation> Z);

private:                        // Private methods
    void initModels();
    void destroyModels();

    void timeUpdate(MotionModel u);
    bool correctionStep(std::vector<Observation>& Z);

    bool applyUnambiguousObservations(std::vector<Observation>& Z);
    bool applyAmbiguousObservations(const std::vector<Observation>& Z);
    void applyObsToActiveModels(const Observation& Z);
    void applyNoCorrectionStep();

    void splitObservation(const Observation& obs, LocEKF * model);
    void consolidateModels(int maxAfterMerge);

    void mergeModels(double mergeThreshold);

    void endFrame();
    void normalizeProbabilities(const std::list<LocEKF*>& unnormalized,
                                double totalProb);

    void setAllModelsInactive();
    void equalizeProbabilities();
    bool mergeable(double mergeThreshold, LocEKF* one, LocEKF* two);


private:                        // Private variables

    const static int MAX_MODELS = 30;

    LocEKF* models[MAX_MODELS];
    std::list<LocEKF*> modelList;

    int mostLikelyModel;
    int numActive, numFree;

    inline const int getMostLikelyModel() const;
    inline LocEKF * getInactiveModel() const;
    inline void deactivateModel(LocEKF * model);
    inline void activateModel(LocEKF * model);

    MotionModel lastOdo;
    std::vector<Observation> lastObservations;

    const static double PROB_SUM = 1.0;
    const static double MERGE_THRESH_INIT = 0.01f;
    const static double MERGE_THRESH_STEP = 0.05f;
    const static int MAX_ACTIVE_MODELS = 6;
    const static double OUTLIER_PROB_LIMIT = 0.005;

public:
    // LocSystem virtual getters
    virtual const PoseEst getCurrentEstimate() const;
    virtual const PoseEst getCurrentUncertainty() const;
    virtual const float getXEst() const;
    virtual const float getYEst() const;
    virtual const float getHEst() const;
    virtual const float getHEstDeg() const;
    virtual const float getXUncert() const;
    virtual const float getYUncert() const;
    virtual const float getHUncert() const;
    virtual const float getHUncertDeg() const;

    virtual const MotionModel getLastOdo() const {
        return lastOdo;
    }

    const std::list<LocEKF*>  getModels() const;

    virtual const std::vector<Observation> getLastObservations() const {
        return lastObservations;
    }

    virtual void blueGoalieReset();
    virtual void redGoalieReset();
    virtual void reset();

    // LocSystem virtual setters
    virtual void setXEst(float xEst){}
    virtual void setYEst(float yEst){}
    virtual void setHEst(float hEst){}
    virtual void setXUncert(float uncertX){}
    virtual void setYUncert(float uncertY){}
    virtual void setHUncert(float uncertH){}

};

#endif //MMLocEKF_h_DEFINED
