/*
 * @brief  The main localization module base class.
 * @author Ellis Ratner <eratner@bowdoin.edu>
 * @date   January 2013
 * @modified EJ Googins <egoogins@bowdoin.edu>
 * @date   January 2013
 */
#pragma once

#include "../portals/RoboGrams.h"
#include "../memory/protos/Common.pb.h"
#include "../memory/protos/Vision.pb.h"

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
        virtual ~LocalizationModule();

        /**
         * @brief Resets the localization.
         */
        virtual void resetLocalization() = 0;

        //InPortal<memory::proto::PMotion> motionInput;
        InPortal<memory::proto::PVision> visionInput;
        OutPortal<memory::proto::RobotLocation> output;

    protected:
        /**
         * @brief Simply calls updateLocalization.
         */
        virtual void run_();

        /**
         * @brief Updates the current robot pose estimate given
         *        the most recent motion control inputs and
         *        measurements taken.
         */
        virtual void updateLocalization(
            /* @todo sort out observation classes */) = 0;
    };
    } // namespace localization
} // namespace man
