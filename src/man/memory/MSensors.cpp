/*
 * MSensors.cpp
 *
 *      Author: oneamtu
 */

#include "Common.h" //for micro_time
#include "MemoryMacros.hpp"
#include "MSensors.hpp"

namespace memory {

using boost::shared_ptr;

MSensors::MSensors(shared_ptr<Sensors> s) : sensors(s) {
    fileLogger = new log::FileLogger("Sensors.log", MSENSORS_ID, this);
}

MSensors::~MSensors() {
    delete fileLogger;
}

void MSensors::update() {

    ADD_PROTO_TIMESTAMP;


    std::cout << this->DebugString() << std::endl;
}

void MSensors::log() const {
    fileLogger->write();
}
}
