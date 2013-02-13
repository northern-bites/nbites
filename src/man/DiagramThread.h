#pragma once

#include <string>
#include <pthread.h>
#include "RoboGrams.h"

namespace man{

class DiagramThread
{
public:
    DiagramThread(std::string name_);
    virtual ~DiagramThread();

    void addModule(portals::Module& mod);

    int start();
    void stop();

private:
    static void* runDiagram(void* _this);

    portals::RoboGram diagram;
    pthread_t thread;
    std::string name;
    bool running;
};

}
