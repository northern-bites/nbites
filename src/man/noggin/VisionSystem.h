/**
 * Implements an interface between localization and the vision
 * system to apply visual landmark measurements to the
 * particles.
 *
 * @author Ellis Ratner <eratner@bowdoin.edu>
 */
#ifndef VISION_SYSTEM_H
#define VISION_SYSTEM_H

#include <string>
#include "ParticleFilter.h"
#include "ConcreteFieldObject.h"
#include "NBMath.h"
#include "Vision.h"
#include <boost/math/distributions.hpp>


//struct LocalizationVisionParams
//{
//    float sigma_d;       // Variance for calculating distance weights.
//    float sigma_h;       // Variance for calculating heading weights.
//};
//
//static const LocalizationVisionParams DEFAULT_LOCVIS_PARAMS =
//{
//    15.00f,
//    1.25f
//};

/**
 * @class VisionSystem
 */
class VisionSystem : public PF::SensorModel
{

    typedef PF::LocalizationParticle Particle;

 public:
    VisionSystem(Vision::const_ptr vision);//, LocalizationVisionParams params = DEFAULT_LOCVIS_PARAMS);

    PF::ParticleSet update(PF::ParticleSet particles);

//    void feedObservations(std::vector<PF::Observation> newObs);

    template <class VisualObservationT, class ConcretePossibilityT>
    static float incorporateLandmarkObservation(VisualObservationT& observation,
                                                 const Particle& particle,
                                                 float& totalWeight,
                                                 int& observationCount) {

        if (isSaneLandmarkObservation<VisualObservationT>(observation)) {

            float probability = scoreFromLandmark<VisualObservationT, ConcretePossibilityT>(observation,
                    observation->getPossibilities()->front(), particle);

            totalWeight = updateTotalWeight(totalWeight, probability);
            observationCount++;
        }
    }

    template <class VisualObservationT>
    static bool isSaneLandmarkObservation(VisualObservationT& observation) {
        return observation.hasPositiveID() && observation.hasValidDistance();
    }

    template <class VisualObservationT, class ConcretePossibilityT>
    static float scoreFromLandmark(const VisualObservationT& observation,
                            const ConcretePossibilityT& landmark,
                            const Particle& particle) {

        PF::Vector2D hypothesisVector = PF::getPosition(particle.getLocation(),
                observation.getDistance(), observation.getBearing());
        float distanceDiff = observation.getDistance() - hypothesisVector.magnitude;
        float angleDiff = NBMath::subPIAngle(observation.getBearing()) -
                NBMath::subPIAngle(hypothesisVector.direction);

        boost::math::normal_distribution<float> pDist(0.0f, observation.getDistanceSD());
        float distanceProb = boost::math::pdf<float>(pDist, distanceDiff);

        boost::math::normal_distribution<float> pAngle(0.0f, observation.getBearingSD());
        float angleProb = boost::math::pdf<float>(pAngle, angleDiff);

        // Better way to determine probability?
        return distanceProb * angleProb;
    }

    template <class VisualCornerT, class Co

 private:
    static float updateTotalWeight(float currentTotalWeight, float currentWeight) {

        if(currentTotalWeight == 0.0f)
            currentTotalWeight = currentWeight;
        else
            currentTotalWeight = currentTotalWeight*currentWeight;

        return currentTotalWeight;
    }

 private:
    Vision::const_ptr vision;
//    LocalizationVisionParams parameters;

//    std::vector<PF::Observation> currentObservations;

    // Normal distributions for calculating weights of particles
    // based on visual observations.
    boost::math::normal_distribution<float> distanceDistribution;
    boost::math::normal_distribution<float> angleDistribution;
};

#endif // VISION_SYSTEM_H
