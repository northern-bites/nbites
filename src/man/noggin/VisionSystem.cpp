#include "VisionSystem.h"

//TODO: move this to a sensible header
static const float MAX_CORNER_DISTANCE = 400.0f;

/**
 * Constructor
 */
VisionSystem::VisionSystem(Vision::const_ptr vision, LocalizationVisionParams params)
    : PF::SensorModel(), vision(vision), parameters(params)
{ }

/**
 * Update particle weights based on current visual observations.
 * @param particles the particle set to be updated with sensor
 *                  data.
 * @return the updated particle set.
 */
PF::ParticleSet VisionSystem::update(PF::ParticleSet particles)
{

#ifdef DEBUG_LOCALIZATION
    std::cout << "Using current location " << currentLocation << std::endl;
#endif
    // FOR TESTING, should not be in this scope
    int count = 0;

    float bestCornerProbabilities = 0.0f;
    int count_corner = 0;

    PF::ParticleIt partIter;
    for(partIter = particles.begin(); partIter != particles.end(); ++partIter)
    {
        float totalWeight = 0.0f;
//        int count = 0;

        // Visual objects

        // TODO: a bit nicer if we had a visual field object iterator of sorts in vision ...
        incorporateLandmarkObservation<VisualFieldObject, ConcreteFieldObject>(*(vision->yglp),
                *(partIter), totalWeight, count);
        incorporateLandmarkObservation<VisualFieldObject, ConcreteFieldObject>(*(vision->ygrp),
                *(partIter), totalWeight, count);

        // Visual cross
        incorporateLandmarkObservation<VisualCross, ConcreteCross>(*(vision->cross),
                                *(partIter), totalWeight, count);

        std::list<VisualCorner> * corners = vision->fieldLines->getCorners();
        std::list<VisualCorner>::const_iterator vc;
        for(vc = corners->begin(); vc != corners->end(); ++vc)
        {
            if (vc->getDistance() < MAX_CORNER_DISTANCE && vc->isReliable())
            {
                float bestProbability = 0.0f;
                typedef const std::list<const ConcreteCorner*> ConcreteCorners;

                ConcreteCorners* concreteCorners = vc->getPossibilities();
                ConcreteCorners::const_iterator cc;

                for (cc = concreteCorners->begin(); cc != concreteCorners->end(); cc++) {
                    float probability = scoreFromCorner(*vc, **cc, *partIter);

                    if (probability > bestProbability)
                        bestProbability = probability;
                }

                bestCornerProbabilities+=bestProbability;
                count_corner++;

                totalWeight = updateTotalWeight(totalWeight, bestProbability);
                count++;

            } else{
//                std::cout << "We saw a corner REALLY far away: "
//                        << vc->getDistance()<< " centimeters away" <<std::endl
//                        << "They can't be more than" << MAX_CORNER_DISTANCE
//                        << " centimeters away." << std::endl;
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
        } else {
            //no observations, no use trying to re-weigh the other particles
            setUpdated(false);
//            std::cout << "We made no new observations \n";
            return particles;
        }
    }

//    if (bestCornerProbabilities == 0.0f && count_corner > 0 && particles.size() > 0) {
//        throw(std::exception());
//    }

    //std::cout << "---------------------------------------------" << std::endl;
    setUpdated(true);
    return particles;
}
