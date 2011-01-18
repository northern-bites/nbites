/*
 * Memory.hpp
 *
 *
 *      Author: oneamtu
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
