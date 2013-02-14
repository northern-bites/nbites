#pragma once

#include <string>
#include <pthread.h>
#include "RoboGrams.h"
#include "log/LogModule.h"

namespace man{

class DiagramThread
{
public:
    DiagramThread(std::string name_);
    virtual ~DiagramThread();

    void addModule(portals::Module& mod);

    int start();
    void run();
    void stop();

    template<class T>
    void log(portals::OutPortal<T>* out, std::string name)
    {
        logs.push_back(new log::LogModule<T>(out, name));
        diagram.addModule(*logs.back());
    }

private:
    static void* runDiagram(void* _this);

    std::vector<log::LogBase*> logs;
    portals::RoboGram diagram;
    pthread_t thread;
    std::string name;
    bool running;
};

}
