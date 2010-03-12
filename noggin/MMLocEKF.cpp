
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
	}
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
