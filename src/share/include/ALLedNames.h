#ifndef _ALLedNames_h_DEFINED
#define _ALLedNames_h_DEFINED

namespace ALNames {

static const std::string BrainLed = "Head/Led";
static const std::string FaceLed ="Face/Led";
static const std::string EarLed ="Ears/Led";
static const std::string LFootLed ="LFoot/Led";
static const std::string RFootLed ="RFoot/Led";
static const std::string ChestLed ="ChestBoard/Led";

static const std::string Green ="/Green";
static const std::string Red ="/Red";
static const std::string Blue ="/Blue";

static const std::string Left ="/Left";
static const std::string Right ="/Right";

static const std::string PositionPostFix = "/Position/Actuator/Value";
static const std::string HardnessPostFix = "/Hardness/Actuator/Value";
static const std::string LedPostFix      = "/Actuator/Value";
static const std::string ValuePostFix    = "/Position/Sensor/Value";
static const std::string ValuePreFix     = "Device/SubDeviceList/";
static const std::string TempValuePostFix= "/Temperature/Sensor/Value";

enum LedColor{
    RED_LED = 0,
    GREEN_LED,
    BLUE_LED
};
enum LedOrientation{
    LEFT_LED = 0,
    RIGHT_LED
};

// NUM_FACE_LEDS refers to how many LEDS there are per eye
static const unsigned int NUM_BRAIN_LEDS = 12;
static const unsigned int NUM_FACE_LEDS  = 8;
static const unsigned int NUM_EAR_LEDS   = 10;
static const unsigned int NUM_FOOT_LEDS  = 1;
static const unsigned int NUM_CHEST_LEDS = 1;

/**
 * The following were added by Wils Dawson on 6/7/12.
 * The ears are broken up into comm and calibration halves where
 * calibration is toward the front of the robot.
 * The eyes are broken up into three parts (top broken in half
 * and bottom is the third segment).
 *   For the right eye, the bottom shows my goal or opp goal,
 *   the top right shows the right goal, the top left shows the left goal
 *   For the left eye, the bottom shows the ball,
 *   the top shows the role.
 */
static const unsigned int NUM_COMM_LEDS = 1;
static const unsigned int NUM_CALIBRATION_LEDS  = 1;
static const unsigned int NUM_BALL_LEDS = 5;
static const unsigned int NUM_GOALBOX_LEDS = NUM_FACE_LEDS - NUM_BALL_LEDS;
static const unsigned int NUM_ROLE_LEDS = NUM_FACE_LEDS;

static const unsigned int NUM_LED_COLORS = 3;
static const unsigned int NUM_LED_ORIENTATIONS = 2;
static const unsigned int NUM_ONE_EYE_LEDS = NUM_LED_COLORS * NUM_FACE_LEDS;

static const unsigned int NUM_UNIQUE_LEDS = 51;
static const unsigned int NUM_RGB_LEDS[NUM_UNIQUE_LEDS] ={
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1};

    // NUM_CALIBRATION_LEDS,NUM_CALIBRATION_LEDS,NUM_CALIBRATION_LEDS,NUM_CALIBRATION_LEDS,NUM_CALIBRATION_LEDS,
    // NUM_CALIBRATION_LEDS,NUM_CALIBRATION_LEDS,NUM_CALIBRATION_LEDS,NUM_CALIBRATION_LEDS,NUM_CALIBRATION_LEDS,
    // NUM_COMM_LEDS,NUM_COMM_LEDS,NUM_COMM_LEDS,NUM_COMM_LEDS,NUM_COMM_LEDS,
    // NUM_COMM_LEDS,NUM_COMM_LEDS,NUM_COMM_LEDS,NUM_COMM_LEDS,NUM_COMM_LEDS,
    // NUM_ROLE_LEDS,                                 // Left Eye
    // NUM_BALL_LEDS, NUM_GOALBOX_LEDS,               // Right Eye
    // NUM_CHEST_LEDS,
    // NUM_FOOT_LEDS,NUM_FOOT_LEDS,
    // 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

static const unsigned int LED_START_COLOR[NUM_UNIQUE_LEDS] ={
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
    0, 0, 0};
    // BLUE_LED,BLUE_LED,BLUE_LED,BLUE_LED,BLUE_LED,  // Ear fronts
    // BLUE_LED,BLUE_LED,BLUE_LED,BLUE_LED,BLUE_LED,
    // BLUE_LED,BLUE_LED,BLUE_LED,BLUE_LED,BLUE_LED,  // Ear backs
    // BLUE_LED,BLUE_LED,BLUE_LED,BLUE_LED,BLUE_LED,
    // RED_LED,                                       // Left Eye
    // RED_LED, RED_LED,                              // Right Eye
    // RED_LED,                                       // Chest
    // RED_LED,RED_LED,
    // 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2};                              // Feet

static const unsigned int LED_END_COLOR[NUM_UNIQUE_LEDS] ={
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 
    3, 3, 3, 3, 3, 3, 3, 3, 
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
    3, 3, 3};
    // NUM_LED_COLORS,NUM_LED_COLORS,NUM_LED_COLORS,NUM_LED_COLORS,NUM_LED_COLORS,
    // NUM_LED_COLORS,NUM_LED_COLORS,NUM_LED_COLORS,NUM_LED_COLORS,NUM_LED_COLORS,
    // NUM_LED_COLORS,NUM_LED_COLORS,NUM_LED_COLORS,NUM_LED_COLORS,NUM_LED_COLORS,
    // NUM_LED_COLORS,NUM_LED_COLORS,NUM_LED_COLORS,NUM_LED_COLORS,NUM_LED_COLORS,
    // NUM_LED_COLORS,                // Left Eye
    // NUM_LED_COLORS, NUM_LED_COLORS, // Right Eye
    // NUM_LED_COLORS,                // Chest
    // NUM_LED_COLORS,NUM_LED_COLORS,
    // 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3}; // Feet

static const std::string brain[NUM_BRAIN_LEDS] = {
    ValuePreFix + BrainLed + std::string("/Front/Right/1") + LedPostFix,
    ValuePreFix + BrainLed + std::string("/Front/Right/0") + LedPostFix,
    ValuePreFix + BrainLed + std::string("/Middle/Right/0") + LedPostFix,
    ValuePreFix + BrainLed + std::string("/Rear/Right/0") + LedPostFix,
    ValuePreFix + BrainLed + std::string("/Rear/Right/1") + LedPostFix,
    ValuePreFix + BrainLed + std::string("/Rear/Right/2") + LedPostFix,
    ValuePreFix + BrainLed + std::string("/Rear/Left/2") + LedPostFix,
    ValuePreFix + BrainLed + std::string("/Rear/Left/1") + LedPostFix,
    ValuePreFix + BrainLed + std::string("/Rear/Left/0") + LedPostFix,
    ValuePreFix + BrainLed + std::string("/Middle/Left/0") + LedPostFix,
    ValuePreFix + BrainLed + std::string("/Front/Left/0") + LedPostFix,
    ValuePreFix + BrainLed + std::string("/Front/Left/1") + LedPostFix,
};

static const std::string eye[NUM_LED_ORIENTATIONS][NUM_FACE_LEDS][NUM_LED_COLORS] = {
    /* Eye LEDs Right */
    {
        {
            ValuePreFix + FaceLed + Red + Right + std::string("/0Deg")+LedPostFix,
            ValuePreFix + FaceLed + Green + Right + std::string("/0Deg")+LedPostFix,
            ValuePreFix + FaceLed + Blue + Right + std::string("/0Deg")+LedPostFix,
        },
        {
            ValuePreFix + FaceLed + Red + Right + std::string("/45Deg")+LedPostFix,
            ValuePreFix + FaceLed + Green + Right + std::string("/45Deg")+LedPostFix,
            ValuePreFix + FaceLed + Blue + Right + std::string("/45Deg")+LedPostFix,
        },
        {
            ValuePreFix + FaceLed + Red + Right + std::string("/90Deg")+LedPostFix,
            ValuePreFix + FaceLed + Green + Right + std::string("/90Deg")+LedPostFix,
            ValuePreFix + FaceLed + Blue + Right + std::string("/90Deg")+LedPostFix,
        },
        {
            ValuePreFix + FaceLed + Red + Right + std::string("/135Deg")+LedPostFix,
            ValuePreFix + FaceLed + Green + Right + std::string("/135Deg")+LedPostFix,
            ValuePreFix + FaceLed + Blue + Right + std::string("/135Deg")+LedPostFix,
        },
        {
            ValuePreFix + FaceLed + Red + Right + std::string("/180Deg")+LedPostFix,
            ValuePreFix + FaceLed + Green + Right + std::string("/180Deg")+LedPostFix,
            ValuePreFix + FaceLed + Blue + Right + std::string("/180Deg")+LedPostFix,
        },
        {
            ValuePreFix + FaceLed + Red + Right + std::string("/225Deg")+LedPostFix,
            ValuePreFix + FaceLed + Green + Right + std::string("/225Deg")+LedPostFix,
            ValuePreFix + FaceLed + Blue + Right + std::string("/225Deg")+LedPostFix,
        },
        {
            ValuePreFix + FaceLed + Red + Right + std::string("/270Deg")+LedPostFix,
            ValuePreFix + FaceLed + Green + Right + std::string("/270Deg")+LedPostFix,
            ValuePreFix + FaceLed + Blue + Right + std::string("/270Deg")+LedPostFix,
        },
        {
            ValuePreFix + FaceLed + Red + Right + std::string("/315Deg")+LedPostFix,
            ValuePreFix + FaceLed + Green + Right + std::string("/315Deg")+LedPostFix,
            ValuePreFix + FaceLed + Blue + Right + std::string("/315Deg")+LedPostFix,
        }
    },
    
    /* Eye LEDs Left */
    {
        {
            ValuePreFix + FaceLed + Red + Left + std::string("/0Deg")+LedPostFix,
            ValuePreFix + FaceLed + Green + Left + std::string("/0Deg")+LedPostFix,
            ValuePreFix + FaceLed + Blue + Left + std::string("/0Deg")+LedPostFix,
        },
        {
            ValuePreFix + FaceLed + Red + Left + std::string("/45Deg")+LedPostFix,
            ValuePreFix + FaceLed + Green + Left + std::string("/45Deg")+LedPostFix,
            ValuePreFix + FaceLed + Blue + Left + std::string("/45Deg")+LedPostFix,
        },
        {
            ValuePreFix + FaceLed + Red + Left + std::string("/90Deg")+LedPostFix,
            ValuePreFix + FaceLed + Green + Left + std::string("/90Deg")+LedPostFix,
            ValuePreFix + FaceLed + Blue + Left + std::string("/90Deg")+LedPostFix,
        },
        {
            ValuePreFix + FaceLed + Red + Left + std::string("/135Deg")+LedPostFix,
            ValuePreFix + FaceLed + Green + Left + std::string("/135Deg")+LedPostFix,
            ValuePreFix + FaceLed + Blue + Left + std::string("/135Deg")+LedPostFix,
        },
        {
            ValuePreFix + FaceLed + Red + Left + std::string("/180Deg")+LedPostFix,
            ValuePreFix + FaceLed + Green + Left + std::string("/180Deg")+LedPostFix,
            ValuePreFix + FaceLed + Blue + Left + std::string("/180Deg")+LedPostFix,
        },
        {
            ValuePreFix + FaceLed + Red + Left + std::string("/225Deg")+LedPostFix,
            ValuePreFix + FaceLed + Green + Left + std::string("/225Deg")+LedPostFix,
            ValuePreFix + FaceLed + Blue + Left + std::string("/225Deg")+LedPostFix,
        },
        {
            ValuePreFix + FaceLed + Red + Left + std::string("/270Deg")+LedPostFix,
            ValuePreFix + FaceLed + Green + Left + std::string("/270Deg")+LedPostFix,
            ValuePreFix + FaceLed + Blue + Left + std::string("/270Deg")+LedPostFix,
        },
        {
            ValuePreFix + FaceLed + Red + Left + std::string("/315Deg")+LedPostFix,
            ValuePreFix + FaceLed + Green + Left + std::string("/315Deg")+LedPostFix,
            ValuePreFix + FaceLed + Blue + Left + std::string("/315Deg")+LedPostFix,
        }
    }
};

// static const std::string faceL[NUM_LED_ORIENTATIONS][NUM_LED_COLORS][NUM_FACE_LEDS] ={
// /*  Face Leds Left */
//     {
//         /* Red*/
//         {ValuePreFix + FaceLed + Red + Left + std::string("/0Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Red + Left + std::string("/45Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Red + Left + std::string("/90Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Red + Left + std::string("/135Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Red + Left + std::string("/180Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Red + Left + std::string("/225Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Red + Left + std::string("/270Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Red + Left + std::string("/315Deg")+LedPostFix},
//         /* Green*/
//         {ValuePreFix + FaceLed + Green + Left + std::string("/0Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Green + Left + std::string("/45Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Green + Left + std::string("/90Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Green + Left + std::string("/135Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Green + Left + std::string("/180Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Green + Left + std::string("/225Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Green + Left + std::string("/270Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Green + Left + std::string("/315Deg")+LedPostFix},
//         /* Blue*/
//         {ValuePreFix + FaceLed + Blue + Left + std::string("/0Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Blue + Left + std::string("/45Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Blue + Left + std::string("/90Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Blue + Left + std::string("/135Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Blue + Left + std::string("/180Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Blue + Left + std::string("/225Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Blue + Left + std::string("/270Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Blue + Left + std::string("/315Deg")+LedPostFix},
//     },
// /* Face Leds Right */
//     {
//         /* Red*/
//         {ValuePreFix + FaceLed + Red + Right + std::string("/0Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Red + Right + std::string("/45Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Red + Right + std::string("/90Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Red + Right + std::string("/135Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Red + Right + std::string("/180Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Red + Right + std::string("/225Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Red + Right + std::string("/270Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Red + Right + std::string("/315Deg")+LedPostFix},
//         /* Green*/
//         {ValuePreFix + FaceLed + Green + Right + std::string("/0Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Green + Right + std::string("/45Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Green + Right + std::string("/90Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Green + Right + std::string("/135Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Green + Right + std::string("/180Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Green + Right + std::string("/225Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Green + Right + std::string("/270Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Green + Right + std::string("/315Deg")+LedPostFix},
//         /* Blue*/
//         {ValuePreFix + FaceLed + Blue + Right + std::string("/0Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Blue + Right + std::string("/45Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Blue + Right + std::string("/90Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Blue + Right + std::string("/135Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Blue + Right + std::string("/180Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Blue + Right + std::string("/225Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Blue + Right + std::string("/270Deg")+LedPostFix,
//          ValuePreFix + FaceLed + Blue + Right + std::string("/315Deg")+LedPostFix},
//     },
// };

static const std::string ear[NUM_LED_ORIENTATIONS][NUM_EAR_LEDS] ={
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

static const std::string foot[NUM_LED_ORIENTATIONS][NUM_LED_COLORS]={
/* Foot Led Left*/
    {ValuePreFix +LFootLed + Red +LedPostFix,
     ValuePreFix +LFootLed + Green +LedPostFix,
     ValuePreFix +LFootLed + Blue +LedPostFix},
/* Foot Led Right*/
    {ValuePreFix +RFootLed + Red +LedPostFix,
     ValuePreFix +RFootLed + Green +LedPostFix,
     ValuePreFix +RFootLed + Blue +LedPostFix},
};

static const std::string chest[NUM_LED_COLORS]={
/* Chest Led*/
    ValuePreFix +ChestLed + Red +LedPostFix,
    ValuePreFix +ChestLed + Green +LedPostFix,
    ValuePreFix +ChestLed + Blue +LedPostFix
};


/* NOTE: EYE LEDS ON NaoV4 ARE ROTATED (RIGHT ALSO BACKWARDS) FROM THE SPECS!!
   In other words, the specs aren't right so below is a "hack" until
   the hardware is actually to spec. HACK
*/
// static const std::string ballL[NUM_LED_COLORS][NUM_BALL_LEDS] ={
// /* Red*/
//     {faceL[RIGHT_LED][RED_LED][2],
//      faceL[RIGHT_LED][RED_LED][3],
//      faceL[RIGHT_LED][RED_LED][4],
//      faceL[RIGHT_LED][RED_LED][5],
//      faceL[RIGHT_LED][RED_LED][6]},
// /* Green*/
//     {faceL[RIGHT_LED][GREEN_LED][2],
//      faceL[RIGHT_LED][GREEN_LED][3],
//      faceL[RIGHT_LED][GREEN_LED][4],
//      faceL[RIGHT_LED][GREEN_LED][5],
//      faceL[RIGHT_LED][GREEN_LED][6]},
// /* Blue*/
//     {faceL[RIGHT_LED][BLUE_LED][2],
//      faceL[RIGHT_LED][BLUE_LED][3],
//      faceL[RIGHT_LED][BLUE_LED][4],
//      faceL[RIGHT_LED][BLUE_LED][5],
//      faceL[RIGHT_LED][BLUE_LED][6]}
// };

// static const std::string goalBoxL[NUM_LED_COLORS][NUM_GOALBOX_LEDS] ={
// /* Red*/
//     {faceL[RIGHT_LED][RED_LED][0],
//      faceL[RIGHT_LED][RED_LED][1],
//      faceL[RIGHT_LED][RED_LED][7]},
// /* Green*/
//     {faceL[RIGHT_LED][GREEN_LED][0],
//      faceL[RIGHT_LED][GREEN_LED][1],
//      faceL[RIGHT_LED][GREEN_LED][7]},
// /* Blue*/
//     {faceL[RIGHT_LED][BLUE_LED][0],
//      faceL[RIGHT_LED][BLUE_LED][1],
//      faceL[RIGHT_LED][BLUE_LED][7]}
// };

// static const std::string rightEyeZero[NUM_LED_COLORS] = {
//     faceL[RIGHT_LED][RED_LED][0],
//     faceL[RIGHT_LED][GREEN_LED][0],
//     faceL[RIGHT_LED][BLUE_LED][0]
// };

// static const std::string rightEyeOne[NUM_LED_COLORS] = {
//     faceL[RIGHT_LED][RED_LED][1],
//     faceL[RIGHT_LED][GREEN_LED][1],
//     faceL[RIGHT_LED][BLUE_LED][1]
// };

// static const std::string rightEyeTwo[NUM_LED_COLORS] = {
//     faceL[RIGHT_LED][RED_LED][2],
//     faceL[RIGHT_LED][GREEN_LED][2],
//     faceL[RIGHT_LED][BLUE_LED][2]
// };

// static const std::string rightEyeThree[NUM_LED_COLORS] = {
//     faceL[RIGHT_LED][RED_LED][3],
//     faceL[RIGHT_LED][GREEN_LED][3],
//     faceL[RIGHT_LED][BLUE_LED][3]
// };

// static const std::string rightEyeFour[NUM_LED_COLORS] = {
//     faceL[RIGHT_LED][RED_LED][4],
//     faceL[RIGHT_LED][GREEN_LED][4],
//     faceL[RIGHT_LED][BLUE_LED][4]
// };

// static const std::string rightEyeFive[NUM_LED_COLORS] = {
//     faceL[RIGHT_LED][RED_LED][5],
//     faceL[RIGHT_LED][GREEN_LED][5],
//     faceL[RIGHT_LED][BLUE_LED][5]
// };

// static const std::string rightEyeSix[NUM_LED_COLORS] = {
//     faceL[RIGHT_LED][RED_LED][6],
//     faceL[RIGHT_LED][GREEN_LED][6],
//     faceL[RIGHT_LED][BLUE_LED][6]
// };

// static const std::string rightEyeSeven[NUM_LED_COLORS] = {
//     faceL[RIGHT_LED][RED_LED][7],
//     faceL[RIGHT_LED][GREEN_LED][7],
//     faceL[RIGHT_LED][BLUE_LED][7]
// };

// static const std::string leftEyeZero[NUM_LED_COLORS] = {
//     faceL[LEFT_LED][RED_LED][0],
//     faceL[LEFT_LED][GREEN_LED][0],
//     faceL[LEFT_LED][BLUE_LED][0]
// };

// static const std::string leftEyeOne[NUM_LED_COLORS] = {
//     faceL[LEFT_LED][RED_LED][1],
//     faceL[LEFT_LED][GREEN_LED][1],
//     faceL[LEFT_LED][BLUE_LED][1]
// };

// static const std::string leftEyeTwo[NUM_LED_COLORS] = {
//     faceL[LEFT_LED][RED_LED][2],
//     faceL[LEFT_LED][GREEN_LED][2],
//     faceL[LEFT_LED][BLUE_LED][2]
// };

// static const std::string leftEyeThree[NUM_LED_COLORS] = {
//     faceL[LEFT_LED][RED_LED][3],
//     faceL[LEFT_LED][GREEN_LED][3],
//     faceL[LEFT_LED][BLUE_LED][3]
// };

// static const std::string leftEyeFour[NUM_LED_COLORS] = {
//     faceL[LEFT_LED][RED_LED][4],
//     faceL[LEFT_LED][GREEN_LED][4],
//     faceL[LEFT_LED][BLUE_LED][4]
// };

// static const std::string leftEyeFive[NUM_LED_COLORS] = {
//     faceL[LEFT_LED][RED_LED][5],
//     faceL[LEFT_LED][GREEN_LED][5],
//     faceL[LEFT_LED][BLUE_LED][5]
// };

// static const std::string leftEyeSix[NUM_LED_COLORS] = {
//     faceL[LEFT_LED][RED_LED][6],
//     faceL[LEFT_LED][GREEN_LED][6],
//     faceL[LEFT_LED][BLUE_LED][6]
// };

// static const std::string leftEyeSeven[NUM_LED_COLORS] = {
//     faceL[LEFT_LED][RED_LED][7],
//     faceL[LEFT_LED][GREEN_LED][7],
//     faceL[LEFT_LED][BLUE_LED][7]
// };

// static const std::string roleL[NUM_LED_COLORS][NUM_ROLE_LEDS] ={
// /* Red*/
//     {faceL[LEFT_LED][RED_LED][0],
//      faceL[LEFT_LED][RED_LED][1],
//      faceL[LEFT_LED][RED_LED][2],
//      faceL[LEFT_LED][RED_LED][3],
//      faceL[LEFT_LED][RED_LED][4],
//      faceL[LEFT_LED][RED_LED][5],
//      faceL[LEFT_LED][RED_LED][6],
//      faceL[LEFT_LED][RED_LED][7]},
// /* Green*/
//     {faceL[LEFT_LED][GREEN_LED][0],
//      faceL[LEFT_LED][GREEN_LED][1],
//      faceL[LEFT_LED][GREEN_LED][2],
//      faceL[LEFT_LED][GREEN_LED][3],
//      faceL[LEFT_LED][GREEN_LED][4],
//      faceL[LEFT_LED][GREEN_LED][5],
//      faceL[LEFT_LED][GREEN_LED][6],
//      faceL[LEFT_LED][GREEN_LED][7]},
// /* Blue*/
//     {faceL[LEFT_LED][BLUE_LED][0],
//      faceL[LEFT_LED][BLUE_LED][1],
//      faceL[LEFT_LED][BLUE_LED][2],
//      faceL[LEFT_LED][BLUE_LED][3],
//      faceL[LEFT_LED][BLUE_LED][4],
//      faceL[LEFT_LED][BLUE_LED][5],
//      faceL[LEFT_LED][BLUE_LED][6],
//      faceL[LEFT_LED][BLUE_LED][7]},
// };


// static const std::string strPtr = footL[RIGHT_LED][0];
static const std::string * RGB_LED_STRINGS[NUM_UNIQUE_LEDS] ={
    &brain[0], //0
    &brain[1], //1
    &brain[2], //2
    &brain[3], 
    &brain[4],
    &brain[5],
    &brain[6],
    &brain[7],
    &brain[8],
    &brain[9],
    &brain[10],
    &brain[11],
    
    &eye[RIGHT_LED][0][0], //12
    &eye[RIGHT_LED][1][0], //13
    &eye[RIGHT_LED][2][0], //14
    &eye[RIGHT_LED][3][0], //15
    &eye[RIGHT_LED][4][0], //16
    &eye[RIGHT_LED][5][0], //17
    &eye[RIGHT_LED][6][0], //18
    &eye[RIGHT_LED][7][0], //19

    &eye[LEFT_LED][0][0], //0x14
    &eye[LEFT_LED][1][0],
    &eye[LEFT_LED][2][0],
    &eye[LEFT_LED][3][0],
    &eye[LEFT_LED][4][0],
    &eye[LEFT_LED][5][0],
    &eye[LEFT_LED][6][0],
    &eye[LEFT_LED][7][0],

    &ear[RIGHT_LED][0],
    &ear[RIGHT_LED][1],
    &ear[RIGHT_LED][2],
    &ear[RIGHT_LED][3],
    &ear[RIGHT_LED][4],
    &ear[RIGHT_LED][5],
    &ear[RIGHT_LED][6],
    &ear[RIGHT_LED][7],
    &ear[RIGHT_LED][8],
    &ear[RIGHT_LED][9],

    &ear[LEFT_LED][0],
    &ear[LEFT_LED][1],
    &ear[LEFT_LED][2],
    &ear[LEFT_LED][3],
    &ear[LEFT_LED][4],
    &ear[LEFT_LED][5],
    &ear[LEFT_LED][6],
    &ear[LEFT_LED][7],
    &ear[LEFT_LED][8],
    &ear[LEFT_LED][9],

    &chest[0],
    &foot[RIGHT_LED][0],
    &foot[LEFT_LED][0]
};

//     &earL[LEFT_LED][0],             // Left Calibration 1
//     &earL[LEFT_LED][1],             // 2
//     &earL[LEFT_LED][2],             // 3
//     &earL[LEFT_LED][3],             // 4
//     &earL[LEFT_LED][4],             // 5
//     &earL[RIGHT_LED][1],            // Right Calibration 1
//     &earL[RIGHT_LED][2],            // 2
//     &earL[RIGHT_LED][3],            // 3
//     &earL[RIGHT_LED][4],            // 4
//     &earL[RIGHT_LED][5],            // 5
//     &earL[LEFT_LED][5],             // Left Comm 1
//     &earL[LEFT_LED][6],             // 2
//     &earL[LEFT_LED][7],             // 3
//     &earL[LEFT_LED][8],             // 4
//     &earL[LEFT_LED][9],             // 5
//     &earL[RIGHT_LED][6],            // Right Comm 1
//     &earL[RIGHT_LED][7],            // 2
//     &earL[RIGHT_LED][8],            // 3
//     &earL[RIGHT_LED][9],            // 4
//     &earL[RIGHT_LED][0],            // 5
//     &roleL[0][0],                   // Role
//     &ballL[0][0],                   // Ball
//     &goalBoxL[0][0],                // Goalbox
//     &chestL[0], //23
//     &footL[LEFT_LED][0],
//     &footL[RIGHT_LED][0],
//     &rightEyeZero[0], //26
//     &rightEyeOne[0], //27
//     &rightEyeTwo[0],
//     &rightEyeThree[0],
//     &rightEyeFour[0],
//     &rightEyeFive[0],
//     &rightEyeSix[0],
//     &rightEyeSeven[0],
//     &brainL[8],
//     &brainL[9],
//     &brainL[10],
//     &brainL[1],
// };
};

/*
brain 0
brain 1
brain 2
brain 3
brain 4
brain 5
brain 6
brain 7
brain 8
brain 9
brain 10
brain 11

r eye 0
r eye 1
r eye 2
r eye 3
r eye 4
r eye 5
r eye 6
r eye 7

l eye 0
l eye 1
l eye 2
l eye 3
l eye 4
l eye 5
l eye 6
l eye 7

r ear 0
r ear 1
r ear 2
r ear 3
r ear 4
r ear 5
r ear 6
r ear 7
r ear 8
r ear 9

l ear 0
l ear 1
l ear 2
l ear 3
l ear 4
l ear 5
l ear 6
l ear 7
l ear 8
l ear 9

chest
r foot
l foot
*/

#endif
