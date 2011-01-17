/*
 * Memory.h
 *
 *
 *      Author: oneamtu
 */

#pragma once

#include <boost/shared_ptr.hpp>

class Memory;
//#include "MemoryMacros.hpp"
#include "MVision.hpp"

#include "Vision.h"

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
