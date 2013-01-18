#pragma once

#include "RoboGrams.h"
#include <stdint.h>
#include <string>

namespace man {
namespace log {

static std::string FILEPATH = "/home/nao/nbites/frames/";
static std::string EXT = ".frm";

class LogModule : public portals::Module {

public:
	LogModule();
	virtual ~LogModule() {}

    portals::InPortal<uint16_t> topImageIn;

protected:
    virtual void run_();
    void writeFrame();
    int saved_frames;
};

}
}
