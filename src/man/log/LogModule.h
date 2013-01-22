#pragma once

#include "RoboGrams.h"
#include "ThresholdedImage.h"
#include <stdint.h>
#include <string>
#include <fstream>

namespace man {
namespace log {

// The location on the robot where logs will be saved
static const std::string FILEPATH = "/home/nao/nbites/frames/";
static const std::string EXT = ".log";
static const std::string LOG_VERSION = "1.0";
static const std::string HEADER = ("NORTHERN BITES IMAGE LOG VERSION " +
                                   LOG_VERSION + " ");

class LogModule : public portals::Module {

public:
	LogModule();
	virtual ~LogModule();

    portals::InPortal<messages::ThresholdedImage> topImageIn;
    portals::InPortal<messages::ThresholdedImage> bottomImageIn;

protected:
    virtual void run_();
    void writeCurrentFrames();
    int saved_frames;
    std::fstream topFile;
    std::fstream bottomFile;
};

}
}
