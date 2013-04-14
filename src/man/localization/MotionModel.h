/**
 * @brief  Defines an abstract interface for modeling robot
 *         motion in localization, which for our purposes is
 *         done by computing odometry measurements in the
 *         motion system.
 * @author Ellis Ratner <eratner@bowdoin.edu>
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   January 2013
 */
#pragma once

#include "RobotLocation.pb.h"

#include "Particle.h"

namespace man
{
    namespace localization
    {

    /**
     * @class MotionModel
     * @brief The abstract interface for providing motion updates
     *        to the particle filter. Must implement a control
     *        update method.
     */
    class MotionModel
    {
    public:
        MotionModel() { }
        virtual ~MotionModel() { }

        /**
         * @brief Update the particles with a the control input
         *        according to the latest motion.
         */
        virtual void update(ParticleSet& particles,
                            messages::RobotLocation& lastOdometry);

        /*
         * These methods allow the client to access information as
         * to whether or not the SensorModel has performed an
         * update on the latest iteration.
         */
       bool hasUpdated() const { return updated; }
       void setUpdated(bool updated_) { updated = updated_; }

    private:
       bool updated;    //! Flag indicates whether or not the particles have
                         //! been updated with the latest sensor readings.

    };

    } // namespace localization
} // namespace man
