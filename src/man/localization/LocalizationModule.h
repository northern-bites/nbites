/*
 * @brief  The abstract localization module base class.
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   February 2013
 */
#pragma once

#include "../portals/RoboGrams.h"
#include "../memory/protos/Common.pb.h"
#include "../memory/protos/Vision.pb.h"
#include "../memory/protos/Motion.pb.h"

#include "SensorModel.h"
#include "MotionModel.h"
#include "ParticleFilter.h"

namespace man
{
    namespace localization
    {
    /**
     * @class LocalizationModule
     */
    class LocalizationModule : public Module
    {
    public:
        LocalizationModule();
        ~LocalizationModule();

        InPortal<memory::proto::Motion> motionInput;
        InPortal<memory::proto::PVisionField> visionInput;
        OutPortal<memory::proto::RobotLocation> output;

        float lastMotionTimestamp;
        float lastVisionTimestamp;

    protected:
        /**
         * @brief Calls Update
         */
        void run();

        /**
         * @brief Updates the current robot pose estimate given
         *        the most recent motion control inputs and
         *        measurements taken.
         */
        void update();

        SensorModel visionModel;
        MotionModel motionModel;

        ParticleFilter particleFilter;
    };
    } // namespace localization
} // namespace man
