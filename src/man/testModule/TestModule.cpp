#include "TestModule.h"

namespace man {

	TestModule::TestModule()
		: portals::Module(),
		  ledCommandsOut(base())
	{}

	void TestModule::run_()
	{
		//Send some out messages!
		portals::Message<messages::LedCommand> ledCommand(0);

		//values taken from Leds.py and ALLedNames.h
		ledCommand.get()->set_led_id(26);//should be chest led
		ledCommand.get()->set_rgb_hex(0xFF00FF);//should be purple

		//now send the message out.
		ledCommandsOut.setMessage(ledCommand);
	}
}
