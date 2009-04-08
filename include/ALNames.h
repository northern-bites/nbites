#ifndef _ALNames_DEFINED
#define _ALNames_DEFINED

#include "Kinematics.h"

namespace ALNames {
    static const float M1R1 = 0.1012;
    static const float M1R2 = 0.0658;
    static const float M2R1 = 0.1227;
    static const float M2R2 = 0.1066;

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
    static const float jointsMax[Kinematics::NUM_JOINTS] = {
        //head
        M2R2, M2R1,
        //left arm
        M2R2, M2R1, M2R1, M2R2,
        //left leg
        M1R1, M1R2, M1R2,
        M1R1, M1R2, M1R1,
        //right leg
        M1R1, M1R2, M1R2,
        M1R1, M1R2, M1R1,
        //right arm
        M2R2, M2R1, M2R1, M2R2
    };
}
#endif
