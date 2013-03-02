#include "AudioEnactorModule.h"
#include <iostream>

namespace man {
namespace audio {

AudioEnactorModule::AudioEnactorModule(boost::shared_ptr<AL::ALBroker> broker)
	: portals::Module(),
	  alspeech(broker)
{}

void AudioEnactorModule::run_()
{
	AudioIn.latch();
	messages::AudioCommand audioCommand = AudioIn.message();

	//audioenactor implements a toggle (logic gate) mechanism
	//every time you send a message, you also must flip the toggle

	//set volume if it's different than now - there will always be a volume
	//(default 0.95) but it might not have changed
	if(alspeech.getVolume()!=audioCommand.volume()){
		alspeech.setVolume(audioCommand.volume());
	}

	//only process the message if the toggle is flipped
	if(toggle!=audioCommand.toggle()){
		if(audioCommand.has_text_to_speech_msg()){
			alspeech.say(audioCommand.text_to_speech_msg());
		}
		if(audioCommand.has_path_to_audio_file()) {
		    
		}
	}



	/*if(audioCommand.path_to_audio_file()){
		//herp ALSpeech has no method to deal with audio files right now, that's in guardian
	}
	if (audioCommand.text_to_speech_msg()){
		alspeech.say(audioCommand.text_to_speech_msg());
		}*/


	toggle = audioCommand.toggle();

}

}
}
