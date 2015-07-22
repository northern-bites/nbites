/**
 * Implementation of a particle filter localization
 * system for robot self-localization.
 *
 * @author Ellis Ratner <eratner@bowdoin.edu>
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   May 2012 (updated January 2013)
 */
#pragma once

#include "Particle.h"
#include "FieldConstants.h"
#include "MotionModel.h"
#include "VisionModel.h"
#include "NBMath.h"
#include "DebugConfig.h"

#include "ParticleSwarm.pb.h"

#include <vector>
#include <iostream>
#include <map>
#include <cmath>

#include <boost/shared_ptr.hpp>
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/math/constants/constants.hpp>

namespace man
{
namespace localization
{

// Parameters for the particle filter
static const ParticleFilterParams DEFAULT_PARAMS =
{
    // General particle filter parameters
    200,                        // num particles

    // Particle injection parameters
    0.1f,                       // exponential filter fast
    0.01f,                      // exponential filter slow
    5.0f,                       // learned slow exponential filter value

    // Motion model parameters
#ifdef V5_ROBOT
    0.8f,                       // variance in x-y odometry (cm)
    0.008f,                     // variance in h odometry (radians)
#else
    0.8f,                       // variance in x-y odometry (cm)
    0.012f,                     // variance in h odometry (radians)
#endif

    // Sensor model parameters
    // Line model
    10*TO_RAD,                  // standard deviation of tilt to line (radians)
    20*TO_RAD,                  // standard deviation of bearing to line (radians)
    200,                        // standard deviation of endpoints of line (cm)

    // Landmark model
    10*TO_RAD,                  // standard deviation of tilt to landmark (radians)
    20*TO_RAD,                  // standard deviation of bearing to landmark (radians)
};

/**
 * @class ParticleFilter
 * @brief The main particle filter localization class. Handles
 *        functionality for constructing a posterior belief
 *        based on a prior belief function as well as latest
 *        sensor and control data.
 */
class ParticleFilter// : public LocSystem
{

public:
    ParticleFilter(ParticleFilterParams params = DEFAULT_PARAMS);
    ~ParticleFilter();

    /**
     *  @brief Given a new motion and vision input, update the filter
     */
    void update(const messages::RobotLocation& motionInput,
                messages::Vision&              visionInput,
                const messages::FilteredBall*  ballInput);

    // Overload to use ball info
    // void update(const messages::RobotLocation& motionInput,
    //             const messages::VisionField&   visionInput,
    //             const messages::FilteredBall&    ballInput);

    float getMagnitudeError();

    void resetLocalization();

    void updateMotionModel();

    /** Accessors **/
    const messages::RobotLocation& getCurrentEstimate() const {return poseEstimate;}
    const messages::ParticleSwarm& getCurrentSwarm();

    float getXEst() const {return poseEstimate.x();}
    float getYEst() const {return poseEstimate.y();}
    float getHEst() const {return poseEstimate.h();}
    float getHEstDeg() const {return poseEstimate.h()*TO_DEG;}

    ParticleSet getParticles() { return particles; }

    /**
     * @brief Returns the particle with the highest weight in the set
     *        (i.e., the "best" particle.)
     */
    Particle getBestParticle();

    bool onDefendingSide() {return (poseEstimate.x() < CENTER_FIELD_X);};
    bool nearMidField() {return (fabs(poseEstimate.x() - CENTER_FIELD_X) < 50);};

    /** Reset Functions **/

    /*
     * @Brief - Reset the system by spreading through environment
     */
    void resetLoc();

    /*
     * @Brief - Reset the system by injecting particles around given pose
     */
    void resetLocTo(float x, float y, float h,
                    LocNormalParams params = LocNormalParams());

    /*
     * @Brief - Reset the system by injecting particles around two given pose
     */
    void resetLocTo(float x, float y, float h,
                    float x_, float y_, float h_,
                    LocNormalParams params1 = LocNormalParams(),
                    LocNormalParams params2 = LocNormalParams());

    /*
     * @Brief - Reset the system by injecting particles throughout one side
     */
    void resetLocToSide(bool blueSide);

    /*
     * @Brief - Flip all particles to the other side symmetric location
     */
    void flipLoc();

private:
    /**
     * @brief Resamples (with replacement) the particle population according
     *        to the normalized weights of the particles.
     */
    void resample(bool inSet);

    /**
     * @brief - Update the poseEstimate by avging all particles
     */
    void updateEstimate();

    void updateFieldForDebug(messages::Vision& vision);

    /**
     * @brief - Return symmetric location from given one
     */
    messages::RobotLocation getMirrorLocation(messages::RobotLocation loc);


    ParticleFilterParams params;
    messages::RobotLocation poseEstimate;

    ParticleSet particles;

    MotionModel* motionSystem;
    VisionModel* visionSystem;

    float lastMotionTimestamp;
    float lastVisionTimestamp;

    double wSlow;
    double wFast;

    bool lost;
    bool badFrame;
    float errorMagnitude;

    int framesSinceReset;
    int setResetTransition;

    // For use when logging particle swarm
    messages::ParticleSwarm swarm;

    };
} // namespace localization
} // namespace man
