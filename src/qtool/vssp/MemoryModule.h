#pragma once

#include "man/portals/RoboGrams.h"
#include "Scenes.h"

class MemoryModule : public Module
{
public:
    MemoryModule() : Module(),
                     input(),
                     memory(5),
                     counter(0)
    {
    }

    InPortal<man::memory::proto::PVision> input;

private:
    virtual void run_()
    {
        input.latch();
        //input.message().PrintDebugString();
        memory.addScene(input.message(), counter);
        memory.printInfo();
        SceneIt it = memory.begin();
        while (it != memory.end())
        {
            std::cout << (*it).stamp() << std::endl;
            it++;
        }
        counter++;
    }

    SceneBuffer memory;
    int counter;
};
