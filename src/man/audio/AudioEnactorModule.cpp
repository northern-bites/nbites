#include "AudioEnactorModule.h"
#include "Profiler.h"
#include <iostream>
#include <string>

namespace man {
namespace audio {

AudioEnactorModule::AudioEnactorModule() : portals::Module()
{
}

void AudioEnactorModule::run_()
{
    PROF_ENTER(P_AUDIO);

    audioIn.latch();
    messages::AudioCommand audioCommand = audioIn.message();

    if(audioCommand.has_audio_file())
    {
        // system returns an int.
        if(system(("aplay -q "+audioCommand.audio_file()+" &").c_str()) != 0)
            std::cout << "AudioEnactor could not play file." << std::endl;
    }

    PROF_EXIT(P_AUDIO);
}

}
}
