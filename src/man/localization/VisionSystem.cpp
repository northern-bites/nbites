#include "VisionSystem.h"

namespace man
{
    namespace localization
    {

        VisionSystem::VisionSystem() {
        }
        VisionSystem::~VisionSystem(){}

        /**
         * Updates the particle set according to the observations from Vision
         * Assume that we are given NEW information (ie we checked for repeat message elsewhere)
         *
         * @return if observations were made
         */
        bool VisionSystem::update(ParticleSet& particles,
                                  const messages::VisionField& obsv)
        {
            const float TINY_WEIGHT = .00001f;


            ParticleIt iter;
            // Record totalWeight for normalization
            float totalWeight = 0.0f;
            bool madeObsv = false;

            float times = 0;
            for(iter = particles.begin(); iter != particles.end(); iter++)
            {
                Particle* particle = &(*iter);
                float newParticleError = 0.f;

                for (int i=0; i<obsv.visual_corner_size(); i++)
                {
                    if(obsv.visual_corner(i).visual_detection().on()
                       && (obsv.visual_corner(i).visual_detection().distance() > 0.f)) {
                        madeObsv = true;

                        float newError = scoreFromVisDetect(*particle,
                                                            obsv.visual_corner(i).visual_detection());
                        newParticleError+= newError;

                    }
                }

                if (obsv.has_goal_post_l() && obsv.goal_post_l().visual_detection().on()
                    && (obsv.goal_post_l().visual_detection().distance() > 0.f)) {
                    madeObsv = true;
                    float newError = scoreFromVisDetect(*particle,
                                                        obsv.goal_post_l().visual_detection());
                    newParticleError+= newError;
                }

                if (obsv.has_goal_post_r() && obsv.goal_post_r().visual_detection().on()
                    && (obsv.goal_post_r().visual_detection().distance() > 0.f)) {
                    madeObsv = true;
                    float newError = scoreFromVisDetect(*particle,
                                                        obsv.goal_post_r().visual_detection());
                    newParticleError+= newError;
                }

                if (obsv.has_visual_cross() && obsv.visual_cross().on()
                    && (obsv.visual_cross().distance() > 0.f)) {
                    madeObsv = true;
                    float newError = scoreFromVisDetect(*particle,
                                                        obsv.visual_cross());
                    newParticleError+= newError;
                }

                // We never updated the new particle weight, so no observations been made
                if(!madeObsv)
                {
                    //std::cout << "In the Vision System, given a message with no observations...\n";
                    return false;
                }
                else
                {
                    // Set the particle weight to 1/predictionError (no golf scores...)
                    particle->setWeight(1/newParticleError);
                    totalWeight += particle->getWeight();
                }
            }

            // normalize the particle weights
            for(iter = particles.begin(); iter != particles.end(); iter++)
            {
                Particle* particle = &(*iter);
                particle->normalizeWeight(totalWeight);
                // std::cout << "\t" << particle->getWeight();
            }
            // std::cout << "\n\n";

            //std::cout << "UPDATED\n\n";

            // Succesfully updated particles with Vision!
            return true;
        }

        /**
         * @brief Takes a PVisualObservation and particle & returns the
         *        distance between the observations real location and its
         *        expected one
         */
        float VisionSystem::scoreFromVisDetect(const Particle& particle,
                                               const messages::VisualDetection& obsv)
        {
            // DEBUG VIS DETECT
            //std::cout << "Scoring a visual detection and a particle ---------- \n";

            float bestScore = 100;

            for (int i=0; i<obsv.concrete_coords_size(); i++)
            {
                // std::cout << "Observation (r,b):\t(" << obsv.distance()
                //           << " , " << obsv.bearing() <<")\n";
                // std::cout << "Concrete Obsv(x,y):\t(" << obsv.concrete_coords(i).x() << " , "
                //            << obsv.concrete_coords(i).y() << ")\n";
                // std::cout << "Particle (x,y,h):\t(" << particle.getLocation().x() << " , "
                //           << particle.getLocation().y() << " , "
                //           << particle.getLocation().h() <<")\n";

                // Convert from obsv in polar to rep in cartesian
                // @Todo:  Explain these calculations somewhere!!
                float sin, cos;
                sincosf((particle.getLocation().h() + obsv.bearing()), &sin, &cos);
                // float testSin, testCos;
                // sincosf(ninetyDeg, &testSin, &testCos);
                // std::cout << "sin(90),cos(90)\t" << testSin << "\t" << testCos << "\n";
                float calcX = obsv.distance()*cos + particle.getLocation().x();
                float calcY = obsv.distance()*sin + particle.getLocation().y();

                // // Apply World Rotation and Translation
                // float sinH, cosH;
                // sincosf(particle.getLocation().h(), &sinH, &cosH);
                // float worldFrameRelX = particleFrameRelX*cosH - particleFrameRelY*sinH;
                // float worldFrameRelY = particleFrameRelX*sinH + particleFrameRelY*cosH;

                // float calcX = particle.getLocation().x() + worldFrameRelX;
                // float calcY = particle.getLocation().y() + worldFrameRelY;

                // std::cout << "Calculated FieldX:\t" << calcX << "\tFieldY:\t" << calcY << "\n";
                // std::cout << "Actual FieldX:    \t" << obsv.concrete_coords(i).x() << "\tFieldY:\t" << obsv.concrete_coords(i).y() << "\n";

                // Calc distance between calculated coordinates and the concrete coords
                float dist = std::sqrt(NBMath::square(calcX - obsv.concrete_coords(i).x())
                                       + NBMath::square(calcY - obsv.concrete_coords(i).y()));



                // RelVector relVector = getRelativeVector(particle,
                //                                         obsv.concrete_coords(i).x(),
                //                                         obsv.concrete_coords(i).y());

                // std::cout << "Calc Distance:\t" << relVector.magnitude << "\t VS\t" << obsv.distance() << "\n";
                // std::cout << "Calc bear:\t" << relVector.direction << "\t VS\t" << obsv.bearing() << "\n";

                // float distanceDiff = obsv.distance() - relVector.magnitude;
                // float angleDiff = NBMath::subPIAngle(obsv.bearing() - relVector.direction);

                // std::cout << "DistanceDiff:\t" << distanceDiff << "\tAngleDiff\t" << angleDiff << "\n";

                // boost::math::normal_distribution<float> pDist(0.0f, obsv.distance_sd());
                // float distanceProb = boost::math::pdf<float>(pDist, distanceDiff);


                // boost::math::normal_distribution<float> pAngle(0.0f, obsv.bearing_sd());
                // float angleProb = boost::math::pdf<float>(pAngle, angleDiff);

                // // Better way to determine probability?
                // float score = distanceProb * angleProb;

                // // ***TEMP*** to debug the scoring step
                // std::cout << "Score:\t" << score << "\n"
                //           << "DistanceProb:\t" << distanceProb
                //           << "\tAngleProb:\t" << angleProb << "\n";;

                float score = dist;
                if (score < bestScore)
                    bestScore = score;

            }
            //std::cout <<"Scored a particle\n";
            return bestScore;

        }
    } // namespace localization
} // namespace man
