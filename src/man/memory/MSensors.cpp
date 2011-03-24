/*
 * MSensors.cpp
 *
 *      Author: oneamtu
 */

#include "MemoryMacros.hpp"
#include "MSensors.hpp"
#include <vector>

namespace memory {

using boost::shared_ptr;
using namespace std;

MSensors::MSensors(shared_ptr<Sensors> s) :
        MMotionSensors(s),
        MVisionSensors(s),
        sensors(s) {

    //fileLogger = new log::FileLogger("/home/nao/Sensors.log", MSENSORS_ID, this);
}

MSensors::~MSensors() {
    //delete fileLogger;
}

void MSensors::update(const ProviderEvent e) {

    if (e.getType() == NEW_MOTION_SENSORS) {
        this->MMotionSensors::update();
        this->MMotionSensors::log();
    }

    if (e.getType() == NEW_VISION_SENSORS) {
        this->MVisionSensors::update();
        this->MVisionSensors::log();
    }
}
/*
void MSensors::log() const {
    fileLogger->write();
}*/
}

