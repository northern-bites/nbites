/*
 * Memory.cpp
 *
 *      Author: oneamtu
 */


#include "Memory.hpp"

Memory::Memory(shared_ptr<Vision> vision_ptr) {
    this->vision = shared_ptr<MVision>(new MVision(vision_ptr));
}

/**
 * function updateVision()
 *
 * calls the update function on the vision memory object
 * which in turn
 * updates the vision memory object with all the relevant data
 * from the Man vision object
 *
 */
void Memory::updateVision() {
    vision->update();
}
