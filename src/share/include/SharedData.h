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

static const char * NBITES_MEM = "/nbites-memory";
static const int COMMAND_SIZE = (1 << 11);
static const int SENSOR_SIZE = (1 << 10);

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
    uint8_t sensors[2][SENSOR_SIZE];
    uint8_t command[COMMAND_SIZE];

    pthread_mutex_t sensor_mutex[2];
    pthread_mutex_t cmnd_mutex;

    uint8_t sensorSwitch;
    uint64_t latestSensorWritten;
    uint64_t latestSensorRead;

    uint64_t latestCommandWritten;
    uint64_t latestCommandRead;

    uint8_t sit;
};
