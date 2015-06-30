/**
 * @brief  Module that reads Sensor data (such as joint angles, inertials etc)
 *         from shared memory, provided by Boss process
 * @author Daniel Zeller (Tore apart Ellis Ratners original Module)
 * @date   June 2015
 */
#pragma once

#include <vector>
#include <string>
#include <iostream>

#include "RoboGrams.h"
#include "SensorTypes.h"
#include "SharedData.h"

#include "PMotion.pb.h"
#include "ButtonState.pb.h"
#include "InertialState.pb.h"
#include "SonarState.pb.h"
#include "FSR.pb.h"
#include "BatteryState.pb.h"
#include "Toggle.pb.h"
#include "StiffnessControl.pb.h"

namespace man {
namespace sensors {

/**
 * @class SensorsModule
 */
class SensorsModule : public portals::Module
{
public:
    SensorsModule();
    virtual ~SensorsModule();

    std::string makeSweetMoveTuple(const messages::JointAngles* angles);
    /*
     * These portals enable other modules to get sensory
     * information.
     */
    portals::InPortal<messages::Toggle>           printInput;

    portals::OutPortal<messages::JointAngles>     jointsOutput_;
    portals::OutPortal<messages::JointAngles>     currentsOutput_;
    portals::OutPortal<messages::JointAngles>     temperatureOutput_;
    portals::OutPortal<messages::ButtonState>     chestboardButtonOutput_;
    portals::OutPortal<messages::FootBumperState> footbumperOutput_;
    portals::OutPortal<messages::InertialState>   inertialsOutput_;
    portals::OutPortal<messages::SonarState>      sonarsOutput_;
    portals::OutPortal<messages::FSR>             fsrOutput_;
    portals::OutPortal<messages::BatteryState>    batteryOutput_;
    portals::OutPortal<messages::StiffStatus>     stiffStatusOutput_;
    portals::OutPortal<messages::Toggle>          sitDownOutput_;

private:
    void run_();
    void updateSensorValues();

    SensorValues values;

    int shared_fd;
    volatile SharedData* shared;
    // Temporary location for the sensor data
    uint8_t sensorsStage[SENSOR_SIZE];

    bool lastPrint;
    bool sitDown;
};

} // namespace sensors
} // namespace man
