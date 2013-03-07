/**
 * @brief  Define a class responsible for updating particles based on an
 *         odometry measurement from the motion system
 *
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   February 2013
 */

#pragma once

#include "MotionModel.h"


namespace man
{
    namespace localization
    {

    class MotionSystem
    {
    public:
        MotionSystem();
        ~MotionSystem();

        void update(ParticleSet& particles,
                    messages::RobotLocation deltaMotionInfo);

        void randomlyShiftParticle(Particle* particle);

        void setFallen(bool fallen);

        boost::mt19937 rng;
    private:
        bool robotFallen;
    };
    } // namespace localization
} // namespace man
