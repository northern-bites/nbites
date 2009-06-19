#ifndef _ALNames_DEFINED
#define _ALNames_DEFINED

#include "Kinematics.h"

namespace ALNames {


    static const string PositionPostFix = "/Position/Actuator/Value";
    static const string HardnessPostFix = "/Hardness/Actuator/Value";
    static const string LedPostFix      = "/Actuator/Value";
    static const string ValuePostFix    = "/Position/Sensor/Value";
    static const string ValuePreFix     = "Device/SubDeviceList/";
    static const string TempValuePostFix= "/Temperature/Sensor/Value";

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
    static const string jointsT[Kinematics::NUM_JOINTS] = {
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

//ChestBoard/Led/Red/Actuator

//Device/SubDeviceList/LFoot/Led/Green/Actuator/Value
//Device/SubDeviceList/Face/Led/Green/Left/90Deg/Actuator/Value
//Face 0,45,90,135,180,225,270,315,
//Ear  0,36,72,108,144,180,216,252,288,324
    static const string FaceLed ="/Face/Led";
    static const string EarLed ="/Ear/Led";


    static const string Green ="/Green";
    static const string Red ="/Red";
    static const string Blue ="/Blue";
    
    static const string Left ="/Left";
    static const string Right ="/Right";

    static const string ledName = ValuePreFix + "Face/Led/" 
        +  Green + Left + "/90Deg"+LedPostFix;

    enum LedColors{
        RED_LED = 0,
        GREEN_LED,
        BLUE_LED
    };
    enum LedOrientation{
        LEFT_LED = 0,
        RIGHT_LED
    };
    static const int NUM_FACE_LEDS = 8;
    static const int NUM_EAR_LEDS = 10;
    static const int NUM_LED_COLORS = 3;

/*  Face Leds Left */
    static const string leftFaceRedL[NUM_FACE_LEDS] ={
        ValuePreFix + FaceLed +  Red + Left + "/0Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Red + Left + "/35Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Red + Left + "/90Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Red + Left + "/135Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Red + Left + "/180Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Red + Left + "/225Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Red + Left + "/270Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Red + Left + "/315Deg"+LedPostFix,
    };
    static const string leftFaceGreenL[NUM_FACE_LEDS] ={
        ValuePreFix + FaceLed +  Green + Left + "/0Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Green + Left + "/35Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Green + Left + "/90Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Green + Left + "/135Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Green + Left + "/180Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Green + Left + "/225Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Green + Left + "/270Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Green + Left + "/315Deg"+LedPostFix,
    };
    static const string leftFaceBlueL[NUM_FACE_LEDS] ={
        ValuePreFix + FaceLed +  Blue + Left + "/0Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Blue + Left + "/35Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Blue + Left + "/90Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Blue + Left + "/135Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Blue + Left + "/180Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Blue + Left + "/225Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Blue + Left + "/270Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Blue + Left + "/315Deg"+LedPostFix,
    };
/* Face Leds Right */
    static const string rightFaceRedL[NUM_FACE_LEDS] ={
        ValuePreFix + FaceLed +  Red + Right + "/0Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Red + Right + "/35Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Red + Right + "/90Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Red + Right + "/135Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Red + Right + "/180Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Red + Right + "/225Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Red + Right + "/270Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Red + Right + "/315Deg"+LedPostFix,
    };
    static const string rightFaceGreenL[NUM_FACE_LEDS] ={
        ValuePreFix + FaceLed +  Green + Right + "/0Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Green + Right + "/35Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Green + Right + "/90Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Green + Right + "/135Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Green + Right + "/180Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Green + Right + "/225Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Green + Right + "/270Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Green + Right + "/315Deg"+LedPostFix,
    };
    static const string rightFaceBlueL[NUM_FACE_LEDS] ={
        ValuePreFix + FaceLed +  Blue + Right + "/0Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Blue + Right + "/35Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Blue + Right + "/90Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Blue + Right + "/135Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Blue + Right + "/180Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Blue + Right + "/225Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Blue + Right + "/270Deg"+LedPostFix,
        ValuePreFix + FaceLed +  Blue + Right + "/315Deg"+LedPostFix,
    };

/* Ear Led Left*/
    static const string leftEarL[NUM_EAR_LEDS] ={
        ValuePreFix + EarLed + Left + "/0Deg"+LedPostFix,
        ValuePreFix + EarLed + Left + "/36Deg"+LedPostFix,
        ValuePreFix + EarLed + Left + "/72Deg"+LedPostFix,
        ValuePreFix + EarLed + Left + "/108Deg"+LedPostFix,
        ValuePreFix + EarLed + Left + "/144Deg"+LedPostFix,
        ValuePreFix + EarLed + Left + "/180Deg"+LedPostFix,
        ValuePreFix + EarLed + Left + "/216Deg"+LedPostFix,
        ValuePreFix + EarLed + Left + "/252Deg"+LedPostFix,
        ValuePreFix + EarLed + Left + "/288Deg"+LedPostFix,
        ValuePreFix + EarLed + Left + "/324Deg"+LedPostFix,
    };
/* Ear Led Right*/
    static const string rightEarL[NUM_EAR_LEDS] ={
        ValuePreFix + EarLed + Left + "/0Deg"+LedPostFix,
        ValuePreFix + EarLed + Left + "/36Deg"+LedPostFix,
        ValuePreFix + EarLed + Left + "/72Deg"+LedPostFix,
        ValuePreFix + EarLed + Left + "/108Deg"+LedPostFix,
        ValuePreFix + EarLed + Left + "/144Deg"+LedPostFix,
        ValuePreFix + EarLed + Left + "/180Deg"+LedPostFix,
        ValuePreFix + EarLed + Left + "/216Deg"+LedPostFix,
        ValuePreFix + EarLed + Left + "/252Deg"+LedPostFix,
        ValuePreFix + EarLed + Left + "/288Deg"+LedPostFix,
        ValuePreFix + EarLed + Left + "/324Deg"+LedPostFix,
    };
};
#endif
