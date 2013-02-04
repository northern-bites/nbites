#pragma once

#include "RoboGrams.h"
#include "Logger.h"
#include <string>

namespace man {
namespace log {

static std::string FILEPATH = "/home/nao/nbites/frames/";
static std::string EXT = ".frm";

class LogModule : public portals::Module {

public:
	LogModule();
	virtual ~LogModule() {}

    template<class T>
    void addLogger(portals::OutPortal<T>* port, std::string name)
    {
        Logger<T>* log = new Logger<T>(port, name);
        logs.push_back(log);
        log->start();
    }

protected:
    void run_();
    std::vector<LoggerBase*> logs;
};

}
}
