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
         * @return if observations were made
         */
        bool VisionSystem::update(ParticleSet& particles,
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
                        float newWeight = scoreFromVisDetect(*particle,visualCorner.visual_detection());
                        // std::cout << "Best Weight from the corner\t" << newWeight << "\n\n";
                        //float newWeight = 1.0f; //scoreFromLandmark(**particle, visualCorner);
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
                    {
                        std::cout << "In the Vision System, given a message with no observations...\n";
                        return false;
                    }
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

            return true;
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
            // DEBUG VIS DETECT
            // std::cout << "Scoring a visual detection and a particle ---------- \n";

            float bestScore = 0;

            for (int i=0; i<obsv.concrete_coords_size(); i++)
            {
                // std::cout << "Concrete Obsv(x,y):\t(" << obsv.concrete_coords(i).x() << " , "
                //           << obsv.concrete_coords(i).y() << ")\n";
                // std::cout << "Particle (x,y,h):\t(" << particle.getLocation().x() << " , "
                //           << particle.getLocation().y() << " , "
                //           << particle.getLocation().h() <<")\n";
                const messages::Point& fieldPoint = obsv.concrete_coords(i);

                RelVector relVector = getRelativeVector(particle,
                                                        fieldPoint.x(),
                                                        fieldPoint.y());

                // std::cout << "Calc Distance:\t" << relVector.magnitude << "\t VS\t" << obsv.distance() << "\n";
                // std::cout << "Calc bear:\t" << relVector.direction << "\t VS\t" << obsv.bearing() << "\n";

                float distanceDiff = obsv.distance() - relVector.magnitude;
                float angleDiff = NBMath::subPIAngle(obsv.bearing() - relVector.direction);

                // std::cout << "DistanceDiff:\t" << distanceDiff << "\tAngleDiff\t" << angleDiff << "\n";

                boost::math::normal_distribution<float> pDist(0.0f, obsv.distance_sd());
                float distanceProb = boost::math::pdf<float>(pDist, distanceDiff);


                boost::math::normal_distribution<float> pAngle(0.0f, obsv.bearing_sd());
                float angleProb = boost::math::pdf<float>(pAngle, angleDiff);

                // Better way to determine probability?
                float score = distanceProb * angleProb;
                // std::cout << "Score:\t" << score << "\n";
                if (score > bestScore)
                    bestScore = score;

            }

            return bestScore;

        }

        RelVector VisionSystem::getRelativeVector(const Particle& particle,
                                    float fieldX, float fieldY)
        {
            float relX = fieldX - particle.getLocation().x();
            float relY = fieldY - particle.getLocation().y();

            float magnitude = std::sqrt(relX*relX + relY*relY);

            // Know that the heading + bearing = angle from Field Origing (call it Theta)
            // So calculate Theta using arctan(relX/relY) and then subtract the heading in 1 line
            float bearing = NBMath::subPIAngle(NBMath::safe_atan2(relX,relY));

            return RelVector(magnitude, bearing);
        }

    } // namespace localization
} // namespace man
