/*
 * MSensors.cpp
 *
 *      Author: oneamtu
 */

#include <vector>

#include "Common.h" //for micro_time
#include "MemoryMacros.h"
#include "MMotionSensors.h"

namespace man {
namespace memory {

using boost::shared_ptr;
using namespace proto;
using namespace std;

MMotionSensors::MMotionSensors(MObject_ID id, shared_ptr<Sensors> s,
                               shared_ptr<PMotionSensors> motion_s_data)
    : MObject(id, motion_s_data), sensors(s), data(motion_s_data){
}

MMotionSensors::~MMotionSensors() {
}

void MMotionSensors::update() {

    ADD_PROTO_TIMESTAMP;

    this->data->clear_body_angles();
    vector<float> bodyAngles = sensors->getBodyAngles();
    for (vector<float>::iterator i = bodyAngles.begin(); i != bodyAngles.end(); i++) {
        this->data->add_body_angles(*i);
    }

    this->data->clear_body_temperatures();
    vector<float> bodyTemperatures = sensors->getBodyTemperatures();
    for (vector<float>::iterator i = bodyTemperatures.begin(); i != bodyTemperatures.end(); i++) {
        this->data->add_body_temperatures(*i);
    }

    const Inertial _inertial = sensors->getInertial();
    PSensors::PInertial* inertial = this->data->mutable_inertial();
    inertial->set_acc_x(_inertial.accX);
    inertial->set_acc_y(_inertial.accY);
    inertial->set_acc_z(_inertial.accZ);
    inertial->set_gyr_x(_inertial.accX);
    inertial->set_gyr_y(_inertial.accY);
    inertial->set_angle_x(_inertial.angleX);
    inertial->set_angle_y(_inertial.angleY);

    const FSR _lfsr = sensors->getLeftFootFSR();
    PSensors::PFSR* lfsr = this->data->mutable_left_foot_fsr();
    lfsr->set_front_left(_lfsr.frontLeft);
    lfsr->set_front_right(_lfsr.frontRight);
    lfsr->set_rear_left(_lfsr.rearLeft);
    lfsr->set_rear_right(_lfsr.rearRight);
    const FSR _rfsr = sensors->getRightFootFSR();
    PSensors::PFSR* rfsr = this->data->mutable_right_foot_fsr();
    rfsr->set_front_left(_rfsr.frontLeft);
    rfsr->set_front_right(_rfsr.frontRight);
    rfsr->set_rear_left(_rfsr.rearLeft);
    rfsr->set_rear_right(_rfsr.rearRight);

    this->data->set_support_foot(sensors->getSupportFoot());
}

}
}
