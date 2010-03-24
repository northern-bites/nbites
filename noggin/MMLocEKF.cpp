
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

		models[i]->setProbability(0.0);

		modelList.push_back(models[i]);
	}
	models[0]->activate();
	models[0]->setProbability(1.0);

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

	bool hasAppliedACorrection = correctionStep(Z);

	if (!hasAppliedACorrection)
		applyNoCorrectionStep();

	//consolidateModels();

	endFrame();

	int numActive = 0;
	for (int i=0; i<MAX_MODELS; ++i){
		if (models[i]->isActive())
			numActive++;
	}
	cout << numActive << " models active" << endl;

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

bool MMLocEKF::correctionStep(vector<Observation>& Z)
{

	bool appliedUnambiguous = applyUnambiguousObservations(Z);
	bool appliedAmbiguous = applyAmbiguousObservations(Z);
	return appliedUnambiguous || appliedAmbiguous;
}

bool MMLocEKF::applyUnambiguousObservations(vector<Observation>& Z)
{
	bool appliedObs = false;
	vector<Observation>::iterator obs = Z.begin();
	while (obs != Z.end()){
		if (!obs->isAmbiguous()){
			applyObsToActiveModels(*obs);
			obs = Z.erase(obs);
			appliedObs = true;
		} else {
			++obs;
		}
	}
	return appliedObs;
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

bool MMLocEKF::applyAmbiguousObservations(const vector<Observation>& Z)
{
	bool applied = false;
	if (!Z.empty())
		applied = true;
	for (int i=0; i < MAX_MODELS ; ++i){
		vector<Observation>::const_iterator obs;
		for (obs = Z.begin(); obs != Z.end() ; ++obs){
			splitObservation(*obs, models[i]);
		}
	}
	return applied;
}

void MMLocEKF::splitObservation(const Observation& obs, LocEKF * model)
{
	const double originalProb = model->getProbability();
	list<LocEKF*> splitModels;

	for (unsigned int i=0; i < obs.getNumPossibilities(); ++i){
		LocEKF * inactiveModel = getInactiveModel();
		inactiveModel->copyEKF(*model);

		Observation newObs(obs);
		if (obs.isLine())
			newObs.setLinePossibility(obs.getLinePossibilities()[i] );
		else
			newObs.setPointPossibility(obs.getPointPossibilities()[i] );

		inactiveModel->applyObservation(newObs);
		inactiveModel->activate();
		splitModels.push_back(inactiveModel);

		normalizeProbabilities(splitModels, originalProb);
	}
	model->deactivate();
}

void MMLocEKF::endFrame()
{
	for (int i=0; i < MAX_MODELS; ++i){
		if (models[i]->isActive()){
			models[i]->endFrame();
		}
	}
	mergeModels();
	normalizeProbabilities(modelList, PROB_SUM);
}

void MMLocEKF::normalizeProbabilities(const list<LocEKF*>& unnormalized,
									  double totalProb)
{
	// Normalize the Probabilities so that they all sum to totalProb
    double sumAlpha=0.0;
	list<LocEKF*>::const_iterator model = unnormalized.begin();
    for ( ; model != unnormalized.end() ; model++) {
        if ((*model)->isActive()) {
            sumAlpha += (*model)->getProbability();
        }
    }

    if(sumAlpha == 1) return;

    if (sumAlpha == 0) sumAlpha = 1e-12;

	model = unnormalized.begin();
    for ( ; model != unnormalized.end() ; model++) {
        if ((*model)->isActive()) {
            (*model)->setProbability((*model)->getProbability()/sumAlpha);
        }
    }

}

void MMLocEKF::mergeModels()
{
	for (int i=0; i < MAX_MODELS; ++i) {
		for (int j=0; j < MAX_MODELS; ++j){
			if (mergeable(models[i], models[j])){
				models[i]->mergeEKF(*models[j]);
				models[j]->deactivate();
			}
		}
	}
}

bool MMLocEKF::mergeable(LocEKF* one, LocEKF* two)
{
	if (!one->isActive() || !two->isActive() || one == two)
		return false;
	const LocEKF::StateVector diff = one->getState() - two->getState();

	LocEKF::StateMatrix oneUncert = one->getStateUncertainty();
	LocEKF::StateMatrix twoUncert = two->getStateUncertainty();

	LocEKF::StateMatrix uncertSum = (oneUncert * one->getProbability() +
									 twoUncert * two->getProbability());


	double denom = (-uncertSum(0,1) * uncertSum(1,0) +
					uncertSum(0,0) * uncertSum(1,1));

	if (denom < 0.0001)
		denom = 1e-08;

	if (isnan(denom) || isnan(one->getProbability()) || isnan(two->getProbability()))
		return false;

	LocEKF::StateMatrix uncertSumInv = uncertSum;

	uncertSumInv(0,0) = uncertSum(1,1)/denom;
	uncertSumInv(0,1) = 0;
	uncertSumInv(1,0) = 0;
	uncertSumInv(1,1) = uncertSum(0,0)/denom;

	if (isnan(uncertSumInv(0,0))) cout << "found a nan 00" << endl;
	if (isnan(uncertSumInv(0,1))) cout << "found a nan 01" << endl;
	if (isnan(uncertSumInv(1,0))) cout << "found a nan 10" << endl;
	if (isnan(uncertSumInv(1,1))) cout << "found a nan 11" << endl;

	double metric = (one->getProbability() * two->getProbability()) *
		inner_prod(trans(diff), prod(uncertSumInv, diff));

	if (metric < 0.8){
		return true;
	} else {
		cout << "metric is " << metric << endl;
		return false;
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
		if (models[i]->getProbability() > max &&
			models[i]->isActive()){
			max = models[i]->getProbability();
			index = i;
		}
	}

	return index;
}

LocEKF * MMLocEKF::getInactiveModel() const
{
	for (int i=0; i < MAX_MODELS; ++i)
		if (!models[i]->isActive())
			return models[i];
	return models[MAX_MODELS -1];
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
		if (models[i]->isActive())
			numActive++;
	}

	const double newProbability = 1.0/numActive;
	for (int i=0; i < MAX_MODELS ; ++i){
		if (models[i]->isActive())
			models[i]->setProbability(newProbability);
	}
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
