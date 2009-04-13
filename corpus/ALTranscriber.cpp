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
     accelerationFilter()
{
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
    sensors->setMotionBodyAngles(jointValues);
}

/**
 * ALFastAccess allows us to pull out values from ALMemory a lot faster
 * and in bulk. The order in which we declare the desired devices are also
 * the order in which we receive them (see syncWithALMemory).
 * In this class we only sync the sensors values we need for motion. The
 * rest are synced in Man.cpp (may change).
 */
void ALTranscriber::initSyncMotionWithALMemory(){
    try{
        alfastaccessJoints =
            AL::ALPtr<ALMemoryFastAccess >(new ALMemoryFastAccess());
    } catch(AL::ALError &e){
        cout << "Failed to initialize proxy to ALFastAccess for joints"<<endl;
    }

    try{
        alfastaccessSensors =
            AL::ALPtr<ALMemoryFastAccess >(new ALMemoryFastAccess());
    } catch(AL::ALError &e){
        cout << "Failed to initialize proxy to ALFastAccess for sensors"<<endl;
    }

    try{
        alfastaccessTemps =
            AL::ALPtr<ALMemoryFastAccess >(new ALMemoryFastAccess());
    } catch(AL::ALError &e){
        cout << "Failed to initialize proxy to ALFastAccess for temps"<<endl;
    }

    vector<string> jointNames;
    jointNames +=
        string(jointsV[0]), string(jointsV[1]), string(jointsV[2]),
        string(jointsV[3]), string(jointsV[4]), string(jointsV[5]),
        string(jointsV[6]), string(jointsV[7]), string(jointsV[8]),
        string(jointsV[9]), string(jointsV[10]), string(jointsV[11]),
        string(jointsV[12]), string(jointsV[13]), string(jointsV[14]),
        string(jointsV[15]), string(jointsV[16]), string(jointsV[17]),
        string(jointsV[18]), string(jointsV[19]), string(jointsV[20]),
        string(jointsV[21]);

    try{
        alfastaccessJoints->ConnectToVariables(broker,jointNames);
    } catch(AL::ALError& a) {
        std::cout << "ALTranscriber " << a.toString() << std::endl;
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
        string("Device/SubDeviceList/InertialSensor/AccX/Sensor/Value"),
        string("Device/SubDeviceList/InertialSensor/AccY/Sensor/Value"),
        string("Device/SubDeviceList/InertialSensor/AccZ/Sensor/Value"),
        string("Device/SubDeviceList/InertialSensor/GyrX/Sensor/Value"),
        string("Device/SubDeviceList/InertialSensor/GyrY/Sensor/Value"),
        string("Device/SubDeviceList/InertialSensor/AngleX/Sensor/Value"),
        string("Device/SubDeviceList/InertialSensor/AngleY/Sensor/Value");

    try {
        alfastaccessSensors->ConnectToVariables(broker, sensorNames);
    } catch(AL::ALError& a) {
        std::cout << "ALTranscriber " << a.toString() << std::endl;
    }


    vector<string> jointTemperatureNames;
    jointTemperatureNames +=
        string(jointsT[0]), string(jointsT[1]), string(jointsT[2]),
        string(jointsT[3]), string(jointsT[4]), string(jointsT[5]),
        string(jointsT[6]), string(jointsT[7]), string(jointsT[8]),
        string(jointsT[9]), string(jointsT[10]), string(jointsT[11]),
        string(jointsT[12]), string(jointsT[13]), string(jointsT[14]),
        string(jointsT[15]), string(jointsT[16]), string(jointsT[17]),
        string(jointsT[18]), string(jointsT[19]), string(jointsT[20]),
        string(jointsT[21]);

    try{
        alfastaccessTemps->ConnectToVariables(broker,jointTemperatureNames);
    }catch(AL::ALError& a) {
        std::cout << "ALTranscriber " << a.toString() << std::endl;
    }

}


// for marvin!
static const float ACCEL_CONVERSION_X = (-GRAVITY_mss) / 50.0f;
static const float ACCEL_CONVERSION_Y = (-GRAVITY_mss) / 54.0f;
static const float ACCEL_CONVERSION_Z = (-GRAVITY_mss) / 56.5f;

const float ALTranscriber::calibrate_acc_x(const float x) {
    return x * ACCEL_CONVERSION_X;
}

const float ALTranscriber::calibrate_acc_y(const float y) {
    return y * ACCEL_CONVERSION_Y;
}

const float ALTranscriber::calibrate_acc_z(const float z) {
    return z * ACCEL_CONVERSION_Z;
}

void ALTranscriber::syncMotionWithALMemory() {
    alfastaccessJoints->GetValues(jointValues);
    sensors->setBodyAngles(jointValues);

    static vector<float> jointTemps(NUM_JOINTS,0.0f);
    alfastaccessTemps->GetValues(jointTemps);
    sensors->setBodyTemperatures(jointTemps);

    // There are 16 sensor values we want.
    // The vector is static so that it is initialized only once for this
    // method.
    static vector<float> sensorValues(16, 0.0f);
    alfastaccessSensors->GetValues(sensorValues);

    // The indices here are determined by the order in which we requested
    // the sensors values (see initSyncWithALMemory).
    const float LfrontLeft = sensorValues[0], LfrontRight = sensorValues[1],
        LrearLeft = sensorValues[2], LrearRight = sensorValues[3],
        RfrontLeft = sensorValues[4], RfrontRight = sensorValues[5],
        RrearLeft = sensorValues[6], RrearRight = sensorValues[7];

    const float accX = calibrate_acc_x(sensorValues[8]),
        accY = calibrate_acc_y(sensorValues[9]),
        accZ = calibrate_acc_z(sensorValues[10]),
        gyrX = sensorValues[11], gyrY = sensorValues[12],
        angleX = clip(sensorValues[13],-M_PI_FLOAT,M_PI_FLOAT),
        angleY = clip(sensorValues[14],-M_PI_FLOAT,M_PI_FLOAT);

    accelerationFilter.update(accX, accY, accZ);
    const float filteredX = accelerationFilter.getX();
    const float filteredY = accelerationFilter.getY();
    const float filteredZ = accelerationFilter.getZ();

    //TODO: don't allocate these FSR, etc objects each time
    sensors->
        setMotionSensors(FSR(LfrontLeft, LfrontRight, LrearLeft, LrearRight),
                         FSR(RfrontLeft, RfrontRight, RrearLeft, RrearRight),
                         Inertial(filteredX, filteredY, filteredZ,
                                  gyrX, gyrY, angleX, angleY),
                         Inertial(accX, accY, accZ,
                                  gyrX, gyrY, angleX, angleY));
}


void ALTranscriber::initSyncVisionWithALMemory() {
    try{
        alfastaccessVision =
            AL::ALPtr<ALMemoryFastAccess >(new ALMemoryFastAccess());
    } catch(AL::ALError &e){
        cout << "Failed to initialize proxy to ALFastAccess"<<endl;
    }

    vector<string> varNames;
    varNames += string("Device/SubDeviceList/LFoot/Bumper/Left/Sensor/Value"),
        string("Device/SubDeviceList/LFoot/Bumper/Right/Sensor/Value"),
        string("Device/SubDeviceList/RFoot/Bumper/Left/Sensor/Value"),
        string("Device/SubDeviceList/RFoot/Bumper/Right/Sensor/Value"),
        string("Device/SubDeviceList/ChestBoard/Button/Sensor/Value"),
        string("Device/SubDeviceList/US/Sensor/Value"),
        string("Device/SubDeviceList/US/Actuator/Value"),
        string("Device/SubDeviceList/Battery/Charge/Sensor/Value"),
        string("Device/SubDeviceList/Battery/Current/Sensor/Value");

    alfastaccessVision->ConnectToVariables(broker,varNames);
}

void ALTranscriber::syncVisionWithALMemory() {
    static vector<float> varValues(6, 0.0f);
    alfastaccessVision->GetValues(varValues);

    const float leftFootBumperLeft  = varValues[0],
        leftFootBumperRight  = varValues[1];
    const float rightFootBumperLeft = varValues[2],
        rightFootBumperRight = varValues[3];
    const float chestButton = varValues[4];
    const float ultraSoundDist = varValues[5];
    const int ultraSoundMode = static_cast<int>(varValues[6]);
    const float batteryCharge = varValues[7];
    const float batteryCurrent = varValues[8];

    sensors->
        setVisionSensors(FootBumper(leftFootBumperLeft, leftFootBumperRight),
                         FootBumper(rightFootBumperLeft, rightFootBumperRight),
                         chestButton,
                         ultraSoundDist,
                         // UltraSoundMode is just an enum
                         static_cast<UltraSoundMode> (ultraSoundMode),
                         batteryCharge,
                         batteryCurrent);
}
