#ifndef _ALNames_DEFINED
#define _ALNames_DEFINED

#include <string>

namespace ALNames {


    static const std::string PositionPostFix = "/Position/Actuator/Value";
    static const std::string HardnessPostFix = "/Hardness/Actuator/Value";
    static const std::string LedPostFix      = "/Actuator/Value";
    static const std::string ValuePostFix    = "/Position/Sensor/Value";
    static const std::string ValuePreFix     = "Device/SubDeviceList/";
    static const std::string TempValuePostFix= "/Temperature/Sensor/Value";

    static const std::string jointsP[Kinematics::NUM_JOINTS] = {
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
    static const std::string jointsH[Kinematics::NUM_JOINTS] = {
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
    static const std::string jointsV[Kinematics::NUM_JOINTS] = {
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
    static const std::string jointsT[Kinematics::NUM_JOINTS] = {
        ValuePreFix + "HeadYaw" + TempValuePostFix,
        ValuePreFix + "HeadPitch" + TempValuePostFix,
        ValuePreFix + "LShoulderPitch" + TempValuePostFix,
        ValuePreFix + "LShoulderRoll" + TempValuePostFix,
        ValuePreFix + "LElbowYaw" + TempValuePostFix,
        ValuePreFix + "LElbowRoll" + TempValuePostFix,
        ValuePreFix + "LHipYawPitch" + TempValuePostFix,
        ValuePreFix + "LHipRoll" + TempValuePostFix,
        ValuePreFix + "LHipPitch" + TempValuePostFix,
        ValuePreFix + "LKneePitch" + TempValuePostFix,
        ValuePreFix + "LAnklePitch" + TempValuePostFix,
        ValuePreFix + "LAnkleRoll" + TempValuePostFix,
        //this double subscription is intentional
        ValuePreFix + "LHipYawPitch" + TempValuePostFix,
        ValuePreFix + "RHipRoll" + TempValuePostFix,
        ValuePreFix + "RHipPitch" + TempValuePostFix,
        ValuePreFix + "RKneePitch" + TempValuePostFix,
        ValuePreFix + "RAnklePitch" + TempValuePostFix,
        ValuePreFix + "RAnkleRoll" + TempValuePostFix,
        ValuePreFix + "RShoulderPitch" + TempValuePostFix,
        ValuePreFix + "RShoulderRoll" + TempValuePostFix,
        ValuePreFix + "RElbowYaw" + TempValuePostFix,
        ValuePreFix + "RElbowRoll" + TempValuePostFix
    };

};
#endif
