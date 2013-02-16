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

    class MotionSystem : public MotionModel
    {
    public:
        MotionSystem();
        ~MotionSystem();

        ParticleSet update(ParticleSet& particles,
                           messages::RobotLocation deltaMotionInformation);

        void setFallen(bool fallen);

    private:
        bool robotFallen;
    };
    } // namespace localization
} // namespace man
