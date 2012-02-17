/**
 *
 * @class OfflineManController
 *
 * Provides a way to control the ThreadedImageTranscriber and OfflineEnactor
 *
 * @author Octavian Neamtu
 *
 */

#pragma once

#include <string>
#include "corpus/ThreadedImageTranscriber.h"
#include "ClassHelper.h"
#include "man/memory/Memory.h"

namespace man {
namespace corpus{

class OfflineManController {

ADD_SHARED_PTR(OfflineManController);

public:
	OfflineManController(memory::Memory::const_ptr offline_memory)
		: fake_memory(offline_memory) { }

	memory::Memory::const_ptr getFakeMemory() const { return fake_memory; }

	void loadTable(const std::string &path) {
	    if (imageTranscriber.get()) {
	        imageTranscriber->initTable(path);
	    }
	}

	void setImageTranscriber(ThreadedImageTranscriber::ptr imTrans) {
		imageTranscriber = imTrans;
	}

	void resetImageTranscriber() {
		imageTranscriber = ThreadedImageTranscriber::ptr();
	}

	memory::Memory::ptr getManMemory() const {
	    return man_memory;
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
	memory::Memory::const_ptr fake_memory;
	memory::Memory::ptr man_memory;

};

}
}
