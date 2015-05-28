#pragma once

// These three are for making Boss an almodule
#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <alproxies/dcmproxy.h>
#include <almemoryfastaccess/almemoryfastaccess.h>
#include <althread/alprocesssignals.h>

#include <boost/shared_ptr.hpp>
#include <vector>

namespace boss {

class Boss : public AL::ALModule
{
public:
    Boss(boost::shared_ptr<AL::ALBroker> broker_, const std::string &name);
    ~Boss();

private:
    void DCMPostProcessCallback();
    void initSensorAccess();

    boost::shared_ptr<AL::ALBroker> broker;

    boost::shared_ptr<AL::DCMProxy> dcm;
    AL::ALProcessSignals::ProcessSignalConnection dcmPreProcessConnection;

    std::vector<float> sensorValues;
    std::vector<std::string> sensorKeys;
    AL::ALMemoryFastAccess* fastAccess;
};

}
