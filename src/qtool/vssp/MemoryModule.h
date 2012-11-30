#pragma once

#include "man/portals/RoboGrams.h"

class MemoryModule : public Module
{
public:
    MemoryModule() : Module(),
                     input()
    {
    }

    InPortal<PVision> input;

private:
    virtual void run_()
    {
        input.latch();
        input.message().PrintDebugString();
    }
};
