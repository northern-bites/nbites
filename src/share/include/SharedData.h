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

#define NBITES_MEM "/nbites-memory"
#define NBITES_SEM "/nbites-semaphore"

struct JointCommand {
    messages::JointAngles jointsCommand;
    messages::JointAngles stiffnessCommand;
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
    volatile int sensorsRead;
    volatile int sensorsLatest;
    volatile int commandsRead;
    volatile int commandsLatest;

    volatile JointCommand commands[2];
    volatile SensorValues sensors[2];
    volatile messages::LedCommand leds[2];
};
