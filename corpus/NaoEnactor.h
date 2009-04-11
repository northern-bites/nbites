#ifndef _NaoEnactor_h_DEFINED
#define _NaoEnactor_h_DEFINED

#include "dcmproxy.h"
#include "almemoryproxy.h"
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
	NaoEnactor(AL::ALPtr<AL::ALBroker> broker,
               boost::shared_ptr<Sensors> s,
               boost::shared_ptr<Transcriber> transcriber);
    virtual ~NaoEnactor() { };
    void sendCommands();
    void postSensors();

private: // Constants

    static const int MOTION_FRAME_RATE;
    static const float MOTION_FRAME_LENGTH_uS; // in microseconds
    static const float MOTION_FRAME_LENGTH_S; // in seconds

private: // Members
    AL::ALPtr<AL::ALBroker> broker;
    AL::ALPtr<ALMemoryFastAccess> alfastaccessJoints;
    AL::ALPtr<ALMemoryFastAccess> alfastaccessSensors;
    AL::ALPtr<AL::DCMProxy> dcmProxy;
    boost::shared_ptr<Sensors> sensors;
    boost::shared_ptr<Transcriber> transcriber;
    std::vector<float> jointValues;
    std::vector<float> motionValues;
    std::vector<float> lastMotionCommandAngles;
    std::vector<float> motionHardness;
    AL::ALValue hardness_command;
    AL::ALValue joint_command;
    AL::ALValue us_command;



private: // Helper methods
    void sendHardness();
    void sendJoints();
    void sendUltraSound();

    float SafetyCheck(float,float,float, int);
    void initDCMAliases();
    void initDCMCommands();
};

#endif
