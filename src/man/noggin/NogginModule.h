#pragma once

#include "LedCommand.ph.h"

namespace man {
	namespace noggin {

		class NogginModule : public portals::Module {

		public:
			NogginModule();
			virtual ~NogginModule() {}

			//placeholder in portal
			portals::InPortal<messages::Ball> ballMessageIn;

		protected:
			virtual void run_();
		};
	}
}
