#include "VisionSystem.h"

namespace man
{
    namespace localization
    {

        VisionSystem::VisionSystem() {};
        VisionSystem::~VisionSystem(){};

        /**
         * Updates the particle set according to the observations from Vision
         *
         * @return the updated ParticleSet.
         */
        ParticleSet VisionSystem::update(ParticleSet& particles,
                                         messages::PVisionField observations)
        {
            const float TINY_WEIGHT = .00001f;

            if (visionInput.timestamp() > lastVisionTimestamp)
            {
                Particle It iter;

                // Record totalWeight for normalization
                float totalWeight = 0.0f;
                float newParticleWeight = 1.0f

                for(iter = particles.begin(); iter != particles.end(); iter++)
                {
                    Particle* particle = &(*iter);

                    for (int i=0; i<observations.visual_corner_size(); i++)
                    {
                        float newWeight = scoreFromLandmark(particle, observations.visual_corner(i));
                        if (newWeight < TINY_WEIGHT)
p                            newWeight = TINY_WEIGHT;
                        newParticleWeight *= newWeight;

                    }

                    if (observations.has_goal_post_l){
                        float newWeight = scoreFromLandmark(particle, observations.goal_post_l);
                        if (newWeight < TINY_WEIGHT)
                            newWeight = TINY_WEIGHT;
                        newParticleWeight *= newWeight;
                    }
                    if (observations.has_goal_post_r){
                        float newWeight = scoreFromLandmark(particle, observations.goal_post_l);
                        if (newWeight < TINY_WEIGHT)
                            newWeight = TINY_WEIGHT;
                        newParticleWeight *= newWeight;
                    }
                    if (observations.has_visual_cross){
                        float newWeight = scoreFromLandmark(particle, observations.goal_post_l);
                        if (newWeight < TINY_WEIGHT)
                            newWeight = TINY_WEIGHT;
                        newParticleWeight *= newWeight;
                    }

                    // We never updated the new particle weight, so no observations been made
                    if(newParticleWeight >= 1.0f)
                        return particles;
                    else
                    {
                        particle.setWeight(newParticleWeight);
                        totalWeight += newParticleWeight;
                    }
                }

                // normalize the particle weights
                 Particle It iter;
                for(iter = particles.begin(); iter != particles.end(); iter++)
                {
                    Particle* particle = &(*iter);
                    particle.normalizeWeight(totalWeight
                }

                // We've updated particles with vision, so tell PF to resample
                setUpdated(true);
            }
        }

        /**
         * @brief Takes a PVisualObservation and particle & returns the best
         *        possible score for the combination
         */
        template <class Observation>
        float VisionSystem::scoreFromLandmark(const Particle& particle,
                                const Observation& observation_)
        {
            float bestScore = 0;

            // Only using info from Visual Detection if in this function, so call obsv for short
            messages::PVisualDetection& obsv = observation_.visual_detection();
            for (int i=0; i<obsv.concrete_coords_size(); i++)
            {
                const messages::Point& fieldPoint = obsv.concrete_coords(i);

                RelVector relVector = getRelativeVector(particle,
                                                        fieldPoint.x(),
                                                        fieldPoint.y());

                float distanceDiff = obsv.distance() - relVector.magnitude;
                float angleDiff = NBMath::subPIAngle(obsv.bearing() - relVector.direction());

                boost::math::normal_distribution<float> pDist(0.0f, obsv.distance_SD());
                float distanceProb = boost::math::pdf<float>(pDist, distanceDiff);

                boost::math::normal_distribution<float> pAngle(0.0f, obsv.bearing_SD());
                float angleProb = boost::math::pdf<float>(pAngle, angleDiff);

                // Better way to determine probability?
                float score = distanceProb * angleProb;
                if (score > bestScore)
                    bestScore = score;

            }

            return bestScore;

        }

        RelVector getRelativeVector(const Particle& particle,
                                    float fieldX, float fieldY)
        {
            float dx = fieldX - particle->getLocation.x();
            float dy = fieldY - particle->getLocation.y();

            float magnitude = std::sqrt(dx*dx + dy*dy);

            float sinh, cosh;
            sincosf(-origin.heading, &sinh, &cosh);

            float x_prime = cosh * dx - sinh * dy;
            float y_prime = sinh * dx + cosh * dy;

            float bearing = NBMath::safe_atan2(y_prime, x_prime);

            return RelVector(magnitude, bearing);
        }

    } // namespace localization
} // namespace man
