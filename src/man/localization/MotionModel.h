/**
 * @brief  Defines an abstract interface for modeling robot
 *         motion in localization, which for our purposes is
 *         done by computing odometry measurements in the
 *         motion system.
 * @author Ellis Ratner <eratner@bowdoin.edu>
 * @date   January 2013
 */
#pragma once

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
        virtual ParticleSet update(ParticleSet& particles) = 0;

        /**
         * @brief Get the last odometry measurement.
         */
//        virtual const OdometryMeasurement& getLastOdometry() const = 0;
    };

    /**
     * @class OdometryModel
     * @brief Responsible for updating particles based on an odometry
     *        measurement from the motion system.
     */
    class OdometryModel : public MotionModel
    {
    public:
        OdometryModel();
        ~OdometryModel();

        ParticleSet update(ParticleSet& particles);

        /**
         * @brief When a robot falls, it tends to rotate, altering its
         *        heading in a manner not measureable by conventional
         *        odometry. Therefore, we will account for this by adding
         *        additional noise/uncertainty to the heading following
         *        a fall.
         *
         * @param fallen Whether or not the robot is fallen.
         */
        void setFallen(bool fallen);

    private:
        bool robotFallen;
    };
    } // namespace localization
} // namespace man
