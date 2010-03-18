/**
 * Multiple model Extend Kalman Filter class for robot self localization.
 *
 */

#ifndef MMLocEKF_h_DEFINED
#define MMLocEKF_h_DEFINED

#include "LocEKF.h"
#include <boost/shared_ptr.hpp>

class MMLocEKF : public LocSystem
{


public:							// Public interface
	MMLocEKF();
	virtual ~MMLocEKF();

	virtual void updateLocalization(MotionModel u, std::vector<Observation> Z);

private:						// Private methods
	void initModels();
	void destroyModels();

	void timeUpdate(MotionModel u);
	bool correctionStep(std::vector<Observation>& Z);

	bool applyUnambiguousObservations(std::vector<Observation>& Z);
	bool applyAmbiguousObservations(std::vector<Observation>& Z);
	void applyObsToActiveModels(Observation& Z);
	void applyNoCorrectionStep();

	void consolidateModels(){}

	void mergeModels(LocEKF * one, LocEKF * two);

	void endFrame();

	void setAllModelsInactive();
	void equalizeProbabilities();
	inline bool isModelActive(int i) const;
	inline void setProbability(int i, double prob);


private:						// Private variables
	const static int MAX_MODELS = 30;

	LocEKF* models[MAX_MODELS];
	double probabilities[MAX_MODELS];

	int mostLikelyModel;

	inline const int getMostLikelyModel() const;

	MotionModel lastOdo;
	vector<Observation> lastObservations;

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

	virtual const vector<Observation> getLastObservations() const {
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
