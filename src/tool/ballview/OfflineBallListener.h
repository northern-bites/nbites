#pragma once

#include "RoboGrams.h"
#include "RobotLocation.pb.h"
#include "BallModel.pb.h"

namespace tool{
class OfflineBallListener : public portals::Module
{
public:
    OfflineBallListener();

    portals::InPortal<messages::FilteredBall> ballIn;

protected:
    virtual void run_();
};
}
