#pragma once

#include "synchro/synchro.h"
#include "RoboGrams.h"
#include "TestModules.h"

namespace man {

class TestThread : public synchro::Thread
{
public:
    TestThread();
    virtual void run();

private:
    portals::RoboGram testDiagram;
    FirstModule firstMod;
    SecondModule secondMod;
};

}
