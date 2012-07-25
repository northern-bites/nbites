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

struct LocalizationVisionParams
{
    float sigma_d;       // Variance for calculating distance weights.
    float sigma_h;       // Variance for calculating heading weights.
};

static const LocalizationVisionParams DEFAULT_LOCVIS_PARAMS =
{
    15.00f,
    (float) M_PI/20.0f
};

/**
 * @class VisionSystem
 */
class VisionSystem : public PF::SensorModel
{
    typedef PF::LocalizationParticle Particle;

 public:
    VisionSystem(Vision::const_ptr vision, LocalizationVisionParams params = DEFAULT_LOCVIS_PARAMS);

    PF::ParticleSet update(PF::ParticleSet particles);

    template <class VisualObservationT, class ConcretePossibilityT>
    void incorporateLandmarkObservation(VisualObservationT& observation,
                                                 const Particle& particle,
                                                 float& totalWeight,
                                                 int& observationCount){
        if (isSaneLandmarkObservation<VisualObservationT>(observation)) {

            typedef const std::list<const ConcretePossibilityT*> ConcretePossibilities;

            ConcretePossibilities* concretePossibilities = observation.getPossibilities();
            typename ConcretePossibilities::const_iterator possibilityIterator;

            float bestProbability = 0.0f;
            float secondBestProbability = 0.0f;

            for (possibilityIterator = concretePossibilities->begin();
                 possibilityIterator != concretePossibilities->end(); possibilityIterator++) {

                float probability = scoreFromLandmark<VisualObservationT, ConcretePossibilityT>
                    (observation, (**possibilityIterator), particle);

                if (probability > bestProbability) {
                    secondBestProbability = bestProbability;
                    bestProbability = probability;
                }
            }

            const float MIN_DIFF_PROBABILITY = 0.001f;
            if (std::abs(bestProbability - secondBestProbability) < MIN_DIFF_PROBABILITY) {
                //deal with confusing particles - they shouldn't be probable
		return;
            }

            totalWeight = updateTotalWeight(totalWeight, bestProbability);
            observationCount++;
        }
    }

    template <class VisualObservationT>
    bool isSaneLandmarkObservation(VisualObservationT& observation) {
        return observation.hasPositiveID() && observation.hasValidDistance();
    }

    template <class VisualObservationT, class ConcretePossibilityT>
    float scoreFromLandmark(const VisualObservationT& observation,
                            const ConcretePossibilityT& landmark,
                            const Particle& particle) {

        //       std::cout << "score from a " << landmark << "\n";
        //std::cout << "coordinates " << landmark.getFieldX() << " , " << landmark.getFieldY() << "\n";
        PF::Vector2D hypothesisVector = PF::getPosition(particle.getLocation(),
                landmark.getFieldX(), landmark.getFieldY());

        float distanceDiff = observation.getDistance() - hypothesisVector.magnitude;
        float angleDiff = NBMath::subPIAngle(observation.getBearing() - hypothesisVector.direction);

        boost::math::normal_distribution<float> pDist(0.0f, observation.getDistanceSD());
        float distanceProb = boost::math::pdf<float>(pDist, distanceDiff);

        boost::math::normal_distribution<float> pAngle(0.0f, observation.getBearingSD());
        float angleProb = boost::math::pdf<float>(pAngle, angleDiff);

        // Better way to determine probability?
        return distanceProb * angleProb;
    }

    static float scoreParticleAgainstPose(const Particle& particle,
                                          const PF::Location pose,
                                          float distSD,
                                          float headSD){

        float pose_diff_d = NBMath::getHypotenuse(pose.x - particle.getLocation().x,
                                                  pose.y - particle.getLocation().y);
        boost::math::normal_distribution<float> pDistance(0.0f, distSD);
        float prob_d = boost::math::pdf<float> (pDistance, pose_diff_d);

        float pose_diff_h = NBMath::subPIAngle(pose.heading -
                                               particle.getLocation().heading);
        boost::math::normal_distribution<float> pHeading(0.0f, headSD);
        float prob_h = boost::math::pdf<float>(pHeading, pose_diff_h);

        return prob_h * prob_d;
    }

    float scoreFromCorner(const VisualCorner& observation,
                          const ConcreteCorner& concrete,
                          const Particle& particle) {
        //angle between the robot's visual heading line (or bearing to corner line)
        //and the line parallel to the x axis oriented towards the corner
        //(so x axis flipped)
        float globalPhysicalOrientation =
                observation.getPhysicalOrientation() + concrete.getFieldAngle();

        float sin_global_orientation, cos_global_orientation;
        //sin and cos altogether (faster)
        sincosf(globalPhysicalOrientation, &sin_global_orientation, &cos_global_orientation);

        float pose_x = concrete.getFieldX() - observation.getDistance()*cos_global_orientation;
        float pose_y = concrete.getFieldY() - observation.getDistance()*sin_global_orientation;
        float pose_h = globalPhysicalOrientation - observation.getBearing();

        PF::Location reconstructedLocation(pose_x, pose_y, pose_h);

        //Throwout automatically any locations off field
        if ((pose_x < 0) || (pose_x > FIELD_GREEN_HEIGHT) ||
            (pose_y < 0) || (pose_y > FIELD_GREEN_HEIGHT))
        {
//            std::cout << "OFFFIELD POSSIBILITY \n";
            return 0.0000001f;
        }
        float globalOrientationSD = NBMath::getHypotenuse(observation.getBearingSD(),
                                                          observation.getPhysicalOrientationSD());

        return  scoreParticleAgainstPose(particle,
                                         reconstructedLocation,
                                         observation.getDistanceSD(),
                                         globalOrientationSD);
    }

 private:
    static float updateTotalWeight(float currentTotalWeight, float currentWeight) {
        const float TINY_WEIGHT = .00001;
        if (currentWeight <=  0)
            currentWeight = TINY_WEIGHT;
        if(currentTotalWeight == 0.0f)
            currentTotalWeight = currentWeight;
        else
            currentTotalWeight *= currentWeight;

        return currentTotalWeight;
    }

 private:
    Vision::const_ptr vision;
    LocalizationVisionParams parameters;

//    std::vector<PF::Observation> currentObservations;

    // Normal distributions for calculating weights of particles
    // based on visual observations.
    boost::math::normal_distribution<float> distanceDistribution;
    boost::math::normal_distribution<float> angleDistribution;
};

#endif // VISION_SYSTEM_H
