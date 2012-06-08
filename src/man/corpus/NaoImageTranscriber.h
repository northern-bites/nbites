#pragma once

#include "ThreadedImageTranscriber.h"
#include "V4L2ImageTranscriber.h"

namespace man {
namespace corpus {

class NaoImageTranscriber : public ThreadedImageTranscriber {

ADD_SHARED_PTR(NaoImageTranscriber);

public:
	NaoImageTranscriber(boost::shared_ptr<Sensors> sensors,
                        std::string name);
	virtual ~NaoImageTranscriber() { }

	void run();

    void releaseImage(){}
    void initTable(const std::string&){}

private:
    V4L2ImageTranscriber topImageTranscriber;
    V4L2ImageTranscriber bottomImageTranscriber;

};

}
}
