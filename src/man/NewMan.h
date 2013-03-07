#pragma once

#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <boost/shared_ptr.hpp>

#include "RoboGrams.h"
#include "DiagramThread.h"
#include "sensors/SensorsModule.h"
#include "comm/CommModule.h"
#include "led/LedEnactorModule.h"
#include "behaviors/BehaviorsModule.h"
#include "guardian/GuardianModule.h"
#include "audio/AudioEnactorModule.h"

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

    DiagramThread guardianThread;
    guardian::GuardianModule guardian;
    audio::AudioEnactorModule audio;

    DiagramThread commThread;
    comm::CommModule comm;

    DiagramThread cognitionThread;
	led::LedEnactorModule leds;
	behaviors::BehaviorsModule behaviors;
};

}
