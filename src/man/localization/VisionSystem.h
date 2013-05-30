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

#include <boost/math/distributions.hpp>

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

        // updates give particleset by reference, returns true if observations was nonempty
        bool update(ParticleSet& particles,
                    const messages::VisionField& observations);

        float scoreFromVisDetect(const Particle& particle,
                                 const messages::VisualDetection& obsv);
        void setUpdated(bool val);
        float getLowestError(){return currentLowestError;};

        // Random number generator to be used throughout the system
        boost::mt19937 rng;
    private:
        float currentLowestError;
     };
    } // namespace localization
} // namespace man
