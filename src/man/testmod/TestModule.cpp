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

	if(!msgSent){
		std::string msg2("soft message");
		AudioCommand.get()->set_tts_msg(msg2);
		AudioCommand.get()->set_volume(.3f);
		AudioOut.setMessage(AudioCommand);
	}


	msgSent = true;
}
}
}
