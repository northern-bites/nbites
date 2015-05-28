#pragma once

#include <pthread.h>

#include "PMotion.pb.h"
#include "ButtonState.pb.h"
#include "InertialState.pb.h"
#include "SonarState.pb.h"
#include "FSR.pb.h"
#include "BatteryState.pb.h"
#include "Toggle.pb.h"
#include "StiffnessControl.pb.h"
#include "LedCommand.pb.h"

struct JointCommand {
    messages::JointAngles stiffnessCommand;
    messages::JointAngles JointsCommand;
};

struct SensorValues {
    messages::JointAngles jointsOut;
    messages::JointAngles currentsOut;
    messages::JointAngles temperatureOut;
    messages::ButtonState chestButtonOut;
    messages::FootBumperState footBumperOut;
    messages::InertialState inertialsOut;
    messages::SonarState sonarsOut;
    messages::FSR fsrOut;
    messages::BatteryState batteryOut;
    messages::StiffStatus stiffStatusOut;
};

struct SharedData {
    volatile uint8_t sensorsRead;
    volatile uint8_t sensorsLatest;
    volatile uint8_t commandsRead;
    volatile uint8_t commandsLatest;

    JointCommand commands[3];
    SensorValues sensors[3];
    messages::LedCommand Leds[3];

    pthread_mutex_t lock;
};
