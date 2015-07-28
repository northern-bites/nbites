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
// particle class in loc structs file
// particle injection (off by default)
// particle filter refactor
// future work
//
// log_index
// filtered ball in updateFieldForDebug
// no location log (tell Phil)

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
    LocalizationModule();
    ~LocalizationModule();

    portals::InPortal<messages::RobotLocation>  motionInput;
    portals::InPortal<messages::Vision>         visionInput;
    portals::InPortal<messages::RobotLocation>  resetInput[2];
    portals::InPortal<messages::GameState>      gameStateInput;
    portals::InPortal<messages::FilteredBall>   ballInput;

    portals::OutPortal<messages::RobotLocation> output;
    portals::OutPortal<messages::ParticleSwarm> particleOutput;

protected:
    void run_();
    void update();

    ParticleFilter* particleFilter;
    long long lastReset[2];
    messages::Vision curVision;

    size_t log_index;
};

} // namespace localization
} // namespace man
