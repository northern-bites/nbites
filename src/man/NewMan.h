#pragma once

#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <boost/shared_ptr.hpp>

#include "RoboGrams.h"
#include "DiagramThread.h"
#include "sensors/SensorsModule.h"
#include "comm/CommModule.h"
#include "jointenactor/JointEnactorModule.h"
#include "newmotion/MotionModule.h"

namespace man {

class Man : public AL::ALModule
{
public:
    Man(boost::shared_ptr<AL::ALBroker> broker, const std::string &name);
    virtual ~Man();

private:

    void startSubThreads();

    DiagramThread sensorsThread;
    sensors::SensorsModule sensors;
    jointenactor::JointEnactorModule jointEnactor;
    motion::MotionModule motion;

    DiagramThread commThread;
    comm::CommModule comm;
};

}
