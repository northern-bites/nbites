#pragma once

#include "RoboGrams.h"
#include "Audio.pb.h"

namespace man {

	class TestModule : public portals::Module {

	public:
		TestModule();
		virtual ~TestModule() {}

		portals::OutPortal<messages::Audio> AudioOut;

	protected:
		virtual void run_();
	};
}
