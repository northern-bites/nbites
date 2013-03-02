#pragma once

#include "RoboGrams.h"
#include "ALSpeech.h"
#include <alcommon/albroker.h>
#include "Audio.pb.h"

namespace man {
	namespace audio {

		class AudioEnactorModule : public portals::Module {

		public:
			AudioEnactorModule(boost::shared_ptr<AL::ALBroker> broker);
			virtual ~AudioEnactorModule() {}

			portals::InPortal<messages::Audio> AudioIn;

		protected:
			virtual void run_();
			ALSpeech alspeech;
		};

	}
}
