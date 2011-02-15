/*
 * MSensors.cpp
 *
 *      Author: oneamtu
 */

#include "Common.h" //for micro_time
#include "MSensors.hpp"

namespace memory {

using boost::shared_ptr;

extern long long int birth_time;

MSensors::MSensors(shared_ptr<Sensors> s) : sensors(s) {
    fileLogger = new log::FileLogger("Sensors.log", MSENSORS_ID, this);
}

MSensors::~MSensors() {
    delete fileLogger;
}

void MSensors::update() {

    //TODO: should we make this do milisecs instead of
    //micro to save space?
    this->set_timestamp(micro_time() - birth_time);


    std::cout << this->DebugString() << std::endl;
}

void MSensors::log() const {
    fileLogger->write();
}
}
