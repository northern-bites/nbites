#include "TestModule.h"
#include <string>

namespace man {

	TestModule::TestModule()
		: portals::Module(),
		  AudioOut(base())
	{}

	void TestModule::run_()
	{
		//Send some out messages!
		portals::Message<messages::Audio> Audio(0);

		//test that bugger
		std::string msg("this is a test");
		Audio.get()->text_to_speech_msg(msg);

		//now send the message out.
		AudioOut.setMessage(Audio);
	}
}
