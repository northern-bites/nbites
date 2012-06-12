#include "VisionSystem.h"

/**
 * Constructor
 */
VisionSystem::VisionSystem(LocalizationVisionParams params)
    : PF::SensorModel(), parameters(params)
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

        setUpdated(false);

        return particles;
    }
    // else
    //      std::cout << "#obs = " << obs.size() << std::endl;

    setUpdated(true);

    std::vector<PF::Observation>::iterator obsIter;

#ifdef DEBUG_LOCALIZATION
    std::cout << "Using current location " << currentLocation << std::endl;
#endif

    PF::ParticleIt partIter;
    for(partIter = particles.begin(); partIter != particles.end(); ++partIter)
    {
//        std::cout << "\n \n \n \n NEW PARTICLE IS BEING UPDATES BY VISION"
//                  << std::endl;
//        std::cout << "This particle is at: " << (*partIter).getLocation()
//                  << " \n";
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
                // Since the observation is unambiguous,
                // there is only one possibility.
                if(!o.isCorner())
                {
                Landmark l = o.possibilities[0];
                PF::Vector2D hypothesisVector = PF::getPosition(
                                           (*partIter).getLocation(), l.x, l.y);
                float distanceDiff = o.distance - hypothesisVector.magnitude;
                float angleDiff = NBMath::subPIAngle(o.angle) -
                                 NBMath::subPIAngle(hypothesisVector.direction);
                boost::math::normal_distribution<float> pDist(0.0f,
                                                       parameters.sigma_d);

                float distanceProb = boost::math::pdf<float>(pDist,
                                                             distanceDiff);

                boost::math::normal_distribution<float> pAngle(0.0f,
                                                        parameters.sigma_h);

                float angleProb = boost::math::pdf<float>(pAngle, angleDiff);

                // Better way to determine probability?
                float probability = distanceProb * angleProb;
//                std::cout << "Probability: " << probability << "\n";

                if(totalWeight == 0.0f)
                    totalWeight = probability;
                else
                    totalWeight *= probability;

                count++;
                }
            }
            else
            {
                // Take the most likely landmark.
                std::vector<Landmark>::iterator landmarkIter;
                float maxWeight = 0.0f;
                // Loop through all possibilities.
//                std::cout << "We see an ambigous landmark. Lets go through all possibilities and find the best one" << std::endl;
                for(landmarkIter = o.possibilities.begin();
                    landmarkIter != o.possibilities.end();
                    ++landmarkIter)
                {
                    Landmark l = *landmarkIter;
//                    std::cout << "\n Potential landmark: " << l << "\n";
//                    std::cout << "Landmark Rel Location: " << o.distance
//                              << " , " << o.angle << "\n \n";
                    PF::Vector2D hypothesisVector = PF::getPosition((*partIter).getLocation(), l.x, l.y);
//                    std::cout << "Hypothesis vector: " << hypothesisVector << "\n";
                    float distanceDiff = std::abs(o.distance - hypothesisVector.magnitude);
//                    std::cout << "Distance Diff: " << distanceDiff << std::endl;
                    float angleDiff = std::abs(o.angle - hypothesisVector.direction);
//                    std::cout << "Angle Diff " << angleDiff << std::endl;
                    boost::math::normal_distribution<float> pDist(0.0f, parameters.sigma_d);
                    float distanceProb = boost::math::pdf<float>(pDist, distanceDiff);
                    boost::math::normal_distribution<float> pAngle(0.0f, parameters.sigma_h);
                    float angleProb = boost::math::pdf<float>(pAngle, angleDiff);
                    float probability = distanceProb * angleProb;
//                    std::cout << "Probability: " << probability << "\n \n";
                    if(probability > maxWeight)
                    {
                        maxWeight = probability;
//                        std::cout << "Best Guess so far! \n";
                    }
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
        // std::cout << "(" << (*partIter).getLocation().x
        //            << ", " << (*partIter).getLocation().y
        //            << ", " << (*partIter).getLocation().heading
        //            << ") : " << totalWeight << std::endl;
            (*partIter).setWeight(totalWeight);
#ifdef DEBUG_LOCALIZATION
            std::cout << " with new weight " << (*partIter).getWeight() << std::endl;
#endif
        }
    }
    //std::cout << "---------------------------------------------" << std::endl;

    return particles;
}

/**
 * Provides the vision module with a new set of observations.
 *
 * @param newObs the new vector of observations.
 */
void VisionSystem::feedObservations(std::vector<PF::Observation> newObs)
{
    currentObservations = newObs;
}
