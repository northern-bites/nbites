/**
 * @brief  Define a class responsible for updating particle swarm based on
 *         visual observations from the vision system
 *
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   February 2013
 */

#pragma once

#include "SensorModel.h"
#include "NBMath.h"

#include <vector>



namespace man
{
    namespace localization
    {

    /**
     * @class Vision System
     * @brief Responsible for updating particles based on Visual Observations
     *        from the vision system.
     */
        class VisionSystem// : public SensorModel
    {
    public:
        VisionSystem();
        ~VisionSystem();

        ParticleSet update(ParticleSet& particles,
                           messages::PVisionField observations);

        float scoreFromVisDetect(const Particle& particle,
                                 const messages::PVisualDetection& obsv);

        RelVector getRelativeVector(const Particle& particle,
                                    float fieldX, float fieldY);

        void setUpdated(bool val);

        boost::mt19937 rng;

    private:
        float lastVisionTimestamp;
        bool updated;

     };
    } // namespace localization
} // namespace man
