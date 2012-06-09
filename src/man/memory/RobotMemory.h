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


class RobotMemory : public Memory {

    ADD_SHARED_PTR(RobotMemory)

public:
    RobotMemory();
    virtual ~RobotMemory();
};


}
}
