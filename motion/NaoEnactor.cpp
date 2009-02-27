//TODO: look into replacing DCM->getTime with local time tracking

#include "NaoEnactor.h"
#include <iostream>
#define DEBUG_ENACTOR_JOINTS

NaoEnactor::NaoEnactor(AL::ALPtr<AL::ALBroker> _pbroker,
                        boost::shared_ptr<Sensors> s)
    : MotionEnactor(),broker(_pbroker),sensors(s) {
        try {
            dcmProxy = AL::ALPtr<AL::DCMProxy>(new AL::DCMProxy(broker));
        } catch(AL::ALError &e) {
            cout << "Failed to initialize proxy to DCM" << endl;
        }
        try{
            alfastaccess =
                    AL::ALPtr<ALMemoryFastAccess >(new ALMemoryFastAccess());
        } catch(AL::ALError &e){
            cout << "Failed to initialize proxy to ALFastAccess"<<endl;
        }
	try{
	    NaoEnactor::initSyncWithALMemory();
	} catch(AL::ALError &e){
	    cout << "Failed to initialize sync with al memory"<<endl;
	}
    sensors = s;
    //TestSafety();
}
vector<float> NaoEnactor::jointValues(22,0.0); //current values of joints
vector<float> NaoEnactor::motionValues(22,0.0); //commands sent to joints
const string NaoEnactor::PositionPostFix("/Position/Actuator/Value");
const string NaoEnactor::HardnessPostFix("/Hardness/Actuator/Value");
const string NaoEnactor::ValuePostFix("/Position/Sensor/Value");
const string NaoEnactor::ValuePreFix("Device/SubDeviceList/");
const int NaoEnactor::MOTION_FRAME_RATE = 50;
// 1 second * 1000 ms/s * 1000 us/ms
const float NaoEnactor::MOTION_FRAME_LENGTH_uS = 1.0f * 1000.0f * 1000.0f /
                                                NaoEnactor::MOTION_FRAME_RATE;
const float NaoEnactor::MOTION_FRAME_LENGTH_S = 1.0f /
                                                NaoEnactor::MOTION_FRAME_RATE;

const string NaoEnactor::jointsP[NaoEnactor::NUM_JOINTS] = {
	"HeadYaw" + PositionPostFix,
	"HeadPitch" + PositionPostFix,
	"LShoulderPitch" + PositionPostFix,
	"LShoulderRoll" + PositionPostFix,
	"LElbowYaw" + PositionPostFix,
	"LElbowRoll" + PositionPostFix,
	"LHipYawPitch" + PositionPostFix,
	"LHipRoll" + PositionPostFix,
	"LHipPitch" + PositionPostFix,
	"LKneePitch" + PositionPostFix,
	"LAnklePitch" + PositionPostFix,
	"LAnkleRoll" + PositionPostFix,
	"LHipYawPitch" + PositionPostFix, //overrides previous LHipYawPitch, check how this functions in motionprovider
	"RHipRoll" + PositionPostFix,
	"RHipPitch" + PositionPostFix,
	"RKneePitch" + PositionPostFix,
	"RAnklePitch" + PositionPostFix,
    "RAnkleRoll" + PositionPostFix,
	"RShoulderPitch" + PositionPostFix,
	"RShoulderRoll" + PositionPostFix,
	"RElbowYaw" + PositionPostFix,
	"RElbowRoll" + PositionPostFix
};
const string NaoEnactor::jointsH[NaoEnactor::NUM_JOINTS] = {
	"HeadYaw" + HardnessPostFix,
	"HeadPitch" + HardnessPostFix,
	"LShoulderPitch" + HardnessPostFix,
	"LShoulderRoll" + HardnessPostFix,
	"LElbowYaw" + HardnessPostFix,
	"LElbowRoll" + HardnessPostFix,
	"LHipYawPitch" + HardnessPostFix,
	"LHipRoll" + HardnessPostFix,
	"LHipPitch" + HardnessPostFix,
	"LKneePitch" + HardnessPostFix,
	"LAnklePitch" + HardnessPostFix,
	"LAnkleRoll" + HardnessPostFix,
	"LHipYawPitch" + HardnessPostFix,//overrides previous LHipYawPitch, check how this functions in motionprovider
	"RHipRoll" + HardnessPostFix,
	"RHipPitch" + HardnessPostFix,
	"RKneePitch" + HardnessPostFix,
	"RAnklePitch" + HardnessPostFix,
    "RAnkleRoll" + HardnessPostFix,
	"RShoulderPitch" + HardnessPostFix,
	"RShoulderRoll" + HardnessPostFix,
	"RElbowYaw" + HardnessPostFix,
	"RElbowRoll" + HardnessPostFix
};
const string NaoEnactor::jointsV[NaoEnactor::NUM_JOINTS] = {
    ValuePreFix + "HeadYaw" + ValuePostFix,
    ValuePreFix + "HeadPitch" + ValuePostFix,
	ValuePreFix + "LShoulderPitch" + ValuePostFix,
	ValuePreFix + "LShoulderRoll" + ValuePostFix,
	ValuePreFix + "LElbowYaw" + ValuePostFix,
	ValuePreFix + "LElbowRoll" + ValuePostFix,
	ValuePreFix + "LHipYawPitch" + ValuePostFix,
	ValuePreFix + "LHipRoll" + ValuePostFix,
	ValuePreFix + "LHipPitch" + ValuePostFix,
	ValuePreFix + "LKneePitch" + ValuePostFix,
	ValuePreFix + "LAnklePitch" + ValuePostFix,
	ValuePreFix + "LAnkleRoll" + ValuePostFix,
	ValuePreFix + "LHipYawPitch" + ValuePostFix, //this double subscription is intentional
	ValuePreFix + "RHipRoll" + ValuePostFix,
	ValuePreFix + "RHipPitch" + ValuePostFix,
	ValuePreFix + "RKneePitch" + ValuePostFix,
	ValuePreFix + "RAnklePitch" + ValuePostFix,
    ValuePreFix + "RAnkleRoll" + ValuePostFix,
	ValuePreFix + "RShoulderPitch" + ValuePostFix,
	ValuePreFix + "RShoulderRoll" + ValuePostFix,
	ValuePreFix + "RElbowYaw" + ValuePostFix,
	ValuePreFix + "RElbowRoll" + ValuePostFix
};
const float NaoEnactor::jointsMax[NaoEnactor::NUM_JOINTS] = {
    //head
    NaoEnactor::M2R2, NaoEnactor::M2R1,
    //left leg
    NaoEnactor::M1R1, NaoEnactor::M1R2, NaoEnactor::M1R2,
    NaoEnactor::M1R1, NaoEnactor::M1R2, NaoEnactor:: M1R1,
    //right leg
    NaoEnactor::M1R1, NaoEnactor::M1R2, NaoEnactor::M1R2,
    NaoEnactor::M1R1, NaoEnactor::M1R2, NaoEnactor::M1R1,
    //left arm
    NaoEnactor::M2R2, NaoEnactor::M2R1, NaoEnactor::M2R1, NaoEnactor::M2R2,
    //right arm
    NaoEnactor::M2R2, NaoEnactor::M2R1, NaoEnactor::M2R1, NaoEnactor::M2R2
};

void NaoEnactor::run() {
    std::cout << "NaoEnactor::run()" << std::endl;
	//basically postSensors() but need a different order for the first run
    //makes sure we have current joint values in sensors and in motionValues
    syncWithALMemory();
	motionValues = jointValues;
	sensors->setMotionBodyAngles(motionValues);

	//set-up the array for sending commands to DCM
    ALValue commands;
    commands.arraySetSize(3);
    //kind of dcm update 
     //"ClearAll" :  Delete all timed command before adding these one.
    commands[1] = string("ClearAll");
    commands[2].arraySetSize(1); //list of 1 timed-commands
    commands[2][0].arraySetSize(3);
    commands[2][0][2] = 0;
    //sets the hardness for all the joints
    for (int i = 0; i<NaoEnactor::NUM_JOINTS; i++) {
        commands[0] = string(jointsH[i]); //has the actuator name
        //sets the value for hardness
        commands[2][0][0] = 0.85;
        //tells the DCM how much time it has to get to the requested value
        commands[2][0][1] = dcmProxy->getTime(100);
        //sends the command to the DCM
        #ifndef NO_ACTUAL_MOTION
	    try {
	        dcmProxy->set(commands);
	    } catch(AL::ALError& a) {
		    std::cout << "DCM Hardness set error" << a.toString() << "    " 
		        << commands.toString() << std::endl;
	    }
        #endif
    }
    long long currentTime;
    while (running) {
        currentTime = micro_time();
        if(!switchboard){
            cout<< "Caution!! Switchboard is null, exiting NaoEnactor"<<endl;
	        break;
        }

        // Get the angles we want to go to this frame from the switchboard
        motionValues = switchboard->getNextJoints();
        alfastaccess->GetValues(jointValues);
        for (int i = 0; i<NaoEnactor::NUM_JOINTS; i++) {
            #ifdef DEBUG_ENACTOR_JOINTS
	        cout << "result of joint " << i << " is " << motionValues[i] << endl;
	        #endif
            //the name of the joint to send to be sent to the DCM
            commands[0] = string(jointsP[i]);
            
            //Before assigning this value compare to current sensor value and make
	        //sure that it doesn't exceed max speed (SafteyCheck does this)
            float testVal = motionValues[i];
            //may be better to use previous rounds motionValues[i] in case
            //sensor lag occurs. we risk unsafe values if motion is impeded
        	float currentVal = jointValues[i];
	        motionValues[i] =  SafetyCheck(currentVal, testVal, i);
            commands[2][0][0] = motionValues.at(i);
            //tells the dcm to make this move as soon as possible
            commands[2][0][1] = dcmProxy->getTime(0);
	        //consider making this 200, as in RedDocs
            /*"The better way to use timed command is to send them with at least a 
	        20ms delay in the future. With this delay, the D.C.M. can compute an
	        interpolation in its next cycle, whatever the next cycle time is."
	        - From DCM Red Doc*/
            
            //send the array
            #ifndef NO_ACTUAL_MOTION
            try {
	            dcmProxy->set(commands);
	        } catch(AL::ALError& a) {
		        std::cout << "dcm value set error " << a.toString() << std::endl;
	        }
            #endif
        }
        postSensors();
        const long long zero = 0;
        const long long processTime = micro_time() - currentTime;

        #if ! defined OFFLINE || ! defined SPEEDY_ENACTOR
        if (processTime > MOTION_FRAME_LENGTH_uS){
            cout << "Time spent in NaoEnactor longer than frame length: " 
	            << processTime <<endl;
            //Don't sleep at all
        } else{
            usleep(static_cast<useconds_t>(MOTION_FRAME_LENGTH_uS - processTime));
        }
        #endif
    }

}
/*
//runs unit tests when called.
void NaoEnactor::TestSafety(){
    std::cout << "DCM safety check started"<< std::endl;
    for (int i = 0; i < NaoEnactor::NUM_JOINTS; i++){
        if ( SafetyCheck(0.0, 0.2, i) == true){//should return false for all i
            std::cout << "SafetyCheck(0.0, 0.2, " << i << ") failed" << std::endl;
        } 
        if (SafetyCheck(0.0, -0.2, i) == true){//should return false for all i
            std::cout << "SafetyCheck(0.0, -0.2, " << i << ") failed" << std::endl;
        }
        if (SafetyCheck(0.0, 0.01 ,i) == false){//should return true for all i
            std::cout << "SafetyCheck(0.0, 0.01, " << i << ") failed" << std::endl;
        } 
        if (SafetyCheck(0.0, -0.01 ,i) == false){//should return true for all i
            std::cout << "SafetyCheck(0.0, -0.01, " << i << ") failed" << std::endl;
        }
    }
    std::cout << "DCM safety check finished"<< std::endl;
}
*/
//makes sure that we don't tell the motors to move faster than they can
//the DCM takes care of trimming too large/ too small of values
float NaoEnactor::SafetyCheck(float currentVal, float toCheck, int i){
    
    float absDiffInRad = fabs(currentVal - toCheck);
    float allowedDiffInRad = jointsMax[i];
    if (absDiffInRad > allowedDiffInRad){
       
      if (toCheck > currentVal){ 
	std::cout << jointsP[i] << "Current = " << currentVal << "  TRIM = " << (currentVal + allowedDiffInRad) << std::endl;
	return (currentVal + allowedDiffInRad); 
      }
      else { 
	std::cout << jointsP[i] << "Current = " << currentVal << "  TRIM = " << (currentVal - allowedDiffInRad) << std::endl;
	return (currentVal - allowedDiffInRad); 
      }
    }
    std::cout << jointsP[i] << "Current = " << currentVal << "  Sent = " << toCheck << std::endl;
    return toCheck;
}

void NaoEnactor::postSensors(){
    //At the beginning of each cycle, we need to update the sensor values
    //We also call this from the Motion run method
    //This is important to ensure that the providers have access to the
    //actual joint post of the robot before any computation begins
    sensors->setMotionBodyAngles(motionValues);
    syncWithALMemory();

}
/**
 * ALFastAccess allows us to pull out values from ALMemory a lot faster
 * and in bulk. The order in which we declare the desired devices are also
 * the order in which we receive them (see syncWithALMemory).
 * In this class we only sync the sensors values we need for motion. The
 * rest are synced in Man.cpp (may change).
 */
void NaoEnactor::initSyncWithALMemory(){

    vector<string> jointNames;
    jointNames +=
      string(jointsV[0]), string(jointsV[1]), string(jointsV[2]), string(jointsV[3]), string(jointsV[4]),
      string(jointsV[5]), string(jointsV[6]), string(jointsV[7]), string(jointsV[8]), string(jointsV[9]),
      string(jointsV[10]), string(jointsV[11]), string(jointsV[12]), string(jointsV[13]),
      string(jointsV[14]), string(jointsV[15]), string(jointsV[16]), string(jointsV[17]),
      string(jointsV[18]), string(jointsV[19]), string(jointsV[20]), string(jointsV[21]);
    /*jointNames +=
      string("Device/SubDeviceList/HeadYaw/Position/Sensor/Value"),
      string("Device/SubDeviceList/HeadPitch/Position/Sensor/Value"),
      string("Device/SubDeviceList/LShoulderPitch/Position/Sensor/Value"),
      string("Device/SubDeviceList/LShoulderRoll/Position/Sensor/Value"),
      string("Device/SubDeviceList/LElbowYaw/Position/Sensor/Value"),
      string("Device/SubDeviceList/LElbowRoll/Position/Sensor/Value"),
      string("Device/SubDeviceList/LHipYawPitch/Position/Sensor/Value"),
      string("Device/SubDeviceList/LHipRoll/Position/Sensor/Value"),
      string("Device/SubDeviceList/LHipPitch/Position/Sensor/Value"),
      string("Device/SubDeviceList/LKneePitch/Position/Sensor/Value"),
      string("Device/SubDeviceList/LAnklePitch/Position/Sensor/Value"),
      string("Device/SubDeviceList/LAnkleRoll/Position/Sensor/Value"),
      string("Device/SubDeviceList/LHipYawPitch/Position/Sensor/Value"),
      string("Device/SubDeviceList/RHipRoll/Position/Sensor/Value"),
      string("Device/SubDeviceList/RHipPitch/Position/Sensor/Value"),
      string("Device/SubDeviceList/RKneePitch/Position/Sensor/Value"),
      string("Device/SubDeviceList/RAnklePitch/Position/Sensor/Value"),
      string("Device/SubDeviceList/RAnkleRoll/Position/Sensor/Value"),
      string("Device/SubDeviceList/RShoulderPitch/Position/Sensor/Value"),
      string("Device/SubDeviceList/RShoulderRoll/Position/Sensor/Value"),
      string("Device/SubDeviceList/RElbowYaw/Position/Sensor/Value"),
      string("Device/SubDeviceList/RElbowRoll/Position/Sensor/Value");*/

    try{
        alfastaccess->ConnectToVariables(broker,jointNames);
    } catch(AL::ALError& a) {
      std::cout << "NaoEnactor " << a.toString() << std::endl;}
  
}

void NaoEnactor::syncWithALMemory() {
    alfastaccess->GetValues(jointValues);
    sensors->setBodyAngles(jointValues);
     

    
}
