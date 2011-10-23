/**
 *
 * @class OfflineTranscriber
 *
 * @author Octavian Neamtu
 *
 */

#pragma once

#include "ThreadedImageTranscriber.h"
#include "ClassHelper.h"
#include "memory/MImage.h"

namespace man {
namespace corpus {

class OfflineImageTranscriber : public ThreadedImageTranscriber {

ADD_SHARED_PTR(OfflineImageTranscriber);

public:
	OfflineImageTranscriber(boost::shared_ptr<Sensors> s,
			memory::MImage::const_ptr mImage);
	virtual ~OfflineImageTranscriber();

	void releaseImage() {}
	void run();
	void signalNewImage();
	void waitForNewImage();

protected:
	void copyNewImage();

private:
	memory::MImage::const_ptr mImage;
};

}
}
