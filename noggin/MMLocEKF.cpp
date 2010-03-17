
#include "MMLocEKF.h"


MMLocEKF::MMLocEKF()
{
	initModels();
}

MMLocEKF::~MMLocEKF()
{
	destroyModels();
}

/**
 * Create the fixed models for the filter
 */
void MMLocEKF::initModels()
{
	for (int i = 0; i < MAX_MODELS; ++i){
		models[i] = new LocEKF();
		models[i]->deactivate();

		probabilities[i] = 0.0;
	}
	models[0]->activate();
	probabilities[0] = 1.0;
}

/**
 * Delete all the models on the heap.
 */
void MMLocEKF::destroyModels()
{
	for (int i = 0; i < MAX_MODELS; ++i){
		delete models[i];
	}
}

void MMLocEKF::updateLocalization(MotionModel u, std::vector<Observation> Z)
{
	// Apply time update
	timeUpdate(u);

	correctionStep(Z);

	consolidateModels();

	endFrame();

	lastObservations = Z;
	lastOdo = u;
}

void MMLocEKF::timeUpdate(MotionModel u)
{
	for (int i=0; i < MAX_MODELS; ++i)
		if ( models[i]->isActive() ){
			models[i]->odometryUpdate(u);
		}
}

void MMLocEKF::correctionStep(vector<Observation>& Z)
{
	if (Z.size() > 0){
		applyUnambiguousObservations(Z);
		//applyAmbiguousObservations(Z);
	} else {
		applyNoCorrectionStep();
	}
}

void MMLocEKF::applyUnambiguousObservations(vector<Observation>& Z)
{
	vector<Observation>::iterator obs = Z.begin();
	while (obs != Z.end()){
		if (!obs->isAmbiguous()){
			applyObsToActiveModels(*obs);
			obs = Z.erase(obs);
		} else {
			++obs;
		}
	}
}

void MMLocEKF::applyObsToActiveModels(Observation& obs)
{
	for (int i=0; i < MAX_MODELS; ++i){
		if (models[i]->isActive()){
			models[i]->applyObservation(obs);
		}
	}
}

void MMLocEKF::applyNoCorrectionStep()
{
	for (int i=0; i < MAX_MODELS; ++i){
		if (models[i]->isActive()){
			models[i]->noCorrectionStep();
		}
	}
}

void MMLocEKF::applyAmbiguousObservations(vector<Observation>& Z)
{
	// Renormalize probabilities after split
}

void MMLocEKF::endFrame(){
	for (int i=0; i < MAX_MODELS; ++i){
		if (models[i]->isActive()){
			models[i]->endFrame();
		}
	}
}


/****************** Getters *****************************/
const PoseEst MMLocEKF::getCurrentEstimate() const
{
	return models[getMostLikelyModel()]->getCurrentEstimate();
}

const PoseEst MMLocEKF::getCurrentUncertainty() const
{
	return models[getMostLikelyModel()]->getCurrentUncertainty();
}

const float MMLocEKF::getXEst() const
{
	return models[getMostLikelyModel()]->getXEst();
}

const float MMLocEKF::getYEst() const
{
	return models[getMostLikelyModel()]->getYEst();
}

const float MMLocEKF::getHEst() const
{
	return models[getMostLikelyModel()]->getHEst();
}

const float MMLocEKF::getHEstDeg() const
{
	return models[getMostLikelyModel()]->getHEstDeg();
}

const float MMLocEKF::getXUncert() const
{
	return models[getMostLikelyModel()]->getXUncert();
}

const float MMLocEKF::getYUncert() const
{
	return models[getMostLikelyModel()]->getYUncert();
}

const float MMLocEKF::getHUncert() const
{
	return models[getMostLikelyModel()]->getHUncert();
}

const float MMLocEKF::getHUncertDeg() const
{
	return models[getMostLikelyModel()]->getHUncertDeg();
}

const int MMLocEKF::getMostLikelyModel() const
{
	double max = -1.0;
	int index = 0;
	for (int i=0; i<MAX_MODELS; ++i){
		if (probabilities[i] > max){
			max = probabilities[i];
			index = i;
		}
	}
	return index;
}

/************** PRIVATE HELPERS ***********/
void MMLocEKF::setAllModelsInactive()
{
	for (int i=0; i < MAX_MODELS; ++i)
		models[i]->deactivate();
}

void MMLocEKF::equalizeProbabilities()
{
	int numActive = 0;
	for (int i=0; i < MAX_MODELS ; ++i) {
		if (isModelActive(i))
			numActive++;
	}

	const double newProbability = 1.0/numActive;
	for (int i=0; i < MAX_MODELS ; ++i){
		if (isModelActive(i))
			setProbability(i,newProbability);
	}
}

void MMLocEKF::setProbability(int i, double prob)
{
	probabilities[i] = prob;
}

bool MMLocEKF::isModelActive(int i) const
{
	return models[i]->isActive();
}


/**************** RESETS *******************/
void MMLocEKF::blueGoalieReset()
{
	setAllModelsInactive();
	models[0]->activate();
	equalizeProbabilities();
	models[0]->blueGoalieReset();
}

void MMLocEKF::redGoalieReset()
{
	setAllModelsInactive();
	models[0]->activate();
	equalizeProbabilities();
	models[0]->redGoalieReset();
}

void MMLocEKF::reset()
{
	setAllModelsInactive();
	models[0]->activate();
	equalizeProbabilities();
	models[0]->reset();

}
