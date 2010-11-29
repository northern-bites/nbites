/*
 * Memory.cpp
 *
 *      Author: oneamtu
 */


#include "Memory.hpp"

Memory::Memory(shared_ptr<Vision> vision_ptr) {
    this->vision = shared_ptr<MVision>(new MVision(vision_ptr));
}
