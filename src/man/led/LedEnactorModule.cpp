#include "LedEnactorModule.h"

using namespace portals;
using boost::shared_ptr;

namespace man {
	namespace image {

		LedEnactorModule::LedEnactorModule(boost::shared_ptr<ALBroker> broker, portals::OutPortal<messages::LedCommand> out)
			: Module(),
			  naoLights = new NaoLights(broker);
		{
			ledCommandsIn.wireTo(out);
		}

		void LedEnactorModule::run_()
		{
			ledCommandsIn.latch();
			messages::LedCommand command = ledCommandsIn.message();
			// This might not be the proper way to access proto values
			naoLights.setRGB(command.led_id(),command.rgbHex());
		}

	}
}
