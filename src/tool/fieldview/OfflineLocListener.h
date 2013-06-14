#pragma once

#include "RoboGrams.h"
#include "RobotLocation.pb.h"
#include "ParticleSwarm.pb.h"

namespace tool{
class OfflineLocListener : public portals::Module
{
public:
    OfflineLocListener();

    portals::InPortal<messages::RobotLocation> locIn;
    portals::InPortal<messages::ParticleSwarm> particleIn;

protected:
    virtual void run_();
};
}
