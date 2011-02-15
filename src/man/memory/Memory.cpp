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

Memory::Memory(shared_ptr<Vision> vision_ptr,
        shared_ptr<Sensors> sensors_ptr) {
    birth_time = micro_time();
    this->mvision = new MVision(vision_ptr);
    this->msensors = new MSensors(sensors_ptr);
}

Memory::~Memory() {
    delete mvision;
    delete msensors;
}

void Memory::update(MObject* obj) {
    obj->update();
    obj->log();
}

void Memory::updateVision() {
    update(mvision);
}

void Memory::updateSensors() {
    update(msensors);
}

}
