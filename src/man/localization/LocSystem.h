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
        void updateLocalization(messages::RobotLocation& motionInput,
                                messages::VisionField& visionInput);
        virtual void resetLoc() = 0;
        virtual void resetLocTo(float x, float y, float h,
                                LocNormalParams params = LocNormalParams()) = 0;
        virtual void resetLocTo(float x, float y, float h,
                                float x_, float y_, float h_,
                                LocNormalParams params1 = LocNormalParams(),
                                LocNormalParams params2 = LocNormalParams()) = 0;
        virtual void resetLocToSide(bool blueSide) = 0;

        // Getters
        virtual messages::RobotLocation& getCurrentEstimate() const = 0;
        virtual float getXEst() const = 0;
        virtual float getYEst() const = 0;
        virtual float getHEst() const = 0;
        virtual float getHEstDeg() const = 0;

        // virtual messages::RobotLocation& getCurrentUncertainty() const = 0;
        // virtual float getXUncert() const = 0;
        // virtual float getYUncert() const = 0;
        // virtual float getHUncert() const = 0;
        // virtual float getHUncertDeg() const = 0;

        bool isOnOpposingSide() const { return onOpposingSide; }

        // Setters
        void setOnOpposingSide(bool opp) { onOpposingSide = opp; }

        friend std::ostream& operator<< (std::ostream &o,
                                         const LocSystem &c) {
            return o << "Est: (" << c.getXEst() << ", " << c.getYEst() << ", "
                     << c.getHEst() << ")\n";
        }


    private:
         // Indicates which side of the field the robot is on.
        // True only if the robot is on the opposing side of the
        // field.
        bool onOpposingSide;
    };




    } // namespace localization
} // namespace man
