
#include "OfflineImageTranscriber.h"

namespace man {
namespace corpus {

OfflineImageTranscriber::OfflineImageTranscriber(boost::shared_ptr<Sensors> s,
		memory::MImage::const_ptr mImage)
		: ThreadedImageTranscriber(s, "OfflineImageTranscriber"),
		  mImage(mImage) {
}

void OfflineImageTranscriber::run() {

	while(running) {
		this->waitForNewImage();
		this->copyNewImage();
	}
}

void OfflineImageTranscriber::signalNewImage() {
	this->signalToResume();
}

void OfflineImageTranscriber::waitForNewImage() {
	this->waitForSignal();
}

void OfflineImageTranscriber::copyNewImage() {

}

}
}
