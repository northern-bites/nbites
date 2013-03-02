#include "AudioEnactorModule.h"

namespace man {
	namespace audio {

		AudioEnactorModule::AudioEnactorModule(boost::shared_ptr<ALBroker> broker)
			: portals::Module(),
			  alspeech(broker)
		{}

		void LedEnactorModule::run_()
		{
			AudioIn.latch();
			messages::Audio audioCommand = AudioIn.message();

			if(audioCommand.path_to_audio_file()){
				//herp ALSpeech has no method to deal with audio files right now, that's in guardian
			}
			if (audioCommand.text_to_speech_msg()){
				alspeech.say(audioCommand.text_to_speech_msg());
			}
			if (audioCommand.volume()){
				alspeech.setVolume(audioCommand.volume());
			}
		}

	}
}
