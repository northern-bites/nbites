#ifndef _NaoEnactor_h_DEFINED
#define _NaoEnactor_h_DEFINED

#include "alproxy.h" 
#include "dcmproxy.h"
#include "almemoryproxy.h"
#include "MotionEnactor.h"
#include "albroker.h"
#include "alptr.h"
#include "almemoryfastaccess.h"
#include "Sensors.h"
#include "NaoDef.h"
#include <string>


using namespace std;
 
class NaoEnactor : public MotionEnactor {
		
public:
	NaoEnactor(AL::ALPtr<AL::ALBroker> broker, boost::shared_ptr<Sensors> s);
    virtual ~NaoEnactor() { };
    virtual void run();
    void postSensors();
	
private:
    AL::ALPtr<AL::ALBroker> broker;
    AL::ALPtr<ALMemoryFastAccess> alfastaccess;
    AL::ALPtr<AL::DCMProxy> dcmProxy;
    boost::shared_ptr<Sensors> sensors;
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
    static vector<float> jointValues;
    static vector<float> motionValues;
    static const int MOTION_FRAME_RATE;
    static const float MOTION_FRAME_LENGTH_uS; // in microseconds
    static const float MOTION_FRAME_LENGTH_S; // in seconds

private:
	//void TestSafety(); 
    float SafetyCheck(float,float, int);
    void initSyncWithALMemory();
    void syncWithALMemory();

};
 
#endif
