#pragma once

#include "synchro/synchro.h"
#include "RoboGrams.h"
#include "image/ImageTranscriberModule.h"
#include "log/LogModule.h"
#include "comm/CommModule.h"

namespace man {

class CognitionThread : public synchro::Thread
{
public:
    CognitionThread();
    virtual void run();

private:
    // The diagram handles running all of the modules
    portals::RoboGram cognitionDiagram;

    // Modules:
    image::ImageTranscriberModule imageTranscriber;
    log::LogModule logger;
};

class CommThread : public synchro::Thread
{
public:
    CommThread();
    virtual void run();

private:
    // The diagram handles running all of the modules
    portals::RoboGram commDiagram;

    // Modules:
    comm::CommModule comm;
};

}
