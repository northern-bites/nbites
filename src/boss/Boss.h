#pragma once

#include "SharedData.h"

// Aldebaran headers
#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <alproxies/dcmproxy.h>
#include <almemoryfastaccess/almemoryfastaccess.h>
#include <althread/alprocesssignals.h>

// Because naoqi gives us the broker as a shared_ptr
#include <boost/shared_ptr.hpp>
#include <vector>

// fork(), pid_t, etc.
#include <unistd.h>
#include <sys/signal.h>

#include "PMotion.pb.h"

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

    std::vector<float> sensorValues;
    std::vector<std::string> sensorKeys;

    messages::JointAngles latestJointAngles;
    messages::JointAngles lattestStiffnesses;

    AL::ALValue jointCommand;
    AL::ALValue stiffnessCommand;
    AL::ALMemoryFastAccess* fastAccess;

    // Vars relating to Man
    pid_t manPID;
    bool manRunning;

};

}
