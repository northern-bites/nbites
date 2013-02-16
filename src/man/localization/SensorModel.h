/**
 * @brief  Defines an abstract sensor model interface
 *
 * @author Ellis Ratner <eratner@bowdoin.edu>
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   January 2013
 */
#pragma once

#include "Common.pb.h"
#include "Vision.pb.h"

#include "Particle.h"

namespace man
{
    namespace localization
    {
    /**
     * @class SensorModel
     * @brief The abstract interface for providing sensor
     *        (e.g., vision) updates to the particle filter.
     */
    class SensorModel
    {
    public:
        SensorModel() { }
        virtual ~SensorModel() { }

        /**
         * @brief Update the particles with the latest measurement
         *        taken by the robot's sensors.
         */
        virtual ParticleSet update(ParticleSet& particles,
                                   messages::PVisionField visionInput) = 0;

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
