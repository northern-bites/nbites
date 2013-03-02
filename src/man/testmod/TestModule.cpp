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

	//test that bugger
	std::string msg("test");
	AudioCommand.get()->set_text_to_speech_msg(msg);
	AudioCommand.get()->set_toggle(bool(true));

	//now send the message out.
	AudioOut.setMessage(AudioCommand);
}
}
}
