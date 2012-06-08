#ifndef _NaoEnactor_h_DEFINED
#define _NaoEnactor_h_DEFINED

#include <string>

#include "alcommon/albroker.h"
#include "alcore/alptr.h"
#include "almemoryfastaccess/almemoryfastaccess.h"
#include "alproxies/almemoryproxy.h"
#include "alproxies/dcmproxy.h"
#include "althread/alprocesssignals.h"

#include "Common.h"
#include "MotionEnactor.h"
#include "Sensors.h"
#include "Transcriber.h"


class NaoEnactor : public MotionEnactor {

public:
	NaoEnactor(boost::shared_ptr<Sensors> s,
               boost::shared_ptr<Transcriber> transcriber,
               AL::ALPtr<AL::ALBroker> broker);
    virtual ~NaoEnactor();
    void sendCommands();
    void postSensors();

private: // Members
    AL::ALPtr<AL::ALBroker> broker;
    AL::ALPtr<AL::ALMemoryFastAccess> alfastaccessJoints;
    AL::ALPtr<AL::ALMemoryFastAccess> alfastaccessSensors;
    AL::ALPtr<AL::DCMProxy> dcmProxy;
    boost::shared_ptr<Sensors> sensors;
    boost::shared_ptr<Transcriber> transcriber;
    std::vector<float> motionValues;
    std::vector<float> motionHardness;
    std::vector<float> lastMotionHardness;
    AL::ALValue hardness_command;
    AL::ALValue joint_command;
    AL::ALValue us_command;
    AL::ALProcessSignals::ProcessSignalConnection dcmPreConnection,
        dcmPostConnection;

private: // Helper methods
    void sendHardness();
    void sendJoints();

    void initDCMAliases();
    void initDCMCommands();
};

#endif
