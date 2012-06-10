#pragma once

#include "ThreadedImageTranscriber.h"
#include "V4L2ImageTranscriber.h"

#include "memory/MObjects.h"
#include "memory/MemoryProvider.h"

namespace man {
namespace corpus {

class NaoImageTranscriber : public ThreadedImageTranscriber {

ADD_SHARED_PTR(NaoImageTranscriber);

public:
	NaoImageTranscriber(boost::shared_ptr<Sensors> sensors,
                        std::string name,
                        memory::MRawImages::ptr rawImages);
	virtual ~NaoImageTranscriber() { }

	void run();

    void releaseImage(){}
    void initTable(const std::string&){}

    //memory update
    void updateMRawImages(memory::MRawImages::ptr) const;

private:
    V4L2ImageTranscriber topImageTranscriber;
    V4L2ImageTranscriber bottomImageTranscriber;

    memory::MemoryProvider<memory::MRawImages, NaoImageTranscriber> memoryProvider;
    memory::MRawImages::ptr rawImages;
};

}
}
