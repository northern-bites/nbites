#include "ALEnactor.h"

#include <iostream>
#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

using namespace AL;
const int ALEnactor::MOTION_FRAME_RATE = 50;
// 1 second * 1000 ms/s * 1000 us/ms
const float ALEnactor::MOTION_FRAME_LENGTH_uS = 1.0f * 1000.0f * 1000.0f / ALEnactor::MOTION_FRAME_RATE;
const float ALEnactor::MOTION_FRAME_LENGTH_S = 1.0f / ALEnactor::MOTION_FRAME_RATE;

//#define SPEEDY_ENACTOR

#ifdef NAOQI1
void ALEnactor::run() {
    std::cout << "ALEnactor::run()" << std::endl;

#ifdef NO_ACTUAL_MOTION
    almotion->setBodyStiffness(0.0f, 0.1f);
#else
    almotion->setBodyStiffness(0.80f, 0.1f);
#endif

    //vector<float> rarm(4,M_PI/4);
    //almotion->gotoChainAngles("RArm",rarm,2.0,INTERPOLATION_LINEAR);
    //cout << "Done with AL motion stuff" << endl;

    long long currentTime;
    while (running) {
        currentTime = micro_time();

        if(!switchboard){
            cout<< "Caution!! Switchboard is null, exiting ALEnactor"<<endl;
            break;
        }
        // Get the angles we want to go to this frame from the switchboard
        motionCommandAngles = switchboard->getNextJoints();

#ifdef DEBUG_ENACTOR_JOINTS
        for (unsigned int i=0; i<motionCommandAngles.size();i++)
            cout << "result of joint " << i << " is "
                 << motionCommandAngles.at(i) << endl;
#endif

#ifndef NO_ACTUAL_MOTION
        almotion->setBodyAngles(motionCommandAngles);
#endif

        //Once we've sent the most calculated joints
        postSensors();

        const long long zero = 0;
        const long long processTime = micro_time() - currentTime;

#if ! defined OFFLINE || ! defined SPEEDY_ENACTOR
        if (processTime > MOTION_FRAME_LENGTH_uS){
            cout << "Time spent in ALEnactor longer than frame length: "
                 << processTime <<endl;
            //Don't sleep at all
        } else{
            usleep(static_cast<useconds_t>(MOTION_FRAME_LENGTH_uS
                                           -processTime));
        }
#endif


    }
}

void ALEnactor::postSensors() {
    //At the beginning of each cycle, we need to update the sensor values
    //We also call this from the Motion run method
    //This is important to ensure that the providers have access to the
    //actual joint post of the robot before any computation begins
    vector<float> alAngles = almotion->getBodyAngles();
    sensors->setBodyAngles(alAngles);
    sensors->setMotionBodyAngles(motionCommandAngles);
    vector<float> temp = sensors->getMotionBodyAngles();
    //for (int i = 2; i < 6; i++)cout << "arm angles are"<< temp[i] <<endl;
    // This call syncs all sensors values: bumpers, fsr, inertial, etc.
    syncWithALMemory();

}

/**
 * ALFastAccess allows us to pull out values from ALMemory a lot faster
 * and in bulk. The order in which we declare the desired devices are also
 * the order in which we receive them (see syncWithALMemory).
 * In this class we only sync the sensors values we need for motion. The
 * rest are synced in Man.cpp (may change).
 */
void ALEnactor::initSyncWithALMemory(){
    vector<string> varNames;
    varNames +=
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

    alfastaccess->ConnectToVariables(broker,varNames);

}

// from George: Forgive me for the variable names, but there are just too
// many of them to figure out decent names for all. Feel free to change them...
// they are only used internally in this method.
void ALEnactor::syncWithALMemory() {
    static vector<float> varValues(16,0.0);
    alfastaccess->GetValues(varValues);

    /*
    cout << "****** Sensors values ******" << endl;
    for (int i = 0; i < 20; i++) {
        cout << varValues[i] <<endl;
    }
    cout << endl;
    */

    // The indices here are determined by the order in which we requested
    // the sensors values (see initSyncWithALMemory).
    const float LfrontLeft = varValues[0], LfrontRight = varValues[1],
        LrearLeft = varValues[2], LrearRight = varValues[3],
        RfrontLeft = varValues[4], RfrontRight = varValues[5],
        RrearLeft = varValues[6], RrearRight = varValues[7];

    const float accX = varValues[8], accY = varValues[9], accZ = varValues[10],
        gyrX = varValues[11], gyrY = varValues[12],
        angleX = varValues[13], angleY = varValues[14];

    sensors->
        setMotionSensors(FSR(LfrontLeft, LfrontRight, LrearLeft, LrearRight),
                         FSR(RfrontLeft, RfrontRight, RrearLeft, RrearRight),
                         Inertial(accX,accY,accZ,gyrX,gyrY,angleX,angleY));
}

#endif//NAOQI1
