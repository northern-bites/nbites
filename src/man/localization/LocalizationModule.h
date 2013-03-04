/*
 * @brief  The abstract localization module base class.
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   February 2013
 */
#pragma once

#include "RoboGrams.h"
#include "Common.pb.h"
#include "Vision.pb.h"
#include "Motion.pb.h"

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

        portals::InPortal<messages::Motion> motionInput;
        portals::InPortal<messages::PVisionField> visionInput;
        portals::OutPortal<messages::RobotLocation> output;

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

//        boost::shared_ptr<VisionSystem> visionModel;
        boost::shared_ptr<MotionSystem> motionModel;

//        boost::shared_ptr<ParticleFilter> particleFilter;
    };
    } // namespace localization
} // namespace man
