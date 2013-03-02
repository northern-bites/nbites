#pragma once

#include "RoboGrams.h"
#include "AudioCommand.pb.h"

namespace man {
namespace testmod{

	class TestModule : public portals::Module {

	public:
		TestModule();
		virtual ~TestModule() {}

		portals::OutPortal<messages::AudioCommand> AudioOut;

	protected:
		virtual void run_();
	};
}
}
