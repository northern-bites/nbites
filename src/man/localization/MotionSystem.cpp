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
         * @TODO Currently assumes odometry is how FakeOdometryModule creates it,
         *       Verify when motion module is pulled
         * @return the updated ParticleSet.
         */
        void MotionSystem::update(ParticleSet& particles,
                                  messages::RobotLocation deltaMotionInfo)
        {
//            std::cout<< "MOTION SYSTEM:  x:\t"<<deltaMotionInfo.x() <<"\ty:\t"<<deltaMotionInfo.y()<< "\n";
            ParticleIt iter;
            for(iter = particles.begin(); iter != particles.end(); iter++)
            {
                Particle* particle = &(*iter);

                /** Should be used if odometry gives global **/
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

            // Create random number generators
//            boost::mt19937 rng;

            // TODO: This should be experimentally determined
            boost::uniform_real<float> coordRange(-.0001f, .0001f);
            boost::uniform_real<float> headRange(-.0003f, .0003f);
            boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > coordNoise(rng, coordRange);
            boost::variate_generator<boost::mt19937&, boost::uniform_real<float> > headNoise(rng, headRange);

            // Determine random noise
            messages::RobotLocation noise;
            noise.set_x(coordNoise());
            noise.set_y(coordNoise());
            noise.set_h(NBMath::subPIAngle(headNoise()));

            // std::cout << "X noise:\t" << noise.x() << "\n";

            particle->shift(noise);
        }

    } // namespace localization
} // namespace man
