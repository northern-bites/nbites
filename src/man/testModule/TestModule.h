#pragma once

#include "RoboGrams.h"
#include "LedCommand.pb.h"

namespace man {

	class TestModule : public portals::Module {

	public:
		TestModule();
		virtual ~TestModule() {}

		portals::OutPortal<messages::LedCommand> ledCommandsOut;

	protected:
		virtual void run_();
	};
}
