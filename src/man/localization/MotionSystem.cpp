#include "MotionSystem.h"

namespace man
{
    namespace localization
    {
        MotionSystem::MotionSystem(float xAndYNoise_, float hNoise_)
        {
            xAndYNoise = xAndYNoise_;
            hNoise = hNoise_;
        }
        MotionSystem::~MotionSystem(){}

        /**
         * Updates the particle set according to the motion.
         *
         * @return the updated ParticleSet.
         */
        void MotionSystem::update(ParticleSet& particles,
                                  const messages::RobotLocation& deltaMotionInfo,
                                  bool lost)
        {
            ParticleIt iter;
            for(iter = particles.begin(); iter != particles.end(); iter++)
            {
                Particle* particle = &(*iter);

                /** Should be used if odometry gives global **/
                /** Should also be TESTED extensively       **/
                // float sinh, cosh;
                // sincosf(deltaMotionInfo.h() - particle->getLocation().h(),
                //         &sinh, &cosh);
                // float changeX = cosh * deltaMotionInfo.x() + sinh * deltaMotionInfo.y();
                // float changeY = cosh * deltaMotionInfo.y() - sinh * deltaMotionInfo.x();

                float changeX = deltaMotionInfo.x();
                float changeY = deltaMotionInfo.y();
                float changeH = deltaMotionInfo.h();

                particle->shift(changeX, changeY, changeH);
                randomlyShiftParticle(particle);
            }
//            std::cout << "\n\n Updated Particles w/ Motion \n";
        }

        void MotionSystem::randomlyShiftParticle(Particle* particle)
        {
            // TODO: This should be experimentally determined
            boost::uniform_real<float> coordRange(-1.f * xAndYNoise, xAndYNoise);
            boost::uniform_real<float> headRange (-1.f * hNoise    , hNoise);
            boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > coordNoise(rng, coordRange);
            boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > headNoise(rng, headRange);

            // Determine random noise
            messages::RobotLocation noise;
            noise.set_x(coordNoise());
            noise.set_y(coordNoise());
            noise.set_h(NBMath::subPIAngle(headNoise()));

            particle->shift(noise);
        }

    } // namespace localization
} // namespace man
