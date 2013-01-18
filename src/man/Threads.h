#pragma once

#include "synchro/synchro.h"
#include "RoboGrams.h"
#include "image/ImageTranscriberModule.h"

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
};

}
