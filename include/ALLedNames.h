#ifndef _ALLedNames_h_DEFINED
#define _ALLedNames_h_DEFINED

#include "ALNames.h"
namespace ALNames {

    static const string FaceLed ="/Face/Led";
    static const string EarLed ="/Ear/Led";
    static const string LFootLed ="/LFoot/Led";
    static const string RFootLed ="/RFoot/Led";
    static const string ChestLed ="/ChestBoard/Led";

    static const string Green ="/Green";
    static const string Red ="/Red";
    static const string Blue ="/Blue";

    static const string Left ="/Left";
    static const string Right ="/Right";

    enum LedColors{
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
    static const unsigned int NUM_LED_COLORS = 3;
    static const unsigned int NUM_LED_ORIENTATIONS = 2;
    static const unsigned int NUM_ONE_EYE_LEDS = NUM_LED_COLORS * NUM_FACE_LEDS;

    static const string faceL[NUM_LED_ORIENTATIONS][NUM_LED_COLORS][NUM_FACE_LEDS] ={
/*  Face Leds Left */
    {
        /* Red*/
        {ValuePreFix + FaceLed +  Red + Left + "/0Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Red + Left + "/35Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Red + Left + "/90Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Red + Left + "/135Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Red + Left + "/180Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Red + Left + "/225Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Red + Left + "/270Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Red + Left + "/315Deg"+LedPostFix},
        /* Green*/
        {ValuePreFix + FaceLed +  Green + Left + "/0Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Green + Left + "/35Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Green + Left + "/90Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Green + Left + "/135Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Green + Left + "/180Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Green + Left + "/225Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Green + Left + "/270Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Green + Left + "/315Deg"+LedPostFix},
        /* Blue*/
        {ValuePreFix + FaceLed +  Blue + Left + "/0Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Left + "/35Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Left + "/90Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Left + "/135Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Left + "/180Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Left + "/225Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Left + "/270Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Left + "/315Deg"+LedPostFix},
    },
/* Face Leds Right */
    {
        /* Red*/
        {ValuePreFix + FaceLed +  Red + Right + "/0Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Red + Right + "/35Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Red + Right + "/90Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Red + Right + "/135Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Red + Right + "/180Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Red + Right + "/225Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Red + Right + "/270Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Red + Right + "/315Deg"+LedPostFix},
        /* Green*/
        {ValuePreFix + FaceLed +  Green + Right + "/0Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Green + Right + "/35Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Green + Right + "/90Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Green + Right + "/135Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Green + Right + "/180Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Green + Right + "/225Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Green + Right + "/270Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Green + Right + "/315Deg"+LedPostFix},
        /* Blue*/
        {ValuePreFix + FaceLed +  Blue + Right + "/0Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Right + "/35Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Right + "/90Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Right + "/135Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Right + "/180Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Right + "/225Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Right + "/270Deg"+LedPostFix,
         ValuePreFix + FaceLed +  Blue + Right + "/315Deg"+LedPostFix},
    },
    };

    static const string earL[NUM_LED_ORIENTATIONS][NUM_EAR_LEDS] ={
/* Ear Led Left*/
        {ValuePreFix + EarLed + Left + "/0Deg"+LedPostFix,
         ValuePreFix + EarLed + Left + "/36Deg"+LedPostFix,
         ValuePreFix + EarLed + Left + "/72Deg"+LedPostFix,
         ValuePreFix + EarLed + Left + "/108Deg"+LedPostFix,
         ValuePreFix + EarLed + Left + "/144Deg"+LedPostFix,
         ValuePreFix + EarLed + Left + "/180Deg"+LedPostFix,
         ValuePreFix + EarLed + Left + "/216Deg"+LedPostFix,
         ValuePreFix + EarLed + Left + "/252Deg"+LedPostFix,
         ValuePreFix + EarLed + Left + "/288Deg"+LedPostFix,
         ValuePreFix + EarLed + Left + "/324Deg"+LedPostFix},
/* Ear Led Right*/
        {ValuePreFix + EarLed + Left + "/0Deg"+LedPostFix,
         ValuePreFix + EarLed + Left + "/36Deg"+LedPostFix,
         ValuePreFix + EarLed + Left + "/72Deg"+LedPostFix,
         ValuePreFix + EarLed + Left + "/108Deg"+LedPostFix,
         ValuePreFix + EarLed + Left + "/144Deg"+LedPostFix,
         ValuePreFix + EarLed + Left + "/180Deg"+LedPostFix,
         ValuePreFix + EarLed + Left + "/216Deg"+LedPostFix,
         ValuePreFix + EarLed + Left + "/252Deg"+LedPostFix,
         ValuePreFix + EarLed + Left + "/288Deg"+LedPostFix,
         ValuePreFix + EarLed + Left + "/324Deg"+LedPostFix},
    };

    static const string footL[NUM_LED_ORIENTATIONS][NUM_LED_COLORS]={
/* Foot Led Left*/
        {ValuePreFix +LFootLed + Red +LedPostFix,
         ValuePreFix +LFootLed + Green +LedPostFix,
         ValuePreFix +LFootLed + Blue +LedPostFix},
/* Foot Led Right*/
        {ValuePreFix +RFootLed + Red +LedPostFix,
         ValuePreFix +RFootLed + Green +LedPostFix,
         ValuePreFix +RFootLed + Blue +LedPostFix},
    };
    static const string chestL[NUM_LED_COLORS]={
/* Chest Led*/
        ValuePreFix +ChestLed + Red +LedPostFix,
        ValuePreFix +ChestLed + Green +LedPostFix,
        ValuePreFix +ChestLed + Blue +LedPostFix
    };

};

#endif
