#pragma once

#include "RoboGrams.h"
#include "PMotion.pb.h"
#include "ArmContactState.pb.h"

namespace man {
namespace arms {

class ArmContactModule : public portals::Module
{
public:

    ArmContactModule();

    portals::InPortal<messages::JointAngles> actualJointsIn;
    portals::InPortal<messages::JointAngles> expectedJointsIn;
    portals::OutPortal<messages::ArmContactState> contactOut;

protected:
};

}
}
