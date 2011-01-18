/*
 * Memory.cpp
 *
 *      Author: oneamtu
 */

#include "Common.h"

#include "Memory.hpp"

namespace memory{

long long int birth_time; //the time we initialized memory
//everything else is time stamped relative to this

Memory::Memory(shared_ptr<Vision> vision_ptr) {
    birth_time = micro_time();
    this->vision = new MVision(vision_ptr);
}

Memory::~Memory() {
    delete vision;
}

void Memory::update(MObject* obj) {
    obj->update();
    obj->log();
}

void Memory::updateVision() {
    update(vision);
}

}
