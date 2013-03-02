#include "TestModule.h"
#include "SoundPaths.h"
#include <string>

namespace man {
namespace testmod {

TestModule::TestModule()
	: portals::Module(),
	  AudioOut(base())
{}

void TestModule::run_()
{
	//Send some out messages!
	portals::Message<messages::AudioCommand> AudioCommand(0);
	AudioCommand.get()->Clear();

	//test tts and audio
	if(!msgSent){
		std::string msg("test");
		AudioCommand.get()->set_tts_msg(msg);
		AudioCommand.get()->set_audio_file(shutdown_wav);
	}

	//now send the message out.
	AudioOut.setMessage(AudioCommand);
	msgSent = true;
}
}
}
