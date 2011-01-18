/*
 * Memory.cpp
 *
 *      Author: oneamtu
 */


#include "Memory.hpp"

namespace memory{

Memory::Memory(shared_ptr<Vision> vision_ptr) {
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
