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

#define MEMORY_NAME "/nbites-memory"
#define SEMAPHORE_NAME "/nbites-semaphore"

struct JointCommand {
    messages::JointAngles stiffnessCommand;
    messages::JointAngles JointsCommand;
};

struct SensorValues {
    messages::JointAngles joints;
    messages::JointAngles currents;
    messages::JointAngles temperature;
    messages::ButtonState chestButton;
    messages::FootBumperState footBumper;
    messages::InertialState inertials;
    messages::SonarState sonars;
    messages::FSR fsr;
    messages::BatteryState battery;
    messages::StiffStatus stiffStatus;
};

struct SharedData {
    volatile uint8_t sensorsRead;
    volatile uint8_t sensorsLatest;
    volatile uint8_t commandsRead;
    volatile uint8_t commandsLatest;

    JointCommand commands[2];
    messages::LedCommand Leds[2];

    SensorValues sensors[2];
};
