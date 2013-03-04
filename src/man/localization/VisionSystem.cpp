#include "VisionSystem.h"

namespace man
{
    namespace localization
    {

        VisionSystem::VisionSystem() {
            updated = false;
        }
        VisionSystem::~VisionSystem(){}

        /**
         * Updates the particle set according to the observations from Vision
         *
         * @return the updated ParticleSet.
         */
        ParticleSet VisionSystem::update(ParticleSet& particles,
                                         messages::PVisionField observations)
        {
            const float TINY_WEIGHT = .00001f;

            if (observations.timestamp() > lastVisionTimestamp)
            {
                ParticleIt iter;
                // Record totalWeight for normalization
                float totalWeight = 0.0f;
                float newParticleWeight = 1.0f;

                for(iter = particles.begin(); iter != particles.end(); iter++)
                {
                    Particle* particle = &(*iter);

                    for (int i=0; i<observations.visual_corner_size(); i++)
                    {
                        messages::PVisualCorner visualCorner;
                        visualCorner.CopyFrom(observations.visual_corner(i));
                        float newWeight = 1.0f; //scoreFromLandmark(**particle, visualCorner);
                        if (newWeight < TINY_WEIGHT)
                            newWeight = TINY_WEIGHT;
                        newParticleWeight *= newWeight;

                    }

                    if (observations.has_goal_post_l()){
                        float newWeight = 1.0f; //scoreFromLandmark(particle, observations.goal_post_l);
                        if (newWeight < TINY_WEIGHT)
                            newWeight = TINY_WEIGHT;
                        newParticleWeight *= newWeight;
                    }
                    if (observations.has_goal_post_r()){
                        float newWeight = 1.0f; //scoreFromLandmark(particle, observations.goal_post_l);
                        if (newWeight < TINY_WEIGHT)
                            newWeight = TINY_WEIGHT;
                        newParticleWeight *= newWeight;
                    }
                    if (observations.has_visual_cross()){
                        float newWeight = 1.0f; //scoreFromLandmark(particle, observations.goal_post_l);
                        if (newWeight < TINY_WEIGHT)
                            newWeight = TINY_WEIGHT;
                        newParticleWeight *= newWeight;
                    }

                    // We never updated the new particle weight, so no observations been made
                    if(newParticleWeight >= 1.0f)
                        return particles;
                    else
                    {
                        particle->setWeight(newParticleWeight);
                        totalWeight += newParticleWeight;
                    }
                }

                // normalize the particle weights
//                ParticleIt iter;
                for(iter = particles.begin(); iter != particles.end(); iter++)
                {
                    Particle* particle = &(*iter);
                    particle->normalizeWeight(totalWeight);
                }

                // We've updated particles with vision, so tell PF to resample
                setUpdated(true);
            }

            return particles;
        }

        void VisionSystem::setUpdated(bool val)
        {
            updated = val;
        }

        /**
         * @brief Takes a PVisualObservation and particle & returns the best
         *        possible score for the combination
         */
        float VisionSystem::scoreFromVisDetect(const Particle& particle,
                                               const messages::PVisualDetection& obsv)
        {
            float bestScore = 0;

            for (int i=0; i<obsv.concrete_coords_size(); i++)
            {
                const messages::Point& fieldPoint = obsv.concrete_coords(i);

                RelVector relVector = getRelativeVector(particle,
                                                        fieldPoint.x(),
                                                        fieldPoint.y());

                float distanceDiff = obsv.distance() - relVector.magnitude;
                float angleDiff = NBMath::subPIAngle(obsv.bearing() - relVector.direction);

                boost::normal_distribution<float> pDist(0.0f, obsv.distance_sd());
                boost::normal_distribution<float> pAngle(0.0f, obsv.bearing_sd());
                boost::variate_generator<boost::mt19937&, boost::normal_distribution<float> > distGen(rng,
                                                                                                      pDist);
                boost::variate_generator<boost::mt19937&, boost::normal_distribution<float> > bearGen(rng,
                                                                                                      pAngle);


                float distanceProb = distGen();
                float angleProb = bearGen();

                // Better way to determine probability?
                float score = distanceProb * angleProb;
                if (score > bestScore)
                    bestScore = score;

            }

            return bestScore;

        }

        RelVector VisionSystem::getRelativeVector(const Particle& particle,
                                    float fieldX, float fieldY)
        {
            float dx = fieldX - particle.getLocation().x();
            float dy = fieldY - particle.getLocation().y();

            float magnitude = std::sqrt(dx*dx + dy*dy);

            float sinh, cosh;
            sincosf(-1*particle.getLocation().h(), &sinh, &cosh);

            float x_prime = cosh * dx - sinh * dy;
            float y_prime = sinh * dx + cosh * dy;

            float bearing = NBMath::safe_atan2(y_prime, x_prime);

            return RelVector(magnitude, bearing);
        }

    } // namespace localization
} // namespace man
