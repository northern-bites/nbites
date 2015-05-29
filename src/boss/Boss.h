#pragma once

#include "SharedData.h"

// Aldebaran headers
#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <alproxies/dcmproxy.h>
#include <althread/alprocesssignals.h>

// Because naoqi gives us the broker as a shared_ptr
#include <boost/shared_ptr.hpp>
#include <vector>

// fork(), pid_t, etc.
#include <unistd.h> // fork(), pid_t, etc
#include <sys/signal.h> // SIG_TERM
//#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "PMotion.pb.h"
#include "Sensor.h"
#include "Enactor.h"

namespace boss {

class Boss : public AL::ALModule
{
public:
    Boss(boost::shared_ptr<AL::ALBroker> broker_, const std::string &name);
    ~Boss();

private:
    void DCMPreProcessCallback();
    void DCMPostProcessCallback();
    void initEnactor();
    void initSensorAccess();
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

    // Vars relating to Man
    pid_t manPID;
    bool manRunning;

    SharedData* sharedMem;
};

}
