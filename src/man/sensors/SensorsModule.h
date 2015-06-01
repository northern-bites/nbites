/**
 * @brief  Module that serves as an interface between the low-level
 *         NAOQi sensor data acquisition, and responsible for retrieving
 *         and maintaining information about the state of the sonar,
 *         joints, inertia, FSRs, and buttons.
 * @author Ellis Ratner <eratner@bowdoin.edu>
 * @date   January 2013
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

private:
    void run_();
    void updateSensorValues();

    SensorValues values;

    int shared_fd;
    SharedData* shared;

    uint64_t sensorIndex;

    bool lastPrint;
};

} // namespace sensors
} // namespace man
