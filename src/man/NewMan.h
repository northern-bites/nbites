#pragma once

#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <boost/shared_ptr.hpp>

#include "RoboGrams.h"
#include "DiagramThread.h"
#include "sensors/SensorsModule.h"
#include "vision/VisionModule.h"
#include "image/ImageTranscriberModule.h"

namespace man {

class Man : public AL::ALModule
{
public:
    Man(boost::shared_ptr<AL::ALBroker> broker, const std::string &name);
    virtual ~Man();

private:
    DiagramThread sensorsThread;
    sensors::SensorsModule sensors;
	vision::VisionModule vision;
	image::ImageTranscriberModule imageTranscriber;
};

}
