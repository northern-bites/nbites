#include "ALEnactor.h"

#include <iostream>
using namespace AL;
const int ALEnactor::MOTION_FRAME_RATE = 50;
// 1 second * 1000 ms/s * 1000 us/ms
const float ALEnactor::MOTION_FRAME_LENGTH_uS = 1.0f * 1000.0f * 1000.0f / ALEnactor::MOTION_FRAME_RATE;
const float ALEnactor::MOTION_FRAME_LENGTH_S = 1.0f / ALEnactor::MOTION_FRAME_RATE;

#define NO_ACTUAL_MOTION
#define SPEEDY_ENACTOR

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
        vector<float> result = switchboard->getNextJoints();

#ifdef DEBUG_ENACTOR_JOINTS
        for (int i=0; i<result.size();i++)
            cout << "result of joint " << i << " is " << result.at(i) << endl;
#endif

#ifndef NO_ACTUAL_MOTION
        almotion->setBodyAngles(result);
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

    // This call syncs all sensors values: bumpers, fsr, inertial, etc.
    syncWithALMemory();

}

// from George: Forgive me for the variable names, but there are just too
// many of them to figure out decent names for all. Feel free to change them...
// they are only used internally in this method.
void ALEnactor::syncWithALMemory() {
    // FSR update
    // a cap L means for the left foot. a cap R - for the right foot
    float LfrontLeft = 0.0f, LfrontRight = 0.0f,
        LrearLeft = 0.0f, LrearRight = 0.0f,
        RfrontLeft = 0.0f, RfrontRight = 0.0f,
        RrearLeft = 0.0f, RrearRight = 0.0f;

    try {
        LfrontLeft = almemory->call<const ALValue>(
            "getData",string(
                "Device/SubDeviceList/LFoot/FSR/FrontLeft/Sensor/Value"), 0);
        LfrontRight = almemory->call<const ALValue>(
            "getData",string(
                "Device/SubDeviceList/LFoot/FSR/FrontRight/Sensor/Value"), 0);
        LrearLeft = almemory->call<const ALValue>(
            "getData",string(
                "Device/SubDeviceList/LFoot/FSR/RearLeft/Sensor/Value"), 0);
        LrearRight = almemory->call<const ALValue>(
            "getData",string(
                "Device/SubDeviceList/LFoot/FSR/RearRight/Sensor/Value"), 0);
    } catch(ALError &e) {
        cout << "Failed to read left foot FSR values" << endl;
    }
    //sensors->setLeftFootFSR(frontLeft, frontRight, rearLeft, rearRight);

    try {
        RfrontLeft = almemory->call<const ALValue>(
            "getData",string(
                "Device/SubDeviceList/RFoot/FSR/FrontLeft/Sensor/Value"), 0);
        RfrontRight = almemory->call<const ALValue>(
            "getData",string(
                "Device/SubDeviceList/RFoot/FSR/FrontRight/Sensor/Value"), 0);
        RrearLeft = almemory->call<const ALValue>(
            "getData",string(
                "Device/SubDeviceList/RFoot/FSR/RearLeft/Sensor/Value"), 0);
        RrearRight = almemory->call<const ALValue>(
            "getData",string(
                "Device/SubDeviceList/RFoot/FSR/RearRight/Sensor/Value"), 0);
    } catch(ALError &e) {
        cout << "Failed to read right foot FSR values" << endl;
    }
    //sensors->setRightFootFSR(frontLeft, frontRight, rearLeft, rearRight);

    // Foot bumper update
    float leftFootBumperLeft  = 0.0f, leftFootBumperRight  = 0.0f;
    float rightFootBumperLeft = 0.0f, rightFootBumperRight = 0.0f;
    try {
        leftFootBumperLeft = almemory->call<const ALValue>(
            "getData",string(
                "Device/SubDeviceList/LFoot/Bumper/Left/Sensor/Value"), 0);
        leftFootBumperRight = almemory->call<const ALValue>(
            "getData",string(
                "Device/SubDeviceList/LFoot/Bumper/Right/Sensor/Value"), 0);
        rightFootBumperLeft = almemory->call<const ALValue>(
            "getData",string(
                "Device/SubDeviceList/RFoot/Bumper/Left/Sensor/Value"), 0);
        rightFootBumperRight = almemory->call<const ALValue>(
            "getData",string(
                "Device/SubDeviceList/RFoot/Bumper/Right/Sensor/Value"), 0);
    } catch(ALError &e) {
        cout << "Failed to read bumper values" <<endl;
    }
    //sensors->setLeftFootBumper(leftFootBumperLeft, leftFootBumperRight);
    //sensors->setRightFootBumper(rightFootBumperLeft, rightFootBumperRight);

    // Inertial values. This includes, accelerometers, gyros and the angleX,
    // angleY filtered values which denote body tilt.
    float accX = 0.0f, accY = 0.0f, accZ = 0.0f,
        gyrX = 0.0f, gyrY = 0.0f,
        angleX = 0.0f, angleY = 0.0f;
    try {
        accX = almemory->call<const ALValue>(
            "getData", string(
                "Device/SubDeviceList/InertialSensor/AccX/Sensor/Value"), 0);
        accY = almemory->call<const ALValue>(
            "getData", string(
                "Device/SubDeviceList/InertialSensor/AccY/Sensor/Value"), 0);
        accZ = almemory->call<const ALValue>(
            "getData", string(
                "Device/SubDeviceList/InertialSensor/AccZ/Sensor/Value"), 0);
        gyrX = almemory->call<const ALValue>(
            "getData", string(
                "Device/SubDeviceList/InertialSensor/GyrX/Sensor/Value"), 0);
        gyrY = almemory->call<const ALValue>(
            "getData", string(
                "Device/SubDeviceList/InertialSensor/GyrY/Sensor/Value"), 0);
        angleX = almemory->call<const ALValue>(
            "getData", string(
                "Device/SubDeviceList/InertialSensor/AngleX/Sensor/Value"), 0);
        angleY = almemory->call<const ALValue>(
            "getData", string(
                "Device/SubDeviceList/InertialSensor/AngleY/Sensor/Value"), 0);
    } catch(ALError &e) {
        cout << "Failed to read inertial unit values" << endl;
    }
    //sensors->setInertial(accX, accY, accZ, gyrX, gyrY, angleX, angleY);

    static int counter = 0;
    float ultraSoundDistance = 0.0f;
    // Ultrasound values
    try {
        // This is testing code which sends a new value to the actuator every
        // 20 frames. It also cycles the ultrasound mode between the four
        // possibilities. See docs.
        ALValue commands;

        commands.arraySetSize(3);
        commands[0] = string("US/Actuator/Value");
        commands[1] = string("Merge");
        commands[2].arraySetSize(1);
        commands[2][0].arraySetSize(2);
        // the current mode - changes every 5 frames
        commands[2][0][0] = static_cast<float>(counter / 5);
        commands[2][0][1] = dcm->getTime(250);

        dcm->set(commands);

        counter++;

        if (counter > 20)
            counter = 0;

        ultraSoundDistance = almemory->call<const ALValue>(
            "getData", string(
                "Device/SubDeviceList/US/Sensor/Value"), 0);

        float mode = almemory->call<const ALValue>(
            "getData", string(
                "Device/SubDeviceList/US/Actuator/Value"), 0);

    } catch(ALError &e) {
        cout << "Failed to read ultrasound distance values" << endl;
    }

    //sensors->setUltraSound(ultraSoundDistance);
    sensors->
        setSensorsEnMasse(FSR(LfrontLeft, LfrontRight, LrearLeft, LrearRight),
                          FSR(RfrontLeft, RfrontRight, RrearLeft, RrearRight),
                          FootBumper(leftFootBumperLeft, leftFootBumperRight),
                          FootBumper(rightFootBumperLeft, rightFootBumperRight),
                          Inertial(accX,accY,accZ,gyrX,gyrY,angleX,angleY),
                          ultraSoundDistance);
}

#endif//NAOQI1
