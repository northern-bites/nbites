#include "VisionSystem.h"

/**
 * Constructor
 */
VisionSystem::VisionSystem()
  : PF::SensorModel(), hasNewObs(false)
{ }

/**
 * Update particle weights based on current visual observations.
 * @param particles the particle set to be updated with sensor 
 *                  data.
 * @return the updated particle set.
 */
PF::ParticleSet VisionSystem::update(PF::ParticleSet particles)
{
    std::vector<PF::Observation> obs = currentObservations;
    if(obs.size() == 0)
    {
      //std::cout << "Nothing seen, do not update weights." << std::endl;
        return particles;
    }

    if(!hasNewObservations())
    {
        std::cout << "No new observations, do not update weights." << std::endl;

	setUpdated(false);
	return particles;
    }

    setUpdated(true);

    hasNewObs = false;

    std::vector<PF::Observation>::iterator obsIter;

#ifdef DEBUG_LOCALIZATION
    std::cout << "Using current location " << currentLocation << std::endl;
#endif 

    // @todo add these as parameters.
    const float SIGMA_D = 15.00f;
    const float SIGMA_H = 1.40f;

    PF::ParticleIt partIter;
    for(partIter = particles.begin(); partIter != particles.end(); ++partIter)
    {
	// For each particle, compare the estimated distance from 
	// the robot to the object (and angle) to the actual
	// distance as calculated by the vision system.
        float totalWeight = 0.0f;
        int count = 0;
	for(obsIter = obs.begin(); obsIter != obs.end(); ++obsIter)
	{
	    PF::Observation o = (*obsIter);
	    if(!o.isAmbiguous())
	    {
	        // Since the observation is unambiguous, use the first observation.
		Landmark l = o.possibilities[0];
		PF::Vector2D hypothesisVector = PF::getPosition((*partIter).getLocation(), l.x, l.y);

		float distanceDiff = o.distance - hypothesisVector.magnitude;
		//std::cout << "distanceDiff = " << distanceDiff << std::endl;
		float angleDiff = NBMath::subPIAngle(o.angle) - NBMath::subPIAngle(hypothesisVector.direction);
		//std::cout << "angleDiff = " << angleDiff << std::endl;

		boost::math::normal_distribution<float> pDist(0.0f, SIGMA_D);

		float distanceProb = boost::math::pdf<float>(pDist, distanceDiff);

		boost::math::normal_distribution<float> pAngle(0.0f, SIGMA_H);

		float angleProb = boost::math::pdf<float>(pAngle, angleDiff);
		float probability = distanceProb * angleProb;
		//std::cout << " prob = " << probability << std::endl;

		if(totalWeight == 0.0f)
		    totalWeight = probability;
		else
		    totalWeight *= probability;

		count++;
	    }
	    else
	    {
		// Take the most likely landmark. 
	        std::vector<Landmark>::iterator landmarkIter;
		float maxWeight = 0.0f;
	    	// Loop through all possibilities.
	        for(landmarkIter = o.possibilities.begin(); 
	    	    landmarkIter != o.possibilities.end(); 
	    	    ++landmarkIter)
	        {
	    	    Landmark l = *landmarkIter;
	    	    PF::Vector2D hypothesisVector = PF::getPosition((*partIter).getLocation(), l.x, l.y);
	    	    float distanceDiff = std::abs(o.distance - hypothesisVector.magnitude);
	    	    float angleDiff = std::abs(o.angle - hypothesisVector.direction);
		    boost::math::normal_distribution<float> pDist(0.0f, SIGMA_D);
	    	    float distanceProb = boost::math::pdf<float>(pDist, distanceDiff)/*1.0f/distanceDiff*/;
		    boost::math::normal_distribution<float> pAngle(0.0f, SIGMA_H);
	    	    float angleProb = boost::math::pdf<float>(pAngle, angleDiff)/*1.0f/angleDiff*/;
	    	    float probability = distanceProb * angleProb;
	    	    if(probability > maxWeight)
                        maxWeight = probability;		  
	        }

		// Assign the total weight to be the product of the current total
		// and the newly calculated weight based on current observation.
	    	if(totalWeight == 0.0f)
	    	    totalWeight = maxWeight;
	    	else
	    	    totalWeight *= maxWeight;

	    	count++;
	    }
	}
        // Make sure that we have made an observation before updating weights.
        if(count > 0)
        {
#ifdef DEBUG_LOCALIZATION
            std::cout << "Updating particle of previous weight " << (*partIter).getWeight();
#endif
	    // @todo although this should never happen, it is possible that underflow 
	    // errors cause nan's, which would be problematic for resampling. 
	    if(std::isnan(totalWeight) || std::abs(totalWeight) == HUGE_VAL)
	    {
		std::cout << "Invalid weight calculated!" << std::endl;
		totalWeight = 0.0f;
	    }
            (*partIter).setWeight(totalWeight);
#ifdef DEBUG_LOCALIZATION
            std::cout << " with new weight " << (*partIter).getWeight() << std::endl;
#endif
        }
        //else
	  //std::cout << "Nothing seen, will not update weight." << std::endl;
    }

    return particles;
}

/**
 * Provides the vision module with a new set of observations, and 
 * resets the flag so that the observations will be used on 
 * the next update iteration.
 *
 * @param newObs the new vector of observations.
 */
void VisionSystem::feedObservations(std::vector<PF::Observation> newObs)
{
    if(newObs.size() > 0)
	hasNewObs = true;
    else
	hasNewObs = false;
  
    currentObservations = newObs;
}
