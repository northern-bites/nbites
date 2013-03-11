/**
 * Implementation of a particle filter localization
 * system for robot self-localization.
 *
 * @author Ellis Ratner <eratner@bowdoin.edu>
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   May 2012 (updated January 2013)
 */
#pragma once

#include "SensorModel.h"
#include "MotionModel.h"
#include "Particle.h"
#include "LocSystem.h"
#include "Common.pb.h"
#include "FieldConstants.h"
#include "VisionSystem.h"
#include "MotionSystem.h"

#include "NBMath.h"

#include <vector>
#include <iostream>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/math/constants/constants.hpp>

namespace man
{
    namespace localization
    {

    // Define the default parameters for the Particle Filter
    static const ParticleFilterParams DEFAULT_PARAMS =
    {
        FIELD_GREEN_HEIGHT,
        FIELD_GREEN_WIDTH,
        // ***TEMP*** change num of particles for debugging
        200,
        0.2f,
        0.05f
    };


    /**
     * @class ParticleFilter
     * @brief The main particle filter localization class. Handles
     *        functionality for constructing a posterior belief
     *        based on a prior belief function as well as latest
     *        sensor and control data.
     */
    class ParticleFilter : public LocSystem
    {

    public:
        ParticleFilter(boost::shared_ptr<MotionSystem> motionModel_,
                       boost::shared_ptr<VisionSystem> sensorModel_,
                       ParticleFilterParams parameters = DEFAULT_PARAMS);
        ~ParticleFilter();

        void update(messages::Motion motionInput,
                    messages::PVisionField visionInput);

        ParticleSet getParticles() { return particles; }

        /**
         * @brief Returns the particle with the highest weight in the set
         *        (i.e., the "best" particle.)
         */
        Particle getBestParticle();

        /**
         * @brief Find the standard deviation of the particle set. This is
         *        a useful metric for determining the error in the current
         *        estimate, or the rate of change of error over time.
         */
        std::vector<float> findParticleSD();

        void resetLocalization();

        void updateMotionModel();

        // Getters
        messages::RobotLocation getCurrentEstimate() const {return poseEstimate;}
        float getXEst() const {return poseEstimate.x();}
        float getYEst() const {return poseEstimate.y();}
        float getHEst() const {return poseEstimate.h();}
        float getHEstDeg() const {return poseEstimate.h()*TO_DEG;}

        void resetLoc();
        void resetLocTo(float x, float y, float h,
                        LocNormalParams params = LocNormalParams());
        void resetLocTo(float x, float y, float h,
                        float x_, float y_, float h_,
                        LocNormalParams params1 = LocNormalParams(),
                        LocNormalParams params2 = LocNormalParams());
        void resetLocToSide(bool blueSide);

    private:
        /**
         * @brief Resamples (with replacement) the particle population according
         *        to the normalized weights of the particles.
         */
        void resample();

        void updateEstimate();

        ParticleFilterParams parameters;

        messages::RobotLocation poseEstimate;

        std::vector<float> standardDeviations;

        ParticleSet particles;

        // EJ ----------------------------------------------
        boost::shared_ptr<MotionSystem> motionSystem;
        boost::shared_ptr<VisionSystem> visionSystem;

        float lastMotionTimestamp;
        float lastVisionTimestamp;

        bool updatedVision;
    };
    } // namespace localization
} // namespace man
