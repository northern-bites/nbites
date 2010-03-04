#ifndef _NaoEnactor_h_DEFINED
#define _NaoEnactor_h_DEFINED

#include "dcmproxy.h"
#include "almemoryproxy.h"
#include "almemoryfastaccess.h"
#include "MotionEnactor.h"
#include "albroker.h"
#include "alptr.h"
#include "almemoryfastaccess.h"
#include "Sensors.h"
#include "NaoDef.h"
#include <string>
#include "Transcriber.h"

class NaoEnactor : public MotionEnactor {

public:
	NaoEnactor(boost::shared_ptr<Sensors> s,
               boost::shared_ptr<Transcriber> transcriber,
               AL::ALPtr<AL::ALBroker> broker);
    virtual ~NaoEnactor() { };
    void sendCommands();
    void postSensors();

private: // Constants

    static const int MOTION_FRAME_RATE;
    static const float MOTION_FRAME_LENGTH_uS; // in microseconds
    static const float MOTION_FRAME_LENGTH_S; // in seconds

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



private: // Helper methods
    void sendHardness();
    void sendJoints();
    void sendUltraSound();

    void initDCMAliases();
    void initDCMCommands();
};

#endif
