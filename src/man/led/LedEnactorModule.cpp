#include "LedEnactorModule.h"
#include <iostream>

namespace man {
	namespace led {

		LedEnactorModule::LedEnactorModule(boost::shared_ptr<AL::ALBroker> broker, portals::OutPortal<messages::LedCommand> out)
			: portals::Module(),
			  naoLights(broker)
		{}

		void LedEnactorModule::run_()
		{
			ledCommandsIn.latch();
			messages::LedCommand command = ledCommandsIn.message();

			std::cout<<command.DebugString()<<std::endl;

			// This might not be the proper way to access proto values
			naoLights.setRGB(command.led_id(),command.rgb_hex());
		}

	}
}
