/*
 * @brief  The localization module, calls the particle filter and outputs
 *         localization estimate and swarm
 *
 * @author EJ Googins <egoogins@bowdoin.edu>
 * @date   February 2013
 * @author Josh Imhoff <joshimhoff13@gmail.com>
 * @date   June 2015
 */

#pragma once

// TODO
//
// YES
// sampleNormal -> ReconstructedLocation -> PoseWithGaussianNoise
// future work
// pid controller
//
// MAYBE
// T corners in center circle rename
// LineID -> VisionLineID
// particle class in loc structs file
// log_index
// filtered ball in updateFieldForDebug

#include "RoboGrams.h"
#include "Vision.pb.h"
#include "RobotLocation.pb.h"
#include "GameState.pb.h"
#include "ParticleSwarm.pb.h"
#include "ParticleFilter.h"

namespace man
{
namespace localization
{

class LocalizationModule : public portals::Module
{
public:
    // Constructor
    LocalizationModule();

    // Destructor
    ~LocalizationModule();

    portals::InPortal<messages::RobotLocation>  motionInput;
    portals::InPortal<messages::Vision>         visionInput;
    portals::InPortal<messages::RobotLocation>  resetInput[2];
    portals::InPortal<messages::GameState>      gameStateInput;
    portals::InPortal<messages::FilteredBall>   ballInput;

    portals::OutPortal<messages::RobotLocation> output;
    portals::OutPortal<messages::ParticleSwarm> particleOutput;

protected:
    // RoboGrams run method
    void run_();

    // Update localization system, called by run_() and calls particle filter
    void update();

    ParticleFilter* particleFilter;
    long long lastReset[2];
    messages::Vision curVision;

    size_t log_index;
};

} // namespace localization
} // namespace man
