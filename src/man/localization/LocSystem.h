/**
 * @brief Defines an interface for a localization system
 *
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date February 2013
 */

#pragma once

#include "RoboGrams.h"
#include "Common.pb.h"
#include "Vision.pb.h"
#include "Motion.pb.h"

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
        void updateLocalization(messages::Motion motionInput,
                                messages::PVisionField visionInput);
        virtual void resetLocTo(float x, float y, float h,
                                LocNormalParams params = LocNormalParams()) = 0;
        virtual void resetLocTo(float x, float y, float h,
                                float x_, float y_, float h_,
                                LocNormalParams params1 = LocNormalParams(),
                                LocNormalParams params2 = LocNormalParams()) = 0;
        virtual void resetLocToSide(bool blueSide) = 0;

        // Getters
//         messages::RobotLocation getCurrentEstimate() const { return poseEstimate; }
// //        virtual messages::RobotLocation getCurrentUncertainty() const = 0;

//         float getXEst(){ return poseEstimate.x(); }
//         float getYEst(){ return poseEstimate.y(); }
//         float getHEst(){ return poseEstimate.h(); }

        bool isOnOpposingSide() const { return onOpposingSide; }

        // Setters
        void setOnOpposingSide(bool opp) { onOpposingSide = opp; }


    private:
         // Indicates which side of the field the robot is on.
        // True only if the robot is on the opposing side of the
        // field.
        bool onOpposingSide;
//        messages::RobotLocation poseEstimate;
    };




    } // namespace localization
} // namespace man
