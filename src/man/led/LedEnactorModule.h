#pragma once

namespace man {
	namespace LED {

		class LedEnactorModule : public portals::Module {

		public:
			LedEnactorModule();
			virtual ~LedEnactorModule() {}

			portals::InPortal<messages::LedMessage> LedCommandsIn;
		};

	}
}
