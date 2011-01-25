/**
 * Memory.hpp
 *
 * @class Memory
 *
 * This class keeps instances of all the different memory objects and provides
 * an interface through which they get updated (each memory object pulls data
 * from its corresponding object
 *
 * Future work: we will be able to keep multiple instances of selected objects
 *
 *      Author: Octavian Neamtu
 */

#pragma once

#include <boost/shared_ptr.hpp>

namespace memory {
class Memory; //forward declaration
}

#include "MVision.hpp"
#include "Vision.h"

namespace memory {

using boost::shared_ptr;

class Memory {

public:
    Memory(shared_ptr<Vision> vision_ptr);
    ~Memory();
    void update(MObject* obj);
    void updateVision();

public:
    MVision* vision;
};
}
