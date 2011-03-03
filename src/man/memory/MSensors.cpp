/*
 * MSensors.cpp
 *
 *      Author: oneamtu
 */

#include "Common.h" //for micro_time
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

/*void MSensors::update() {

    ADD_PROTO_TIMESTAMP;

    vector<float> bodyAngles = sensors->getBodyAngles();
    for (vector<float>::iterator i = bodyAngles.begin(); i != bodyAngles.end(); i++) {
        this->add_body_angles(*i);
    }

    std::cout << this->DebugString() << std::endl;
}

void MSensors::log() const {
    fileLogger->write();
}*/
}
