#include "WBTranscriber.h"
#include <webots/robot.h>
#include <webots/accelerometer.h>
#include <webots/gyro.h>
#include <webots/servo.h>
#include <webots/touch_sensor.h>
#include <webots/distance_sensor.h>

#include "Kinematics.h"
#include <cmath>
using boost::shared_ptr;
using namespace std;
using namespace Kinematics;
#include "WBNames.h"
using namespace WBNames;

//#define COMPUTE_WEBOTS_ANGLE

#include "BasicWorldConstants.h"

WBTranscriber::WBTranscriber(shared_ptr<Sensors> s)
    :Transcriber(s),
     jointValues(NUM_JOINTS,0.0f),
     jointDevices(NUM_JOINTS),
     fsrValues(NUM_FSR,0.0f),
     fsrDevices(NUM_FSR),
     angleEKF()
{

    //assign and enable the joint devices
    for(unsigned int joint = 0; joint < NUM_JOINTS; joint++){
        const string devName = JOINT_STRINGS[joint];
        jointDevices[joint] = wb_robot_get_device(devName.c_str());
        wb_servo_enable_position (jointDevices[joint], 20);
    }

    //enable the inertials
    acc = wb_robot_get_device("accelerometer");
    gyro = wb_robot_get_device("gyro");
    wb_accelerometer_enable (acc, 20);
    wb_gyro_enable (gyro, 20);

    us1 = wb_robot_get_device("US/TopRight");
    us2 = wb_robot_get_device("US/BottomRight");
    us3 = wb_robot_get_device("US/TopLeft");
    us4 = wb_robot_get_device("US/BottomLeft");

    wb_distance_sensor_enable(us1,40);
    wb_distance_sensor_enable(us2,40);
    wb_distance_sensor_enable(us3,40);
    wb_distance_sensor_enable(us4,40);

    //enable the FSRs
    for(unsigned int fsr = 0; fsr < NUM_FSR; fsr++){
        fsrDevices[fsr] = wb_robot_get_device(FSR_CORE[fsr].c_str());
        wb_touch_sensor_enable(fsrDevices[fsr], 20);
    }

    //initialize prevAngleX, prevAngleY and the angle EKF
    prevAngleX = 0;
    prevAngleY = 0;
}


WBTranscriber::~WBTranscriber(){}



void WBTranscriber::postVisionSensors(){
    //The following sensors need to updated on the vision cycle
    //Bumpers
    //Battery
    //UltraSound
    const float lFBL = 0.0f;
    const float rFBL = 0.0f;
    const float lFBR = 0.0f;
    const float rFBR = 0.0f;

    const float usd1 = static_cast<float>(wb_distance_sensor_get_value(us1));
    const float usd2 = static_cast<float>(wb_distance_sensor_get_value(us2));
    const float usd3 = static_cast<float>(wb_distance_sensor_get_value(us3));
    const float usd4 = static_cast<float>(wb_distance_sensor_get_value(us4));

    const float usDist1 = std::min(usd3,usd4);
    const float usDist2 = std::min(usd1,usd2);

    const float batteryCharge = 1.0;
    const float batteryCurrent = 0.0;
    sensors->
        setVisionSensors(FootBumper(lFBL, rFBL),
                         FootBumper(lFBR, rFBR),
                         usDist1,
                         usDist2,
                         batteryCharge,
                         batteryCurrent);
}

const boost::tuple<const float, const float>
WBTranscriber::angleWrapper(const float accX, const float accY,
                            const float gyroX,const float gyroY){

    /*
     * This method still needs some serious work. TODO: fix implementation
     * of kalman filter, and fix Nan issue on webots reload
     */

#ifdef COMPUTE_WEBOTS_ANGLE

    //    cout<<"anglex "<<std::asin(-accX/GRAVITY_mss)*(180/3.14)<<"\n";
    //    cout<<"angley "<<std::asin(-accY/GRAVITY_mss)*(180/3.14)<<"\n";
    //the ratio can be bigger than 1 or smaller than -1
    //if the robot hits the floor
    //gyroX does not correspond to accX so we have to invert them
    const float ratioX = -accX/GRAVITY_mss;
    const float ratioY = -accY/GRAVITY_mss;
    float accAngleX = std::asin(ratioX);
    float accAngleY = std::asin(ratioY);

    if (ratioX >= 1){
        accAngleX = M_PI_FLOAT/2;
        prevAngleX = M_PI_FLOAT/2;
    }
    else
        if (ratioX <= -1){
            accAngleX = -M_PI_FLOAT/2;
            prevAngleX = -M_PI_FLOAT/2;
        }

    if (ratioY >= 1){
        accAngleY = M_PI_FLOAT/2;
        prevAngleY = M_PI_FLOAT/2;
    }
    else
        if (ratioY <= -1){
            accAngleY = -M_PI_FLOAT/2;
            prevAngleY = -M_PI_FLOAT/2;
        }
    angleEKF.update(accAngleX, accAngleY);

    if (accAngleX <= .002 && accAngleX >= -.002)
        prevAngleX = 0;
    if (accAngleY <= .002 && accAngleY >= -.002)
        prevAngleY = 0;

    //multiplying with .02 since gyro gives you rad/s and
    //the updating happens every 20 ms
    angleEKF.update(prevAngleX + gyroX*0.02, prevAngleY + gyroY*0.02);
    //    cout<<"anglex gyro "<<(prevAngleX + gyroX*0.02)* (180/3.14)<<"\n";
    //    cout<<"angley gyro "<<(prevAngleY + gyroY*0.02)* (180/3.14)<<"\n";
    const float angleX = angleEKF.getAngleX();
    const float angleY = angleEKF.getAngleY();

    prevAngleX += gyroX*0.02;
    prevAngleY += gyroY*0.02;


    //cout<<"angleX: "<<angleX*TO_DEG<<endl<<"angleY: "<<angleY*TO_DEG<<"\n";

    //HACK!!!! TODO compute angleX and angleY better (filter?)
    //Currently when the gravity accell is in one direction,
    //we use that to consider that the robot is rotated along the other axis
    //     const float angleX = accY/GRAVITY_mss * M_PI_FLOAT;
    //     const float angleY = -accX/GRAVITY_mss * M_PI_FLOAT;
    //better approximation, for now


    return boost::tuple<const float, const float>(angleX, angleY);
#else
    return boost::tuple<const float, const float>(0.0f, 0.0f);
#endif

}

void WBTranscriber::postMotionSensors(){
    //The following sensors need to be updated on the motion cycle:
    //Foot sensors
    //Button (always off)
    //Inertials (including angleX!)
    //Joints
    //Temperatures (always zero)


    //Inertials
    const double *acc_values = wb_accelerometer_get_values (acc);
    const double *gyro_values = wb_gyro_get_values (gyro);


    //webots units are already in m/ss, but the signs may be wack...
    const float accX = -static_cast<float>(acc_values[0]);
    const float accY = -static_cast<float>(acc_values[1]);
    const float accZ = -static_cast<float>(acc_values[2]);

    const float gyroY = static_cast<float>(gyro_values[0]);
    const float gyroX = static_cast<float>(gyro_values[1]);

    const boost::tuple<const float, const float> angles =
        angleWrapper(accX,
                     accY,
                     gyroX,
                     gyroY);

    const float angleX = angles.get<0>();
    const float angleY = angles.get<1>();

    Inertial wbInertial= Inertial(accX,accY,accZ,gyroX,gyroY,angleX,angleY);

    float chestButton = 0.0f; //always off

    //FSRs
    for(unsigned int fsr = 0 ; fsr< NUM_FSR; fsr++){
        fsrValues[fsr] =
            static_cast<float>(wb_touch_sensor_get_value(fsrDevices[fsr]));
    }


    FSR leftFSR = FSR(fsrValues[LFSR_FL],
                      fsrValues[LFSR_FR],
                      fsrValues[LFSR_RL],
                      fsrValues[LFSR_RR]);

    FSR rightFSR = FSR(fsrValues[RFSR_FL],
                       fsrValues[RFSR_FR],
                       fsrValues[RFSR_RL],
                       fsrValues[RFSR_RR]);

    //Put all the structs, etc together, and send them to sensors
    sensors->setMotionSensors(leftFSR, rightFSR, chestButton,
                              wbInertial, wbInertial);

    //Joint Angles
    for(unsigned int joint = 0; joint < NUM_JOINTS; joint++){
        jointValues[joint] =
            static_cast<float>(wb_servo_get_position(jointDevices[joint]));
    }
    sensors->setBodyAngles(jointValues);

    //Joint Temperatures (always zeros)
    vector<float> jointTemps(NUM_JOINTS,0.0f);
    sensors->setBodyTemperatures(jointTemps);

}
