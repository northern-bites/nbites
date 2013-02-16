#include "MotionSystem.h"
#include "Particles.h"

namespace man
{
    namespace localization
    {
        MotionSystem::MotionSystem(){};
        MotionSystem::~MotionSystem(){};

        /**
         * Updates the particle set according to the motion.
         *
         * @return the updated ParticleSet.
         */
        ParticleSet MotionSystem::update(ParticleSet& particles,
                                         messages::RobotLocation deltaMotionInformation)
        {

            if (visionInput.timestamp() > lastVisionTimestamp)
            {
                ParticleIt iter;
                for(iter = particles.begin(); iter != particles.end(); iter++)
                {
                    Particle* particle = &(*iter);

                    float sinh, cosh;
                    sincosf(deltaMotionInformation.h() - particle->getLocation().h(),
                            &sinh, &cosh);

                    float changeX = cosh * deltaOdometry.x() + sinh * deltaOdometry.y();
                    float changeY = cosh * deltaOdometry.y() - sinh * deltaOdometry.x();
                    float changeH = deltaOdometry.h();

                    randomlyShiftParticle(& particle);
                    // @TODO NBMath::subPiAngle() the above shit

                }
            }

        }

        void MotionSysem::randomlyShiftParticle(Particle* particle)
        {
            // Create random number generators
            boost::mt19937 rng;
            boost::uniform_real<float> coordRange(-.6, .6);
            boost::uniform_real<float> headRange(-1.8, 1.8);
            boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > coordNoise(rng, coordRange);
            boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > headNoise(rng, headRange);

            // Determine random noise
            messages::RobotLocation randomShiftAmount;
            randomShiftAmount.set_x(coordNoise());
            randomShiftAmount.set_y(coordNoise());
            randomShiftAmount.set_h(headNoise());

            particle->shiftParticle(noise);
        }

    } // namespace localization
} // namespace man
