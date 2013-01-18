#pragma once

#include "V4L2ImageTranscriber.h"
#include "RoboGrams.h"

namespace man {
namespace image {

class NaoImageTranscriber : public portals::Module {

public:
	NaoImageTranscriber();
	virtual ~NaoImageTranscriber() {}

    portals::OutPortal<uint16_t> topImageOut;
    portals::OutPortal<uint16_t> bottomImageOut;

protected:
    virtual void run_();
    V4L2ImageTranscriber topImageTranscriber;
    V4L2ImageTranscriber bottomImageTranscriber;
};

}
}
