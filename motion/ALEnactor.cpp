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

    while (running) {
        postSensors();

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

        // TODO: This is probably wrong!!!!1!ONE
        // We probably want to sleep webots time and this sleeps real time.
#if ! defined OFFLINE || ! defined SPEEDY_ENACTOR
        usleep(static_cast<useconds_t>(MOTION_FRAME_LENGTH_uS));
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

void ALEnactor::syncWithALMemory() {
    // FSR update
    float frontLeft = 0.0f, frontRight = 0.0f,
        rearLeft = 0.0f, rearRight = 0.0f;
    try {
        frontLeft = almemory->call<const ALValue>(
            "getData",string(
                "Device/SubDeviceList/LFoot/FSR/FrontLeft/Sensor/Value"), 0);
        frontRight = almemory->call<const ALValue>(
            "getData",string(
                "Device/SubDeviceList/LFoot/FSR/FrontRight/Sensor/Value"), 0);
        rearLeft = almemory->call<const ALValue>(
            "getData",string(
                "Device/SubDeviceList/LFoot/FSR/RearLeft/Sensor/Value"), 0);
        rearRight = almemory->call<const ALValue>(
            "getData",string(
                "Device/SubDeviceList/LFoot/FSR/RearRight/Sensor/Value"), 0);
    } catch(ALError &e) {
        cout << "Failed to read left foot FSR values" << endl;
    }
    sensors->setLeftFootFSR(frontLeft, frontRight, rearLeft, rearRight);

    try {
        frontLeft = almemory->call<const ALValue>(
            "getData",string(
                "Device/SubDeviceList/RFoot/FSR/FrontLeft/Sensor/Value"), 0);
        frontRight = almemory->call<const ALValue>(
            "getData",string(
                "Device/SubDeviceList/RFoot/FSR/FrontRight/Sensor/Value"), 0);
        rearLeft = almemory->call<const ALValue>(
            "getData",string(
                "Device/SubDeviceList/RFoot/FSR/RearLeft/Sensor/Value"), 0);
        rearRight = almemory->call<const ALValue>(
            "getData",string(
                "Device/SubDeviceList/RFoot/FSR/RearRight/Sensor/Value"), 0);
    } catch(ALError &e) {
        cout << "Failed to read right foot FSR values" << endl;
    }
    sensors->setRightFootFSR(frontLeft, frontRight, rearLeft, rearRight);

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
    sensors->setLeftFootBumper(leftFootBumperLeft, leftFootBumperRight);
    sensors->setRightFootBumper(rightFootBumperLeft, rightFootBumperRight);

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
    sensors->setInertial(accX, accY, accZ, gyrX, gyrY, angleX, angleY);

    static int counter = 0;
    float ultraSoundValue = 0.0f;
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

        ultraSoundValue = almemory->call<const ALValue>(
            "getData", string(
                "Device/SubDeviceList/US/Sensor/Value"), 0);

        float mode = almemory->call<const ALValue>(
            "getData", string(
                "Device/SubDeviceList/US/Actuator/Value"), 0);

    } catch(ALError &e) {
        cout << "Failed to read ultrasound distance values" << endl;
    }
    sensors->setUltraSound(ultraSoundValue);
}

#endif//NAOQI1
