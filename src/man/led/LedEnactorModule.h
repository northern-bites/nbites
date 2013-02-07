#pragma once

#include "../corpus/alconnect/NaoLights.h"
#include "alcommon/albroker.h"
#include "LedCommand.pb.h"

namespace man {
	namespace led {

		class LedEnactorModule : public portals::Module {

		public:
			LedEnactorModule(boost::shared_ptr<AL::ALBroker> broker);
			virtual ~LedEnactorModule() {}

			portals::InPortal<messages::LedCommand> ledCommandsIn;

		protected:
			virtual void run_();
			NaoLights naoLights;
		};

	}
}
