#pragma once

#include ../corpus/alconnect/NaoLights.h

namespace man {
	namespace LED {

		class LedEnactorModule : public portals::Module {

		public:
			LedEnactorModule(boost::shared_ptr<ALBroker> broker);
			virtual ~LedEnactorModule() {}

			portals::InPortal<messages::LedMessage> ledCommandsIn;

		protected:
			virtual void run_();
			NaoLights naoLights;
		};

	}
}
