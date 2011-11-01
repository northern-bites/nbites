/**
 *
 * @class OfflineManController
 *
 * Provides a way to control the OfflineImageTranscriber and OfflineEnactor
 *
 * @author Octavian Neamtu
 *
 */

#include "corpus/ThreadedImageTranscriber.h"
#include "ClassHelper.h"

namespace man {
namespace corpus{

class OfflineManController {

ADD_SHARED_PTR(OfflineManController);

public:
	OfflineManController();

	void setImageTranscriber(ThreadedImageTranscriber::ptr imTrans) {
		imageTranscriber = imTrans;
	}

	void resetImageTranscriber() {
		imageTranscriber = ThreadedImageTranscriber::ptr();
	}

	void signalNextImageFrame() {
		if (imageTranscriber.get()) {
			imageTranscriber->signalNewImage();
		}
	}

private:
	ThreadedImageTranscriber::ptr imageTranscriber;

};

}
}
