#pragma once

#include <pthread.h>

#if !defined(_POSIX_THREAD_PROCESS_SHARED) && !defined(PTHREAD_PROCESS_SHARED)
#error "Boss must be able to share mutexes between processes"
#endif

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
    int64_t writeIndex;
    messages::JointAngles jointsCommand;
    messages::JointAngles stiffnessCommand;
};

struct SensorValues {
    int64_t writeIndex;
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
    uint8_t sensors[SENSOR_SIZE];
    uint8_t command[COMMAND_SIZE];

    pthread_mutex_t sensor_mutex;
    pthread_mutex_t cmnd_mutex;

    //indices signed so that if (read > written), for whatever horrible reason,
    //simple (last_written - last_read) difference checks will still work.
    
    /* NOTE: it is assumed these values will never overflow (would require 9e15 frames,
     or 25019997929 hours) */
    
    int64_t latestSensorWritten;
    int64_t latestSensorRead;

    int64_t latestCommandWritten;
    int64_t latestCommandRead;
    
    uint8_t sit;
};


