#pragma once

#include "SharedData.h"

// Aldebaran headers
#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <alcommon/alproxy.h>
#include <alproxies/dcmproxy.h>
#include <althread/alprocesssignals.h>

// Because naoqi gives us the broker as a shared_ptr
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <vector>

// fork(), pid_t, etc.
#include <unistd.h> // fork(), pid_t, etc
#include <sys/signal.h> // SIG_TERM
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <semaphore.h>

#include "PMotion.pb.h"
#include "Sensor.h"
#include "Enactor.h"
#include "led/LedEnactor.h"

namespace boss {

class Boss : public AL::ALModule
{
public:
    Boss(boost::shared_ptr<AL::ALBroker> broker_, const std::string &name);
    ~Boss();

private:
    void DCMPreProcessCallback();
    void DCMPostProcessCallback();

    int constructSharedMem();

    int startMan();
    int killMan();

    // To handle naoqi stuff
    boost::shared_ptr<AL::ALBroker> broker;
    boost::shared_ptr<AL::DCMProxy> dcm;

    AL::ALProcessSignals::ProcessSignalConnection dcmPreProcessConnection;
    AL::ALProcessSignals::ProcessSignalConnection dcmPostProcessConnection;

    sensor::Sensor sensor;
    enactor::Enactor enactor;
    led::LedEnactor led;

    // Vars relating to Man
    pid_t manPID;
    bool manRunning;

    int shared_fd;
    SharedData* shared;

    uint64_t commandIndex;
    uint64_t sensorIndex;

    sem_t* semaphore;
};

}