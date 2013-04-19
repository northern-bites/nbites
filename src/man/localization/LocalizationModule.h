/*
 * @brief  The abstract localization module base class.
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   February 2013
 */
#pragma once

#include "DebugConfig.h"

#include "RoboGrams.h"
#include "VisionField.pb.h"
#include "RobotLocation.pb.h"
#include "ParticleSwarm.pb.h"

#include "SensorModel.h"
#include "MotionModel.h"
#include "ParticleFilter.h"
#include "VisionSystem.h"
#include "MotionSystem.h"

#include <boost/shared_ptr.hpp>

namespace man
{
    namespace localization
    {
    /**
     * @class LocalizationModule
     */
    class LocalizationModule : public portals::Module
    {
    public:
        LocalizationModule();
        ~LocalizationModule();

        portals::InPortal<messages::RobotLocation> motionInput;
        portals::InPortal<messages::VisionField> visionInput;
        portals::InPortal<messages::RobotLocation> resetInput;

        portals::OutPortal<messages::RobotLocation> output;

        portals::OutPortal<messages::ParticleSwarm> particleOutput;

        float lastMotionTimestamp;
        float lastVisionTimestamp;

    protected:
        /**
         * @brief Calls Update
         */
        void run_();

        /**
         * @brief Updates the current robot pose estimate given
         *        the most recent motion control inputs and
         *        measurements taken.
         */
        void update();

        ParticleFilter * particleFilter;
        long long lastReset;

        messages::RobotLocation lastOdometry;
        messages::RobotLocation curOdometry;
        messages::RobotLocation deltaOdometry;
    };
    } // namespace localization
} // namespace man
