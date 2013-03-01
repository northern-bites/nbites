#include "AudioEnactorModule.h"

using namespace portals;
using boost::shared_ptr;

namespace man {
	namespace audio {

		AudioEnactorModule::AudioEnactorModule(boost::shared_ptr<ALBroker> broker, portals::OutPortal<messages::Audio> out)
			: Module(),
			  alspeech = new AlSpeech(broker);
		{}

		void LedEnactorModule::run_()
		{
			AudioIn.latch();
			messages::Audio audioCommand = AudioIn.message();

			if(audioCommand.path_to_audio_file()){
				//herp ALSpeech has no method to deal with audio files right now, that's in guardian
			}
			if (audioCommand.text_to_speech_msg()){
				alspeech.say(command.text_to_speech_msg());
			}
			if (audioCommand.volume()){
				alspeech.setVolume(audioCommand.volume())
			}
		}

	}
}
