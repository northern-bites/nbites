#pragma once

#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <boost/shared_ptr.hpp>

#include "RoboGrams.h"
#include "DiagramThread.h"
#include "DebugConfig.h"
#include "sensors/SensorsModule.h"
#include "comm/CommModule.h"
#include "vision/VisionModule.h"
#include "new_image/TranscriberModule.h"
#include "new_image/ImageConverterModule.h"
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
    void startAndCheckThread(DiagramThread& thread);

    DiagramThread sensorsThread;
    sensors::SensorsModule sensors;

    DiagramThread guardianThread;
    guardian::GuardianModule guardian;
    audio::AudioEnactorModule audio;

    DiagramThread commThread;
    comm::CommModule comm;

	DiagramThread cognitionThread;
	image::TranscriberModule topTranscriber;
	image::TranscriberModule bottomTranscriber;
    image::ImageConverterModule topConverter;
    image::ImageConverterModule bottomConverter;
	vision::VisionModule vision;
};

}
