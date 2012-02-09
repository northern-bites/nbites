
#include "Common.h" //for micro_time
#include "MVisionSensors.h"

#include <vector>

namespace man {
namespace memory {

using boost::shared_ptr;
using namespace proto;
using namespace std;

MVisionSensors::MVisionSensors(shared_ptr<Sensors> sensors) :
        MObject(id), sensors(sensors),
        data(new PVisionSensors()) {
    MObject::protoMessage = data;
}

MVisionSensors::~MVisionSensors() {
}

void MVisionSensors::updateData() {

    this->data->set_timestamp(time_stamp());

    this->data->clear_vision_body_angles();
    vector<float> bodyAngles = sensors->getVisionBodyAngles();
    for (vector<float>::iterator i = bodyAngles.begin(); i != bodyAngles.end();
            i++) {
        this->data->add_vision_body_angles(*i);
    }

    FootBumper leftFootBumper = sensors->getLeftFootBumper();
    PSensors::PFootBumper* lfb = this->data->mutable_left_foot_bumper();
    lfb->set_left(leftFootBumper.left);
    lfb->set_right(leftFootBumper.right);
    FootBumper rightFootBumper = sensors->getRightFootBumper();
    PSensors::PFootBumper* rfb = this->data->mutable_right_foot_bumper();
    rfb->set_left(rightFootBumper.left);
    rfb->set_right(rightFootBumper.right);

    this->data->set_ultra_sound_distance_left(sensors->getUltraSoundLeft());
    this->data->set_ultra_sound_distance_right(sensors->getUltraSoundRight());

    this->data->set_battery_charge(sensors->getBatteryCharge());
    this->data->set_battery_current(sensors->getBatteryCurrent());

    //std::cout << this->DebugString() << std::endl;
}

void MVisionSensors::copyTo(shared_ptr<Sensors> sensorsDestination) const {
    vector<float> body_angles(data->vision_body_angles().begin(),
            data->vision_body_angles().end());
    sensorsDestination->setVisionBodyAngles(body_angles);

    sensorsDestination->setLeftFootBumper(data->left_foot_bumper().left(),
            data->left_foot_bumper().right());
    sensorsDestination->setRightFootBumper(data->right_foot_bumper().left(),
            data->right_foot_bumper().right());

    sensorsDestination->setUltraSound(data->ultra_sound_distance_left(),
            data->ultra_sound_distance_right());

    sensorsDestination->setBatteryCharge(data->battery_charge());
    sensorsDestination->setBatteryCurrent(data->battery_current());
}

}

}
