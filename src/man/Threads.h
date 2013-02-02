#pragma once

#include "synchro/synchro.h"
#include "RoboGrams.h"
#include "log/LogModule.h"
#include "sensors/SensorsModule.h"

namespace man {

class CognitionThread : public synchro::Thread
{
public:
    CognitionThread();
    virtual void run();

private:
    // The diagram handles running all of the modules
    portals::RoboGram cognitionDiagram;

};

class SensorsThread : public synchro::Thread
{
public:
    SensorsThread(boost::shared_ptr<AL::ALBroker> broker);
    virtual void run();

private:
    portals::RoboGram sensorsDiagram;

    // Modules:
    image::ImageTranscriberModule imageTranscriber;
    log::LogModule logger;
    sensors::SensorsModule sensors;
};

}
