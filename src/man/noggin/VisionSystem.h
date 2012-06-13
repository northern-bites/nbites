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
    M_PI/20.0f
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

//    void feedObservations(std::vector<PF::Observation> newObs);

    template <class VisualObservationT, class ConcretePossibilityT>
    void incorporateLandmarkObservation(VisualObservationT& observation,
                                                 const Particle& particle,
                                                 float& totalWeight,
                                                 int& observationCount) {

        if (isSaneLandmarkObservation<VisualObservationT>(observation)) {

            float probability = scoreFromLandmark<VisualObservationT, ConcretePossibilityT>(observation,
                    *(observation.getPossibilities()->front()), particle);

            if(totalWeight == 0.0f)
                totalWeight = probability;
            else
                totalWeight*= probability;
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

        PF::Vector2D hypothesisVector = PF::getPosition(particle.getLocation(),
                landmark.getFieldX(), landmark.getFieldY());

        float distanceDiff = observation.getDistance() - hypothesisVector.magnitude;
        float angleDiff = NBMath::subPIAngle(observation.getBearing() - hypothesisVector.direction);

        // Grab the distSD from the observation
        float distSD = sqrtf(observation.getEstimate().distance_variance);
        boost::math::normal_distribution<float> pDist(0.0f, distSD);
        float distanceProb = boost::math::pdf<float>(pDist, distanceDiff);

        // Grab the bearSD from the observation
        float bearSD = sqrtf(observation.getEstimate().bearing_variance);
        boost::math::normal_distribution<float> pAngle(0.0f, bearSD);
        float angleProb = boost::math::pdf<float>(pAngle, angleDiff);

        // Better way to determine probability?
        return distanceProb * angleProb;
    }

    static float scoreParticleAgainstPose(const Particle& particle,
                                          const PF::Location pose,
                                          float distVar,
                                          float headVar){

        float pose_diff_h = NBMath::subPIAngle(pose.heading -
                                               particle.getLocation().heading);
        // create a normal distribution with the std dev derived from the variance
        boost::math::normal_distribution<float> pHeading(0.0f, sqrtf(headVar));
        float prob_h = boost::math::pdf<float>(pHeading, pose_diff_h);

        float pose_diff_d = NBMath::getHypotenuse(pose.x - particle.getLocation().x,
                                                  pose.y - particle.getLocation().y);
        boost::math::normal_distribution<float> pDistance(0.0f, sqrt(distVar));
        float prob_d = boost::math::pdf<float> (pDistance, pose_diff_d);

        return prob_h * prob_d;

    }

    static float scoreFromCorner(const VisualCorner& observation,
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

        float pose_diff_h = NBMath::subPIAngle(pose_h - particle.getLocation().heading);

        float distVar = observation.getEstimate().distance_variance;
        float bearVar = observation.getEstimate().bearing_variance;

        std::cout <<distVar << " " <<bearVar << "\n";
        float globalOrientationSD =
                NBMath::getHypotenuse(observation.getBearingSD(),
                                      observation.getPhysicalOrientationSD());

        std::cout << "Attempt normal distribution? \n";
        boost::math::normal_distribution<float> pHeading(0.0f, globalOrientationSD);
        float prob_h = boost::math::pdf<float>(pHeading, pose_diff_h);

        float pose_diff_d = NBMath::getHypotenuse(pose_x - particle.getLocation().x,
                pose_y - particle.getLocation().y);
        boost::math::normal_distribution<float> pDistance(0.0f, observation.getDistanceSD());
        float prob_d = boost::math::pdf<float>(pDistance, pose_diff_d);

        PF::Location reconstructedLocation(pose_x, pose_y, pose_h);

        float prevProb = prob_h * prob_d;
        float newProb =  scoreParticleAgainstPose(particle,
                                                  reconstructedLocation,
                                                  observation.getDistanceSD(),
                                                  globalOrientationSD);
                                                  // observation.getEstimate().distance_variance,
                                                  // observation.getEstimate().bearing_variance);

        std::cout << "Previous calculation would be: " << prevProb << "\n";
        std::cout << "New Calculation would be: " << newProb << "\n";
        std::cout << "Equal?  " << prevProb << " == " << newProb << "\n\n";

        return newProb;


    }

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
    LocalizationVisionParams parameters;

//    std::vector<PF::Observation> currentObservations;

    // Normal distributions for calculating weights of particles
    // based on visual observations.
    boost::math::normal_distribution<float> distanceDistribution;
    boost::math::normal_distribution<float> angleDistribution;
};

#endif // VISION_SYSTEM_H
