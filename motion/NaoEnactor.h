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

class NaoEnactor : public MotionEnactor {

public:
	NaoEnactor(AL::ALPtr<AL::ALBroker> broker, boost::shared_ptr<Sensors> s);
    virtual ~NaoEnactor() { };
    void sendJoints();
    void postSensors();

private: // Constants
    static const int NUM_JOINTS = NUM_ACTUATORS;
    static const string PositionPostFix;
    static const string HardnessPostFix;
    static const string ValuePostFix;
    static const string ValuePreFix;
    static const string jointsP[NUM_JOINTS];
    static const string jointsH[NUM_JOINTS];
    static const string jointsV[NUM_JOINTS];
    static const float jointsMax[NUM_JOINTS];
    //Set hardware values- nominal speed in rad/20ms
    //from http://robocup.aldebaran-robotics.com/docs/reddoc/hardware.php
    //M=motor r = reduction ratio
    static const float M1R1 = 0.1012;
    static const float M1R2 = 0.0658;
    static const float M2R1 = 0.1227;
    static const float M2R2 = 0.1066;
    static const int MOTION_FRAME_RATE;
    static const float MOTION_FRAME_LENGTH_uS; // in microseconds
    static const float MOTION_FRAME_LENGTH_S; // in seconds

private: // Members
    AL::ALPtr<AL::ALBroker> broker;
    AL::ALPtr<ALMemoryFastAccess> alfastaccessJoints;
    AL::ALPtr<ALMemoryFastAccess> alfastaccessSensors;
    AL::ALPtr<AL::DCMProxy> dcmProxy;
    boost::shared_ptr<Sensors> sensors;
    std::vector<float> jointValues;
    std::vector<float> motionValues;
    AL::ALValue hardness_command;
    AL::ALValue joint_command;

private: // Helper methods
    void setBodyHardness(float bodyHardness);
    float SafetyCheck(float,float, int);
    void initSyncWithALMemory();
    void initDCMAliases();
    void initSensorBodyJoints();
    void initDCMCommands();
    void syncWithALMemory();

    static const float calibrate_acc_x(const float x);
    static const float calibrate_acc_y(const float y);
    static const float calibrate_acc_z(const float z);
};

#endif
