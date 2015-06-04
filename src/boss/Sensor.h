#pragma once

#include "SharedData.h"
#include <boost/shared_ptr.hpp>

#include <alcommon/albroker.h>
#include <almemoryfastaccess/almemoryfastaccess.h>
#include <alproxies/dcmproxy.h>

#include "PMotion.pb.h"
#include "ButtonState.pb.h"
#include "InertialState.pb.h"
#include "SonarState.pb.h"
#include "FSR.pb.h"
#include "BatteryState.pb.h"
#include "Toggle.pb.h"
#include "StiffnessControl.pb.h"


namespace boss {
namespace sensor {

class Sensor
{
public:
    Sensor(boost::shared_ptr<AL::ALBroker> broker_);
    ~Sensor();

    SensorValues getSensors();

private:
    void initSensor();
    void initSonarValues();

    messages::JointAngles buildJointsMessage();
    messages::JointAngles buildCurrentsMessage();
    messages::JointAngles buildTemperatureMessage();
    messages::ButtonState buildChestboardButtonMessage();
    messages::FootBumperState buildFootbumperMessage();
    messages::InertialState buildInertialsMessage();
    messages::SonarState buildSonarsMessage();
    messages::FSR buildFSRMessage();
    messages::BatteryState buildBatteryMessage();
    messages::StiffStatus buildStiffMessage();

    boost::shared_ptr<AL::ALBroker> broker;
    AL::ALMemoryFastAccess* fastAccess;

    std::vector<float> sensorValues;
    std::vector<std::string> sensorKeys;

};
}
}
