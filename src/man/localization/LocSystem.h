/**
 * @brief Defines an interface for a localization system
 *
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date February 2013
 */

#pragma once

#include "../portals/RoboGrams.h"
#include "../memory/protos/Common.pb.h"
#include "../memory/protos/Vision.pb.h"
#include "../memory/protos/Motion.pb.h"

#include <vector>

#include "MotionModel.h"
#include "SensorModel.h"
#include "LocStructs.h"

namespace man
{
    namespace localization
    {

    class LocSystem
    {
        public:
        LocSystem() : onOpposingSide(false){};
        virtual ~LocSystem() {};

       // Core Functions
        void updateLocalization(memory::proto::Motion motionInput,
                                memory::proto::PVisionField visionInput);
        virtual void resetLocTo(float x, float y, float h,
                                LocNormalParams params = LocNormalParams()) = 0;
        virtual void resetLocTo(float x, float y, float h,
                                float x_, float y_, float h_,
                                LocNormalParams params1 = LocNormalParams(),
                                LocNormalParams params2 = LocNormalParams()) = 0;
        virtual void resetLocToSide(bool blueSide) = 0;

        // Getters
        virtual memory::proto::RobotLocation getCurrentEstimate() const    = 0;
        virtual memory::proto::RobotLocation getCurrentUncertainty() const = 0;

        virtual float getXEst() = 0;
        virtual float getYEst() = 0;
        virtual float getHEst() = 0;

        bool isOnOpposingSide() const { return onOpposingSide; }

        // Setters
        void setOnOpposingSide(bool opp) { onOpposingSide = opp; }


    private:
         // Indicates which side of the field the robot is on.
        // True only if the robot is on the opposing side of the
        // field.
        bool onOpposingSide;
    };




    } // namespace localization
} // namespace man
