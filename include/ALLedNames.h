#ifndef _ALLedNames_h_DEFINED
#define _ALLedNames_h_DEFINED

#include "ALNames.h"
namespace ALNames {

    static const std::string FaceLed ="Face/Led";
    static const std::string EarLed ="Ear/Led";
    static const std::string LFootLed ="LFoot/Led";
    static const std::string RFootLed ="RFoot/Led";
    static const std::string ChestLed ="ChestBoard/Led";

    static const std::string Green ="/Green";
    static const std::string Red ="/Red";
    static const std::string Blue ="/Blue";

    static const std::string Left ="/Left";
    static const std::string Right ="/Right";

    enum LedColor{
        RED_LED = 0,
        GREEN_LED,
        BLUE_LED
    };
    enum LedOrientation{
        LEFT_LED = 0,
        RIGHT_LED
    };
    static const unsigned int NUM_FACE_LEDS = 8;
    static const unsigned int NUM_EAR_LEDS = 10;
    static const unsigned int NUM_FOOT_LEDS = 1;
    static const unsigned int NUM_CHEST_LEDS = 1;

    static const unsigned int NUM_LED_COLORS = 3;
    static const unsigned int NUM_LED_ORIENTATIONS = 2;
    static const unsigned int NUM_ONE_EYE_LEDS = NUM_LED_COLORS * NUM_FACE_LEDS;

    static const unsigned int NUM_UNIQUE_LEDS = 7;
    static const unsigned int NUM_RGB_LEDS[NUM_UNIQUE_LEDS] ={
        NUM_EAR_LEDS,
        NUM_EAR_LEDS,
        NUM_FACE_LEDS,
        NUM_FACE_LEDS,
        NUM_CHEST_LEDS,
        NUM_FOOT_LEDS,
        NUM_FOOT_LEDS};
    static const unsigned int LED_START_COLOR[NUM_UNIQUE_LEDS]=
    {BLUE_LED,BLUE_LED,RED_LED,RED_LED,RED_LED,RED_LED,RED_LED};
    static const unsigned int LED_END_COLOR[NUM_UNIQUE_LEDS]=
    {NUM_LED_COLORS,NUM_LED_COLORS,
     NUM_LED_COLORS,NUM_LED_COLORS,
     NUM_LED_COLORS,
     NUM_LED_COLORS,NUM_LED_COLORS};

    static const std::string faceL[NUM_LED_ORIENTATIONS][NUM_LED_COLORS][NUM_FACE_LEDS] ={
/*  Face Leds Left */
    {
        /* Red*/
        {ValuePreFix + FaceLed +  Red + Left + std::string("/0Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Red + Left + std::string("/45Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Red + Left + std::string("/90Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Red + Left + std::string("/135Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Red + Left + std::string("/180Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Red + Left + std::string("/225Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Red + Left + std::string("/270Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Red + Left + std::string("/315Deg")+LedPostFix},
        /* Green*/
        {ValuePreFix + FaceLed +  Green + Left + std::string("/0Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Green + Left + std::string("/45Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Green + Left + std::string("/90Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Green + Left + std::string("/135Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Green + Left + std::string("/180Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Green + Left + std::string("/225Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Green + Left + std::string("/270Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Green + Left + std::string("/315Deg")+LedPostFix},
        /* Blue*/
        {ValuePreFix + FaceLed +  Blue + Left + std::string("/0Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Left + std::string("/45Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Left + std::string("/90Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Left + std::string("/135Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Left + std::string("/180Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Left + std::string("/225Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Left + std::string("/270Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Left + std::string("/315Deg")+LedPostFix},
    },
/* Face Leds Right */
    {
        /* Red*/
        {ValuePreFix + FaceLed +  Red + Right + std::string("/0Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Red + Right + std::string("/45Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Red + Right + std::string("/90Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Red + Right + std::string("/135Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Red + Right + std::string("/180Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Red + Right + std::string("/225Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Red + Right + std::string("/270Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Red + Right + std::string("/315Deg")+LedPostFix},
        /* Green*/
        {ValuePreFix + FaceLed +  Green + Right + std::string("/0Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Green + Right + std::string("/45Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Green + Right + std::string("/90Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Green + Right + std::string("/135Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Green + Right + std::string("/180Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Green + Right + std::string("/225Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Green + Right + std::string("/270Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Green + Right + std::string("/315Deg")+LedPostFix},
        /* Blue*/
        {ValuePreFix + FaceLed +  Blue + Right + std::string("/0Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Right + std::string("/45Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Right + std::string("/90Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Right + std::string("/135Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Right + std::string("/180Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Right + std::string("/225Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Right + std::string("/270Deg")+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Right + std::string("/315Deg")+LedPostFix},
    },
    };

    static const std::string earL[NUM_LED_ORIENTATIONS][NUM_EAR_LEDS] ={
/* Ear Led Left*/
        {ValuePreFix + EarLed + Left + std::string("/0Deg")+LedPostFix,
         ValuePreFix + EarLed + Left + std::string("/36Deg")+LedPostFix,
         ValuePreFix + EarLed + Left + std::string("/72Deg")+LedPostFix,
         ValuePreFix + EarLed + Left + std::string("/108Deg")+LedPostFix,
         ValuePreFix + EarLed + Left + std::string("/144Deg")+LedPostFix,
         ValuePreFix + EarLed + Left + std::string("/180Deg")+LedPostFix,
         ValuePreFix + EarLed + Left + std::string("/216Deg")+LedPostFix,
         ValuePreFix + EarLed + Left + std::string("/252Deg")+LedPostFix,
         ValuePreFix + EarLed + Left + std::string("/288Deg")+LedPostFix,
         ValuePreFix + EarLed + Left + std::string("/324Deg")+LedPostFix},
/* Ear Led Right*/
        {ValuePreFix + EarLed + Right + std::string("/0Deg")+LedPostFix,
         ValuePreFix + EarLed + Right + std::string("/36Deg")+LedPostFix,
         ValuePreFix + EarLed + Right + std::string("/72Deg")+LedPostFix,
         ValuePreFix + EarLed + Right + std::string("/108Deg")+LedPostFix,
         ValuePreFix + EarLed + Right + std::string("/144Deg")+LedPostFix,
         ValuePreFix + EarLed + Right + std::string("/180Deg")+LedPostFix,
         ValuePreFix + EarLed + Right + std::string("/216Deg")+LedPostFix,
         ValuePreFix + EarLed + Right + std::string("/252Deg")+LedPostFix,
         ValuePreFix + EarLed + Right + std::string("/288Deg")+LedPostFix,
         ValuePreFix + EarLed + Right + std::string("/324Deg")+LedPostFix},
    };

    static const std::string footL[NUM_LED_ORIENTATIONS][NUM_LED_COLORS]={
/* Foot Led Left*/
        {ValuePreFix +LFootLed + Red +LedPostFix,
         ValuePreFix +LFootLed + Green +LedPostFix,
         ValuePreFix +LFootLed + Blue +LedPostFix},
/* Foot Led Right*/
        {ValuePreFix +RFootLed + Red +LedPostFix,
         ValuePreFix +RFootLed + Green +LedPostFix,
         ValuePreFix +RFootLed + Blue +LedPostFix},
    };
    static const std::string chestL[NUM_LED_COLORS]={
/* Chest Led*/
        ValuePreFix +ChestLed + Red +LedPostFix,
        ValuePreFix +ChestLed + Green +LedPostFix,
        ValuePreFix +ChestLed + Blue +LedPostFix
    };

    static const std::string strPtr = footL[RIGHT_LED][0];
    static const std::string * RGB_LED_STRINGS[NUM_UNIQUE_LEDS] ={
        &earL[LEFT_LED][0],
        &earL[RIGHT_LED][0],
        &faceL[LEFT_LED][0][0],
        &faceL[RIGHT_LED][0][0],
        &chestL[0],
        &footL[LEFT_LED][0],
        &footL[RIGHT_LED][0],
    };
};

#endif
