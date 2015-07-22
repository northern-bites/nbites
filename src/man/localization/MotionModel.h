/**
 * @brief  Define a class responsible for updating particles based on
 *         odometry measurements from the motion system
 *
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   February 2013
 * @author Josh Imhoff <joshimhoff13@gmail.com>
 * @date   June 2015
 */

#pragma once

#include "Particle.h"
#include "RobotLocation.pb.h"

#include <cmath>

namespace man
{
namespace localization
{

// FUTURE WORK, this is a very simplistic motion model, future work includes 
//              varying the amount of noise added to particles as a function
//              of (1) the speed of robot and (2) the direction of motion 
//              (more noise in principal direction of motion)

class MotionModel
{
public:
    // Constructor
    // @param params_, the particle filter params, including for motion model
    MotionModel(const struct ParticleFilterParams& params_);

    // Destructor
    ~MotionModel() {}

    // Adjusts swarm based on odometry info from motion
    // @param particles, the set of particles that represent localization belief
    // @param odometry, odometry from last motion frame
    void update(ParticleSet& particles,
                const messages::RobotLocation& odometry);

private:
    // Shift particle by samping from gaussians with standard deviations set by
    // the probalistic motion model
    // @param particle, the particle to shift
    void noiseShift(Particle* particle);

    const struct ParticleFilterParams& params;

    boost::mt19937 rng;

    messages::RobotLocation curOdometry;
    messages::RobotLocation lastOdometry;
};
} // namespace localization
} // namespace man
