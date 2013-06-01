#pragma once

#include "RoboGrams.h"
#include "AudioCommand.pb.h"


namespace man {
namespace audio {

class AudioEnactorModule : public portals::Module {

public:
    AudioEnactorModule();
    virtual ~AudioEnactorModule() {}

    portals::InPortal<messages::AudioCommand> audioIn;

protected:
    virtual void run_();
};

}
}
