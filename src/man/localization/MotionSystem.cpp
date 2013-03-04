#include "MotionSystem.h"

namespace man
{
    namespace localization
    {
        MotionSystem::MotionSystem(){}
        MotionSystem::~MotionSystem(){}

        /**
         * Updates the particle set according to the motion.
         *
         * @return the updated ParticleSet.
         */
        void MotionSystem::update(ParticleSet& particles,
                                  messages::RobotLocation deltaMotionInfo)
        {
            ParticleIt iter;
            for(iter = particles.begin(); iter != particles.end(); iter++)
            {
                Particle* particle = &(*iter);

                float sinh, cosh;
                sincosf(deltaMotionInfo.h() - particle->getLocation().h(),
                        &sinh, &cosh);

                float changeX = cosh * deltaMotionInfo.x() + sinh * deltaMotionInfo.y();
                float changeY = cosh * deltaMotionInfo.y() - sinh * deltaMotionInfo.x();
                float changeH = deltaMotionInfo.h();

                randomlyShiftParticle(particle);
                // @TODO NBMath::subPiAngle() the above shit
            }
        }

        void MotionSystem::randomlyShiftParticle(Particle* particle)
        {

            // Create random number generators
            boost::mt19937 rng;
            boost::uniform_real<float> coordRange(-.6f, .6f);
            boost::uniform_real<float> headRange(-1.8f, 1.8f);
            boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > coordNoise(rng, coordRange);
            boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > headNoise(rng, headRange);

            // Determine random noise
            messages::RobotLocation noise;
            noise.set_x(coordNoise());
            noise.set_y(coordNoise());
            noise.set_h(headNoise());

            particle->shiftParticle(noise);
        }

    } // namespace localization
} // namespace man
