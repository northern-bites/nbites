/*
 * MSensors.cpp
 *
 *      Author: oneamtu
 */

#include "Common.h" //for micro_time
#include "MemoryMacros.hpp"
#include "MMotionSensors.hpp"

#include <vector>

namespace memory {

using boost::shared_ptr;
using namespace std;

MMotionSensors::MMotionSensors(shared_ptr<Sensors> s) : sensors(s) {
    fileLogger = new log::FileLogger("/home/nao/MotionSensors.log", MMOTION_SENSORS_ID, this);
}

MMotionSensors::~MMotionSensors() {
    delete fileLogger;
}

void MMotionSensors::update() {

    ADD_PROTO_TIMESTAMP;

    vector<float> bodyAngles = sensors->getBodyAngles();
    for (vector<float>::iterator i = bodyAngles.begin(); i != bodyAngles.end(); i++) {
        this->add_body_angles(*i);
    }

    std::cout << this->DebugString() << std::endl;
}

void MMotionSensors::log() const {
    fileLogger->write();
}
}
