#include "ALTranscriber.h"

using namespace std;

#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

#include "NBMath.h"
using namespace NBMath;

#include "Kinematics.h"
using namespace Kinematics;


#include "BasicWorldConstants.h"
#include "ALNames.h"
using namespace ALNames;

ALTranscriber::ALTranscriber (AL::ALPtr<AL::ALBroker> _broker,
                              boost::shared_ptr<Sensors> _sensors)
    :Transcriber(_sensors),
     broker(_broker),
     accelerationFilter(),
     lastAngleX(0.0f), lastAngleY(0.0f)
{
	try {
		almemory = AL::ALPtr<AL::ALMemoryFastAccess >(
				new AL::ALMemoryFastAccess());
	} catch(AL::ALError &e){
		cout << "Failed to initialize ALMemory for joints"<<endl;
	}

    try{
        initSyncMotionWithALMemory();
    } catch(AL::ALError &e){
        cout << "Failed to initialize motion sync with al memory"
             << e.toString() <<endl;
    }

    try{
        initSyncVisionWithALMemory();
    } catch(AL::ALError &e){
        cout << "Failed to initialize vision sync with al memory:"
             << e.toString() <<endl;
    }

    //Ensure that the current sensor values are copied correctly sensors
    initSensorBodyJoints();
}


void ALTranscriber::initSensorBodyJoints(){
    //We need to correctly initialize the value in Sensors so that
    //motion can run based on the actual pose of the robot.
    //This is basically postSensors() but need a different order
    syncMotionWithALMemory();

    //HACK: this should be moved to MotionSwitchboard
    //sensors->setMotionBodyAngles(jointValues);
}

/**
 * ALFastAccess allows us to pull out values from ALMemory a lot faster
 * and in bulk. The order in which we declare the desired devices are also
 * the order in which we receive them (see syncWithALMemory).
 * In this class we only sync the sensors values we need for motion. The
 * rest are synced in Man.cpp (may change).
 */
void ALTranscriber::initSyncMotionWithALMemory(){

    for (unsigned int i = 0; i < Kinematics::NUM_JOINTS; i++) {
    	jointValuePointers[i] =
    			reinterpret_cast<float*>(almemory->getDataPtr(broker,
    					ALNames::jointsV[i], false));
    }

    // Now connect to all the sensor values we need to update on the motion
    // frame rate: FSR and inertial sensors for now.
    vector<string> sensorNames;
    sensorNames +=
    		string("Device/SubDeviceList/LFoot/FSR/FrontLeft/Sensor/Value"),
    		string("Device/SubDeviceList/LFoot/FSR/FrontRight/Sensor/Value"),
    		string("Device/SubDeviceList/LFoot/FSR/RearLeft/Sensor/Value"),
    		string("Device/SubDeviceList/LFoot/FSR/RearRight/Sensor/Value"),
    		string("Device/SubDeviceList/RFoot/FSR/FrontLeft/Sensor/Value"),
    		string("Device/SubDeviceList/RFoot/FSR/FrontRight/Sensor/Value"),
    		string("Device/SubDeviceList/RFoot/FSR/RearLeft/Sensor/Value"),
    		string("Device/SubDeviceList/RFoot/FSR/RearRight/Sensor/Value"),
    		string("Device/SubDeviceList/ChestBoard/Button/Sensor/Value"),
    		string("Device/SubDeviceList/InertialSensor/AccX/Sensor/Value"),
    		string("Device/SubDeviceList/InertialSensor/AccY/Sensor/Value"),
    		string("Device/SubDeviceList/InertialSensor/AccZ/Sensor/Value"),
    		string("Device/SubDeviceList/InertialSensor/GyrX/Sensor/Value"),
    		string("Device/SubDeviceList/InertialSensor/GyrY/Sensor/Value"),
    		string("Device/SubDeviceList/InertialSensor/AngleX/Sensor/Value"),
    		string("Device/SubDeviceList/InertialSensor/AngleY/Sensor/Value");

    for (unsigned int i = 0; i < 16; i++ ) {
    	motionSensorPointers[i] =
    			reinterpret_cast<float*> (almemory->getDataPtr(broker,
    					sensorNames[i], false));
    }

    for (unsigned int i = 0; i < Kinematics::NUM_JOINTS; i++) {
    	jointTempPointers[i] =
    			reinterpret_cast<float*> (almemory->getDataPtr(broker,
    					ALNames::jointsT[i], false));
    }
}

/**
 * Sensor unit outputs are 8-bit binary, we normalize them here to
 * gravity (-9.8m/s^2) so that they're in units (m/s^2) that we can
 * use in motion etc.  This calibration was done by hand and should
 * probably be checked every so often.
 * -Nathan, June 2011
 */
static const float ACCEL_OFFSET_X = 0.0f;
static const float ACCEL_OFFSET_Y = 0.0f;
static const float ACCEL_OFFSET_Z = 0.0f;

// converts from sensor units to m/ss
static const float ACCEL_CONVERSION_X = -GRAVITY_mss * 0.0153f;
static const float ACCEL_CONVERSION_Y = -GRAVITY_mss * 0.016f;
static const float ACCEL_CONVERSION_Z = ACCEL_CONVERSION_X;

// TODO: convert the gyro here similarly. For more information, see:
// http://users.aldebaran-robotics.com/docs/save_doc_1.10.37/site_en/reddoc/dcm/mandatory_specific_almemory_keys.html

/* The calls to the BoardError class filter out large jumps (usually board errors)
   for accX/Y/Z before other filtering */

const float ALTranscriber::calibrate_acc_x(const float x) {
    float moved = (x + ACCEL_OFFSET_X) * ACCEL_CONVERSION_X;
    //cout << "accX raw: " << x << " calibrated: " << moved << endl;
    return moved;
}

const float ALTranscriber::calibrate_acc_y(const float y) {
    const float moved = (y + ACCEL_OFFSET_Y) * ACCEL_CONVERSION_Y;
    //cout << "accY raw: " << y << " calibrated: " << moved << endl;
    return moved;
}

const float ALTranscriber::calibrate_acc_z(const float z) {
    const float moved = (z + ACCEL_OFFSET_Z) * ACCEL_CONVERSION_Z;
    //cout << "accZ raw: " << z << " calibrated " << moved << endl;
    return moved;
}

void ALTranscriber::syncMotionWithALMemory() {
    sensors->setBodyAngles(jointValuePointers);
    sensors->setBodyTemperatures(jointTempPointers);

    const float LfrontLeft = *motionSensorPointers[0], LfrontRight = *motionSensorPointers[1],
        LrearLeft = *motionSensorPointers[2], LrearRight = *motionSensorPointers[3],
        RfrontLeft = *motionSensorPointers[4], RfrontRight = *motionSensorPointers[5],
        RrearLeft = *motionSensorPointers[6], RrearRight = *motionSensorPointers[7];

    const float chestButton = *motionSensorPointers[8];

    const float accX = calibrate_acc_x(*motionSensorPointers[9]),
        accY = calibrate_acc_y(*motionSensorPointers[10]),
        accZ = calibrate_acc_z(*motionSensorPointers[11]),
        gyrX = *motionSensorPointers[12], gyrY = *motionSensorPointers[13],
        angleX = *motionSensorPointers[14],//,-M_PI_FLOAT,M_PI_FLOAT),
        angleY = *motionSensorPointers[15];//,-M_PI_FLOAT,M_PI_FLOAT);

    accelerationFilter.update(accX, accY, accZ);

    const float filteredX = accelerationFilter.getX();
    const float filteredY = accelerationFilter.getY();
    const float filteredZ = accelerationFilter.getZ();

    //Filter angleX for large jumps, which are board errors
    float filteredAngleX = lastAngleX;
    float filteredAngleY = lastAngleY;
    if(std::abs(lastReadAngleX -angleX) < 0.10 &&
       std::abs(lastReadAngleY -angleY) < 0.10){ // experimental
        const float newWeight = 0.75f;
        filteredAngleX = newWeight*angleX + (1-newWeight)*lastAngleX ;
        filteredAngleY = newWeight*angleY + (1-newWeight)*lastAngleY ;
    }else{
        //cout<< "Got a bad angleXY value!"<<endl;
        //Do nothing, since the values are bad
    }

    lastAngleX = filteredAngleX;
    lastAngleY = filteredAngleY;
    lastReadAngleX = angleX;
    lastReadAngleY = angleY;

    // so the FSRs etc. aren't allocated each time
    static FSR leftFSR, rightFSR;

    leftFSR.frontLeft = LfrontLeft;
    leftFSR.frontRight = LfrontRight;
    leftFSR.rearLeft = LrearLeft;
    leftFSR.rearRight = LrearRight;

    rightFSR.frontLeft = RfrontLeft;
    rightFSR.frontRight = RfrontRight;
    rightFSR.rearLeft = RrearLeft;
    rightFSR.rearRight = RrearRight;

    static Inertial filtered, unfiltered;

    filtered.accX = filteredX;
    filtered.accY = filteredY;
    filtered.accZ = filteredZ;
    filtered.gyrX = gyrX;
    filtered.gyrY = gyrY;
    filtered.angleX = filteredAngleX;
    filtered.angleY = filteredAngleY;

    unfiltered.accX = accX;
    unfiltered.accY = accY;
    unfiltered.accZ = accZ;
    unfiltered.gyrX = gyrX;
    unfiltered.gyrY = gyrY;
    unfiltered.angleX = angleX;
    unfiltered.angleY = angleY;

    sensors->
        setMotionSensors(leftFSR, rightFSR,
                         chestButton,
			 filtered,
			 unfiltered);
}


void ALTranscriber::initSyncVisionWithALMemory() {

        vector<string> varNames;
        varNames +=
            string("Device/SubDeviceList/LFoot/Bumper/Left/Sensor/Value"),
            string("Device/SubDeviceList/LFoot/Bumper/Right/Sensor/Value"),
            string("Device/SubDeviceList/RFoot/Bumper/Left/Sensor/Value"),
            string("Device/SubDeviceList/RFoot/Bumper/Right/Sensor/Value"),
            string("Device/SubDeviceList/US/Left/Sensor/Value"),
            string("Device/SubDeviceList/US/Right/Sensor/Value"),
            string("Device/SubDeviceList/Battery/Charge/Sensor/Value"),
            string("Device/SubDeviceList/Battery/Current/Sensor/Value");

        for (unsigned int i = 0; i < 8; i++ ) {
        	visionSensorPointers[i] =
        			reinterpret_cast<float*> (almemory->getDataPtr(broker,
        					varNames[i], false));
        }
}

void ALTranscriber::syncVisionWithALMemory() {

    const float leftFootBumperLeft  = *visionSensorPointers[0];
    const float leftFootBumperRight = *visionSensorPointers[1];
    const float rightFootBumperLeft = *visionSensorPointers[2];
    const float rightFootBumperRight = *visionSensorPointers[3];
    const float ultraSoundDistLeft = *visionSensorPointers[4];
    const float ultraSoundDistRight = *visionSensorPointers[5];
    const float batteryCharge = *visionSensorPointers[6];
    const float batteryCurrent = *visionSensorPointers[7];

    sensors->
        setVisionSensors(FootBumper(leftFootBumperLeft, leftFootBumperRight),
                         FootBumper(rightFootBumperLeft, rightFootBumperRight),
                         ultraSoundDistLeft,
                         ultraSoundDistRight,
                         batteryCharge,
                         batteryCurrent);
}
