/*
 * MVisionSensors.cpp
 *
 *      Author: oneamtu
 */

#include "Common.h" //for micro_time
#include "MemoryMacros.h"
#include "MVisionSensors.h"

#include <vector>

namespace man {
namespace memory {

using boost::shared_ptr;
using namespace proto;
using namespace std;

MVisionSensors::MVisionSensors(shared_ptr<Sensors> s) : sensors(s) { }

MVisionSensors::~MVisionSensors() {
}

void MVisionSensors::update() {

    ADD_PROTO_TIMESTAMP;

    this->clear_vision_body_angles();
    vector<float> bodyAngles = sensors->getVisionBodyAngles();
    for (vector<float>::iterator i = bodyAngles.begin(); i != bodyAngles.end(); i++) {
        this->add_vision_body_angles(*i);
    }

    FootBumper leftFootBumper = sensors->getLeftFootBumper();
    PSensors::PFootBumper* lfb = this->mutable_left_foot_bumper();
    lfb->set_left(leftFootBumper.left);
    lfb->set_right(leftFootBumper.right);
    FootBumper rightFootBumper = sensors->getRightFootBumper();
    PSensors::PFootBumper* rfb = this->mutable_right_foot_bumper();
    rfb->set_left(rightFootBumper.left);
    rfb->set_right(rightFootBumper.right);

    this->set_ultra_sound_distance_left(sensors->getUltraSoundLeft());
    this->set_ultra_sound_distance_right(sensors->getUltraSoundRight());

    this->set_battery_charge(sensors->getBatteryCharge());
    this->set_battery_current(sensors->getBatteryCurrent());

    //std::cout << this->DebugString() << std::endl;
}
}
}
