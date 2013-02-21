#include "TestModule.h"

using namespace portals;

namespace man {

	TestModule::TestModule()
		: Module();
	{}

	void TestModule::run_()
	{
		//Send some out messages!
		portals::Message<messages::LedCommand> ledCommand(0);

		*ledCommand.get() = messages::LedCommand();

		//values taken from Leds.py and ALLedNames.h
		ledCommand.get()->set_led_id(26);//should be chest led
		ledCommand.get()->set_rgbHex(0xFF00FF);//should be purple
	}
}
