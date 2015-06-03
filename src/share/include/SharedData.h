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

#include "serializer.h"

#define NBITES_MEM "/nbites-memory"

const int COMMAND_SIZE = 2048;
const int SENSOR_SIZE = 1024;

struct JointCommand {
    uint64_t writeIndex;
    messages::JointAngles jointsCommand;
    messages::JointAngles stiffnessCommand;
};

struct SensorValues {
    uint64_t writeIndex;
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
    volatile int commandSwitch;
    volatile int sensorSwitch;

    volatile uint64_t commandReadIndex;
    volatile uint64_t sensorReadIndex;

    uint8_t command[2][COMMAND_SIZE];
    uint8_t sensors[2][SENSOR_SIZE];

    // seperate mutexes for commands and sensors
    //pthread_mutex_t command;
    //pthread_mutex_t sense;
};
