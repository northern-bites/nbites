#include "OfflineTranscriber.h"

using boost::shared_ptr;
using namespace std;
using namespace man::memory;

namespace man {
namespace corpus {

OfflineTranscriber::OfflineTranscriber(boost::shared_ptr<Sensors> s,
			MVisionSensors::const_ptr mvisionSensors,
			MMotionSensors::const_ptr mmotionSensors)
    : Transcriber(s),
      mvisionSensors(mvisionSensors),
      mmotionSensors(mmotionSensors)
{ }

void OfflineTranscriber::postVisionSensors() {
//	mvisionSensors->copyTo(sensors);
//	sensors->notifySubscribers(NEW_VISION_SENSORS);
}

void OfflineTranscriber::postMotionSensors() {
}

}
}
