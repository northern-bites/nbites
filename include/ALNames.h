#ifndef _ALNames_DEFINED
#define _ALNames_DEFINED

#include "Kinematics.h"

namespace ALNames {
    //Set hardware values- nominal speed in rad/20ms
    //from http://robocup.aldebaran-robotics.com/docs/reddoc/hardware.php
    //M=motor r = reduction ratio

    static const float M1R1_NOMINAL = 0.0658;
    static const float M1R2_NOMINAL = 0.1012;
    static const float M2R1_NOMINAL = 0.1227;
    static const float M2R2_NOMINAL = 0.1065;

    static const float M1R1_NO_LOAD = 0.08308;
    static const float M1R2_NO_LOAD = 0.1279;
    static const float M2R1_NO_LOAD = 0.16528;
    static const float M2R2_NO_LOAD = 0.1438;

    static const float M1R1_AVG = 0.07435;
    static const float M1R2_AVG = 0.1146;
    static const float M2R1_AVG = 0.14399;
    static const float M2R2_AVG = 0.12514;


    static const string PositionPostFix = "/Position/Actuator/Value";
    static const string HardnessPostFix = "/Hardness/Actuator/Value";
    static const string ValuePostFix    = "/Position/Sensor/Value";
    static const string ValuePreFix     = "Device/SubDeviceList/";

    static const string jointsP[Kinematics::NUM_JOINTS] = {
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
        //overrides previous LHipYawPitch, check how this functions in motionprovider
        "LHipYawPitch" + PositionPostFix,
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
    static const string jointsH[Kinematics::NUM_JOINTS] = {
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
        //overrides previous LHipYawPitch, check how this functions in motionprovider
        "LHipYawPitch" + HardnessPostFix,
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
    static const string jointsV[Kinematics::NUM_JOINTS] = {
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
        //this double subscription is intentional
        ValuePreFix + "LHipYawPitch" + ValuePostFix,
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
    static const float jointsMaxNominal[Kinematics::NUM_JOINTS] = {
        //head
        M2R2_NOMINAL, M2R1_NOMINAL,
        //left arm
        M2R1_NOMINAL, M2R2_NOMINAL, M2R1_NOMINAL, M2R2_NOMINAL,
        //left leg
        M1R1_NOMINAL, M1R1_NOMINAL, M1R2_NOMINAL,
        M1R2_NOMINAL, M1R2_NOMINAL, M1R1_NOMINAL,
        //right leg
        M1R1_NOMINAL, M1R1_NOMINAL, M1R2_NOMINAL,
        M1R2_NOMINAL, M1R2_NOMINAL, M1R1_NOMINAL,
        //right arm
        M2R2_NOMINAL, M2R2_NOMINAL, M2R1_NOMINAL, M2R2_NOMINAL
    };

    static const float jointsMaxNoLoad[Kinematics::NUM_JOINTS] = {
        //head
        M2R2_NO_LOAD, M2R1_NO_LOAD,
        //left arm
        M2R1_NO_LOAD, M2R2_NO_LOAD, M2R1_NO_LOAD, M2R2_NO_LOAD,
        //left leg
        M1R1_NO_LOAD, M1R1_NO_LOAD, M1R2_NO_LOAD,
        M1R2_NO_LOAD, M1R2_NO_LOAD, M1R1_NO_LOAD,
        //right leg
        M1R1_NO_LOAD, M1R1_NO_LOAD, M1R2_NO_LOAD,
        M1R2_NO_LOAD, M1R2_NO_LOAD, M1R1_NO_LOAD,
        //right arm
        M2R2_NO_LOAD, M2R2_NO_LOAD, M2R1_NO_LOAD, M2R2_NO_LOAD
    };

    static const float jointsMaxAvg[Kinematics::NUM_JOINTS] = {
        //head
        M2R2_AVG, M2R1_AVG,
        //left arm
        M2R1_AVG, M2R2_AVG, M2R1_AVG, M2R2_AVG,
        //left leg
        M1R1_AVG, M1R1_AVG, M1R2_AVG,
        M1R2_AVG, M1R2_AVG, M1R1_AVG,
        //right leg
        M1R1_AVG, M1R1_AVG, M1R2_AVG,
        M1R2_AVG, M1R2_AVG, M1R1_AVG,
        //right arm
        M2R2_AVG, M2R2_AVG, M2R1_AVG, M2R2_AVG
    };

}
#endif
