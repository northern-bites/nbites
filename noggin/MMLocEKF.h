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

	void timeUpdate();
	void correctionStep();

	void applyUnambiguousObservations();
	void applyAmbiguousObservations();
	void consolidateModels();

	void mergeModels(LocEKF * one, LocEKF * two);


private:						// Private variables
	const static int MAX_MODELS = 30;

	LocEKF* models[MAX_MODELS];

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
    virtual const MotionModel getLastOdo() const;
	virtual const vector<Observation> getLastObservations() const;
    virtual void blueGoalieReset();
    virtual void redGoalieReset();

	// LocSystem virtual setters
    virtual void setXEst(float xEst);
    virtual void setYEst(float yEst);
    virtual void setHEst(float hEst);
    virtual void setXUncert(float uncertX);
    virtual void setYUncert(float uncertY);
    virtual void setHUncert(float uncertH);

};

#endif //MMLocEKF_h_DEFINED
