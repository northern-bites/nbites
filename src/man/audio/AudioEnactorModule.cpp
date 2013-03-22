#include "AudioEnactorModule.h"
#include <iostream>
#include <string>

namespace man {
namespace audio {

AudioEnactorModule::AudioEnactorModule(boost::shared_ptr<AL::ALBroker> broker)
    : portals::Module(),
      alspeech(broker)
{
}

void AudioEnactorModule::run_()
{
    audioIn.latch();
    messages::AudioCommand audioCommand = audioIn.message();

    //set volume if it's different than now - there will always be a volume
    //(default 0.95) but it might not have changed
    if(alspeech.getVolume()!=audioCommand.volume())
    {
        alspeech.setVolume(audioCommand.volume());
    }

    if(audioCommand.has_tts_msg())
    {
        alspeech.say(audioCommand.tts_msg());
    }

    if(audioCommand.has_audio_file())
    {
        // system returns an int.
        if(system(("aplay -q "+audioCommand.audio_file()+" &").c_str()) != 0)
            std::cout << "AudioEnactor could not play file." << std::endl;
    }
}

}
}
