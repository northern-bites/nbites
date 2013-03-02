#pragma once

#include "V4L2ImageTranscriber.h"

namespace man {
namespace image {

class ImageTranscriberModule : public portals::Module {

public:
	ImageTranscriberModule();
	virtual ~ImageTranscriberModule() {}

	portals::OutPortal<messages::ThresholdedImage> topImageOut;
	portals::OutPortal<messages::ThresholdedImage> bottomImageOut;

protected:
	virtual void run_();
	V4L2ImageTranscriber topImageTranscriber;
	V4L2ImageTranscriber bottomImageTranscriber;
};

}
}
