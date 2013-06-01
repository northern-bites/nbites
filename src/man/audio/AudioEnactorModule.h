#pragma once

#include "RoboGrams.h"
#include "ALSpeech.h"
#include <alcommon/albroker.h>
#include "AudioCommand.pb.h"


namespace man {
namespace audio {

class AudioEnactorModule : public portals::Module {

public:
    AudioEnactorModule(boost::shared_ptr<AL::ALBroker> broker);
    virtual ~AudioEnactorModule() {}

    portals::InPortal<messages::AudioCommand> audioIn;

protected:
    virtual void run_();
    //ALSpeech alspeech;
};

}
}
