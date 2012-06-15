/**
 * @class RobotMemory
 *
 * This instantiates the most common used MemoryObjects
 *
 * @author Octavian Neamtu
 */

#pragma once

#include "Memory.h"
#include "MObjects.h"
#include "Profiler.h"

namespace man {
namespace memory {

//TODO: better way of doing this

//this determines the ports the streams will use in streaming
//modify this with care
//add new objects if you want them streamed
static const std::string streamableObjects[] = {
        "MVision",
        "MRawImages",
        "MVisionSensors",
        "MMotionSensors",
        "MLocalization",
        "MMotion"
};

static const int numStreamableObjects = 6;

class RobotMemory : public Memory {

    ADD_SHARED_PTR(RobotMemory)

public:
    RobotMemory();
    virtual ~RobotMemory();
};


}
}
