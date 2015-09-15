/**
 * Implementation of a particle filter localization system for robot localization
 *
 * @author Ellis Ratner <eratner@bowdoin.edu>
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   May 2012
 * @author Josh Imhoff <joshimhoff13@gmail.com>
 * @date   June 2015
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

// NOTE FOR FUTURE DEVELOPERS
// FUTURE WORK
// (1) better debug tools including running the particle filter on
//     logged data and simulated data
// (2) better recovery from kidnapping, probably via the augmented
//     MCL strategy
// (3) see FUTURE WORK tags throughout this directory

// Parameters for the particle filter
static const ParticleFilterParams PARAMS =
{
    // General particle filter parameters
    200,                        // num particles

    // Particle injection parameters
    false,                      // particle injection on
    0.1f,                       // exponential filter fast
    0.01f,                      // exponential filter slow

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

class ParticleFilter
{
public:
    // Constructor
    // @param params, the parameters used by particle filter
    ParticleFilter(ParticleFilterParams params = PARAMS);

    // Destructor
    ~ParticleFilter();

    // Updates particle filter with odometry and sensor (vision) measurements
    // @param motionInput, most recent odometry measurements from motion
    // @param visionInput, most recent sensor measurements from vision
    // @param ballInput, the filtered ball, used as a sensor measurement and 
    //                   landmark when in game set
    // @note visionInput is not const because it is updated for debug tool
    //       purposes, see updateObservationsForDebug() for details
    void update(const messages::RobotLocation& motionInput,
                messages::Vision&              visionInput,
                const messages::FilteredBall*  ballInput);

    // Get robot localization estimate
    // @returns pose estimate of robot
    const messages::RobotLocation& getCurrentEstimate() const { return poseEstimate; }

    // Get full particle swarm
    // @returns the particle swarm
    const messages::ParticleSwarm& getCurrentSwarm();
    
    // Reset swarm to (x, y, h) pose plus gaussian noise
    // @params x, the x component of pose
    // @params y, the y component of pose
    // @params h, the h component of pose
    // @params params, the amount of gaussian noise to add to pose
    void resetLocTo(float x, float y, float h,
                    LocNormalParams params = LocNormalParams());

private:
    // Resample swarm based on updated particle scores
    // @param inSet, true if in game set, allows different injection strategies
    //               depending on game state, see implementation for details
    void resample(bool inSet);

    // Update the localization estimate by averaging the x, y, and h components
    // of all particles in swarm
    void updateEstimate();

    // Update the observations for debug tool purposes, project lines onto global
    // coordinates, set observation correspondence and probabilities, etc.
    void updateObservationsForDebug(messages::Vision& vision);

    // General particle filter data
    ParticleFilterParams params;

    boost::mt19937 rng;

    messages::RobotLocation poseEstimate;
    ParticleSet particles;
    messages::ParticleSwarm swarm;
    // NOTE swarm == particles, swarm is protobuf passed around the cognition thread
    //      while particles is the object used by the particle filter

    // Exponential filters used in particle injection
    double wSlow;
    double wFast;

    // Motion and vision model used to move and score particles respectively
    MotionModel* motionModel;
    VisionModel* visionModel;
};

} // namespace localization
} // namespace man
