#include "VisionSystem.h"

/**
 * Constructor
 */
VisionSystem::VisionSystem(LandmarkMap m = LandmarkMap())
    : PF::SensorModel(), map(m)
{ }

/**
 * Update particle weights based on current visual observations.
 * @param particles the particle set to be updated with sensor 
 *                  data.
 * @return the updated particle set.
 */
PF::ParticleSet VisionSystem::update(PF::ParticleSet particles)
{
    // Determine which (if any) observations can be made.
    std::vector<Observation> obs = determineObservations(currentLocation, ROV);
    if(obs.size() == 0)
    {
#ifdef DEBUG_LOCALIZATION
        std::cout << "Nothing seen, do not update weights." << std::endl;
#endif
        return particles;
    }
    std::vector<Observation>::iterator obsIter;

#ifdef DEBUG_LOCALIZATION
    std::cout << "Using current location " << currentLocation << std::endl;
#endif 

    PF::ParticleIt partIter;
    for(partIter = particles.begin(); partIter != particles.end(); ++partIter)
    {
	// For each particle, compare the estimated distance from 
	// the robot to the object (and angle) to the actual
	// distance as calculated by the vision system.
	//float maxProbability = 0.0f;
        float totalWeight = 0.0f;
        int count = 0;
	for(obsIter = obs.begin(); obsIter != obs.end(); ++obsIter)
	{
      	    Observation o = (*obsIter);
	    if(!o.isAmbiguous())
	    {
	        // Since the observation is unambiguous, use the first observation.
		Landmark l = o.possibilities[0];
		PF::Vector2D hypothesisVector = PF::getPosition((*partIter).getLocation(), l.x, l.y);
		float distanceDiff = std::abs(o.distance - hypothesisVector.magnitude);
		float angleDiff = std::abs(o.angle - hypothesisVector.direction);
		// For now, generate a new importance weight as the inverse of 
		// the difference between the distances multiplied by the 
		// inverse of the difference between the angles.
		float distanceProb = 1.0f/distanceDiff;
		float angleProb = 1.0f/angleDiff;
		float probability = distanceProb * angleProb;
		// Assuming conditional independence between measurements.
		if(totalWeight == 0.0f)
		    totalWeight = probability;
		else
		    totalWeight *= probability;

		count++;
	    }
	    else
	    {
		// Deal with ambiguous observations. For now, take the most likely 
	        // measurement. @todo
	        std::vector<Landmark>::iterator landmarkIter;
		float maxWeight = 0.0f;
		// Loop through all possibilities, and use the most likely measurement.
	        for(landmarkIter = o.possibilities.begin(); 
		    landmarkIter != o.possibilities.end(); 
		    ++landmarkIter)
	        {
		    Landmark l = *landmarkIter;
		    PF::Vector2D hypothesisVector = PF::getPosition((*partIter).getLocation(), l.x, l.y);
		    float distanceDiff = std::abs(o.distance - hypothesisVector.magnitude);
		    float angleDiff = std::abs(o.angle - hypothesisVector.direction);
		    float distanceProb = 1.0f/distanceDiff;
		    float angleProb = 1.0f/angleDiff;
		    float probability = distanceProb * angleProb;
		    if(probability > maxWeight)
                        maxWeight = probability;		  
	        }
		// Since we are assuming conditional independence, compound the
		// probabilities of measurement for each individual observation.
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
            (*partIter).setWeight(totalWeight);
#ifdef DEBUG_LOCALIZATION
            std::cout << " with new weight " << (*partIter).getWeight() << std::endl;
#endif
        }
        else
            std::cout << "Nothing seen, will not update weight." << std::endl;
    }

    return particles;
}
