#pragma once

#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <boost/shared_ptr.hpp>

#include "RoboGrams.h"
#include "DiagramThread.h"
#include "sensors/SensorsModule.h"
#include "comm/CommModule.h"
#include "vision/VisionModule.h"
#include "image/ImageTranscriberModule.h"
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
    DiagramThread guardianThread;
    DiagramThread commThread;
	DiagramThread cognitionThread;


    guardian::GuardianModule guardian;
    audio::AudioEnactorModule audio;
    comm::CommModule comm;
    sensors::SensorsModule sensors;
	image::ImageTranscriberModule imageTranscriber;
	vision::VisionModule vision;
};

}
