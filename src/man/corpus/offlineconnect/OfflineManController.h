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
	OfflineManController(memory::Memory::const_ptr offline_memory)
		: offline_memory(offline_memory) { }

	memory::Memory::const_ptr getOfflineMemory() const { return offline_memory; }

	void setImageTranscriber(ThreadedImageTranscriber::ptr imTrans) {
		imageTranscriber = imTrans;
	}

	void resetImageTranscriber() {
		imageTranscriber = ThreadedImageTranscriber::ptr();
	}

	void setManMemory(memory::Memory::ptr memory) {
		man_memory = memory;
	}

	void resetManMemory() {
		man_memory = memory::Memory::ptr();
	}

	void signalNextImageFrame() {
		if (imageTranscriber.get()) {
			imageTranscriber->signalNewImage();
		}
	}

private:
	ThreadedImageTranscriber::ptr imageTranscriber;
	memory::Memory::const_ptr offline_memory;
	memory::Memory::ptr man_memory;

};

}
}
