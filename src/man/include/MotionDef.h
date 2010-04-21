/* Contains the Most basic common information/types/defs */

#ifndef MotionDef_h_DEFINED
#define MotionDef_h_DEFINED

#include "Common.h"

#if ROBOT(NAO_SIM)
#  include <aldefinitions.h>
#elif ROBOT(NAO_RL)
#  include "NaoDef.h"
#endif

#if ROBOT(AIBO)
// degree/radian conversions
// FROM UNSW 2004 Code ===> Common.h

#define RF_SHOULDER 0
#define RF_HIP 1
#define RF_KNEE 2
#define LF_SHOULDER 3
#define LF_HIP 4
#define LF_KNEE 5
#define RH_SHOULDER 6
#define RH_HIP 7
#define RH_KNEE 8
#define LH_SHOULDER 9
#define LH_HIP 10
#define LH_KNEE 11
#define HEAD_NECK 12
#define HEAD_PAN 13
#define HEAD_YAW 14
#define HEAD_MOUTH 15
#define TAIL_YAW 16
#define TAIL_PAN 17

// WalkLegs Parameters Order:
// Stance Parameters
#define BODY_TILT 0
#define SHOULDER_HEIGHT 1
#define FORE_X0 2
#define FORE_Y0 3
#define HIND_X0 4
#define HIND_Y0 5
   
// Walk Speeds
#define QUARTER_PERIOD 6
#define MAX_DISTANCE 7
#define FORE_LIFT_INITIAL 8
#define FORE_LIFT_FINAL 9
#define HIND_LIFT_INITIAL 10
#define HIND_LIFT_FINAL 11
   
// Walk WorkSpace
#define FORE_XMIN 12 
#define FORE_XMAX 13
#define FORE_YMIN 14
#define FORE_YMAX 15
#define HIND_XMIN 16
#define HIND_XMAX 17
#define HIND_YMIN 18
#define HIND_YMAX 19

// gains
#define ERS7_HIGH_GAINS 0
#define ERS7_LOW_GAINS 1
#define ERS220_GAINS 2

// MOTION STATES (MOTION->VISION state information passing)
#define MOTION_STATE_NUM_LEGS_MOVES 0
#define MOTION_STATE_NUM_HEAD_MOVES 1
#define MOTION_STATE_XWALK 2
#define MOTION_STATE_YWALK 3
#define MOTION_STATE_AWALK 4
#define MOTION_STATE_STATE 5
#define MOTION_STATE_FPS 6
#define MOTION_STATE_NECK 7
#define MOTION_STATE_PAN 8
#define MOTION_STATE_YAW 9
#define MOTION_STATE_MOUTH 10
#define MOTION_STATE_BODY_TILT 11

static const int NUM_MOTION_ENG = 4; // number of motion engine values
static const int NUM_HEAD_ENG = 4; // number of head engine values
static const int NUM_LEGS = 4; // literal number of legs
static const int NUM_HEADS = 1; // literal number of heads
static const int NUM_EARS = 2; // literal number of ears
static const int NUM_ERS7_ACTUATORS = 18; // number of legs+head+tail joints
static const int NUM_ERS7_IMPT_ACTUATORS = 15; // actuators-mouth/tail
static const int NUM_ERS220_ACTUATORS = 15; // number of legs+head+tail joints
static const int NUM_EFFECTORS = 20; // number of all joints
static const int NUM_BODY_JOINTS = 16; // number of legs+head joints
static const int NUM_LEG_JOINTS = 12; // number of leg joints  
static const int NUM_HEAD_JOINTS = 4; // number of head joints
static const int NUM_GAIT_PARAMS = 20; // number of inverse kinematic togglers
static const int NUM_MOTION_STATES = 12; // number of states from motion->vision
static const int NUM_PY_MOTION_STATES = 7; // number of states from motion->py
#endif

#if ROBOT(AIBO_ERS7)
static const int NUM_ACTUATORS = NUM_ERS7_ACTUATORS;
#elif ROBOT(AIBO_220)
static const int NUM_ACTUATORS = NUM_ERS220_ACTUATORS;
#elif ROBOT(NAO_SIM)
static const int NUM_ACTUATORS = AL_NUMBER_OF_JOINTS;
#elif ROBOT(NAO)
#ifndef NUM_ACTUATORS
#define NUM_ACTUATORS 22
#endif
#else
#  error "Undefined robot type"
#endif

#if ROBOT(AIBO)
static const int HEADQ_MAX_SIZE = 2000;
static const int LEGSQ_MAX_SIZE = 2000;

/* Limit values are defined in Model_Information_7_E.pdf 
--tweaked numbers accordingly.
***FROM UNSW 2004 Code ==> JointLimit.h***
*/
static const double MIN_FORE_SHOULDER = -120.0;  
static const double MAX_FORE_SHOULDER = 135.0;

static const double MIN_HIND_SHOULDER = -135.0; 
static const double MAX_HIND_SHOULDER = 120.0;

static const double MIN_HIP = -15.0;
static const double MAX_HIP = 93.0;

static const double MIN_KNEE = -30.0;
static const double MAX_KNEE = 127.0;

static const double MIN_NECK = -80;
static const double MAX_NECK = 3;

static const double MIN_PAN = -93;
static const double MAX_PAN = 93;

static const double MIN_YAW = -20;
static const double MAX_YAW = 50;

static const double MIN_MOUTH = -60;
static const double MAX_MOUTH = 0;

static const double MIN_TAIL_TILT = 0;
static const double MAX_TAIL_TILT = 60;

static const double MIN_TAIL_PAN = -60;
static const double MAX_TAIL_PAN = 60;

static const double LOWER_JOINT_LIMIT[NUM_ERS7_ACTUATORS] = {
    MIN_FORE_SHOULDER, MIN_HIP, MIN_KNEE, 
    MIN_FORE_SHOULDER, MIN_HIP, MIN_KNEE, 
    MIN_HIND_SHOULDER, MIN_HIP, MIN_KNEE, 
    MIN_HIND_SHOULDER, MIN_HIP, MIN_KNEE, 
    MIN_NECK, MIN_PAN, MIN_YAW, MIN_MOUTH,
    MIN_TAIL_TILT, MIN_TAIL_PAN
};

static const double UPPER_JOINT_LIMIT[NUM_ERS7_ACTUATORS] = {
    MAX_FORE_SHOULDER, MAX_HIP, MAX_KNEE, 
    MAX_FORE_SHOULDER, MAX_HIP, MAX_KNEE, 
    MAX_HIND_SHOULDER, MAX_HIP, MAX_KNEE, 
    MAX_HIND_SHOULDER, MAX_HIP, MAX_KNEE,
    MAX_NECK, MAX_PAN, MAX_YAW, MAX_MOUTH,
    MAX_TAIL_TILT, MAX_TAIL_PAN
};

// ERS7 joint primitives
static const char* const ERS7_JOINT_LOCATOR[] = {
    "PRM:/r4/c1-Joint2:41",       // RFLEG SHOULDER (Right Front Leg)
    "PRM:/r4/c1/c2-Joint2:42",    // RFLEG HIP
    "PRM:/r4/c1/c2/c3-Joint2:43", // RFLEG KNEE

    "PRM:/r2/c1-Joint2:21",       // LFLEG SHOULDER (Left Front Leg)
    "PRM:/r2/c1/c2-Joint2:22",    // LFLEG HIP
    "PRM:/r2/c1/c2/c3-Joint2:23", // LFLEG KNEE

    "PRM:/r5/c1-Joint2:51",       // RHLEG SHOULDER (Right Hind Leg)
    "PRM:/r5/c1/c2-Joint2:52",    // RHLEG HIP
    "PRM:/r5/c1/c2/c3-Joint2:53", // RHLEG KNEE

    "PRM:/r3/c1-Joint2:31",       // LHLEG SHOULDER (Left Hind Leg)
    "PRM:/r3/c1/c2-Joint2:32",    // LHLEG HIP
    "PRM:/r3/c1/c2/c3-Joint2:33",  // LHLEG KNEE

    "PRM:/r1/c1-Joint2:11",      // HEAD-TILT1
    "PRM:/r1/c1/c2-Joint2:12",   // HEAD-PAN
    "PRM:/r1/c1/c2/c3-Joint2:13", // HEAD-TILT2
    "PRM:/r1/c1/c2/c3/c4-Joint2:14",  // MOUTH

    "PRM:/r6/c1-Joint2:61",             // TAIL TILT
    "PRM:/r6/c2-Joint2:62"              // TAIL PAN

    //"PRM:/r1/c1/c2/c3/e5-Joint4:15", // LEFT EAR
    //"PRM:/r1/c1/c2/c3/e6-Joint4:16", // RIGHT EAR
};

// ERS220 joint primitives
static const char* const ERS220_JOINT_LOCATOR[] = {
 
  //Right fore leg 
  "PRM:/r4/c1-Joint2:j1",   //J1 joint 
  "PRM:/r4/c1/c2-Joint2:j2",   //J2 joint 
  "PRM:/r4/c1/c2/c3-Joint2:j3", // J3 joint 

  //Left fore leg 
  "PRM:/r2/c1-Joint2:j1",   //J1 joint 
  "PRM:/r2/c1/c2-Joint2:j2",   //J2 joint 
  "PRM:/r2/c1/c2/c3-Joint2:j3",  //J3 joint 
 
  //Right hind leg 
  "PRM:/r5/c1-Joint2:j1",   //J1 joint 
  "PRM:/r5/c1/c2-Joint2:j2",   //J2 joint 
  "PRM:/r5/c1/c2/c3-Joint2:j3",  //J3 joint 

  //Left hind leg 
  "PRM:/r3/c1-Joint2:j1",   //J1 joint 
  "PRM:/r3/c1/c2-Joint2:j2",   //J2 joint 
  "PRM:/r3/c1/c2/c3-Joint2:j3",  //J3 joint 

  // Head
  "PRM:/r1/c1-Joint2:j1",   //Head tilt 
  "PRM:/r1/c1/c2-Joint2:j2",   //Head pan 
  "PRM:/r1/c1/c2/c3-Joint2:j3"  //Head roll 
};

static const short SERVO_PID_SETTINGS_HIGH[][6] = {

  {0x1C, 0x08, 0x01, 0x0E, 0x02, 0x0F}, // RF Leg Shoulder
  {0x14, 0x04, 0x01, 0x0E, 0x02, 0x0F}, // RF Leg Hip
  {0x1C, 0x08, 0x01, 0x0E, 0x02, 0x0F}, // RF Leg Knee
  {0x1C, 0x08, 0x01, 0x0E, 0x02, 0x0F}, // LF Leg Shoulder
  {0x14, 0x04, 0x01, 0x0E, 0x02, 0x0F}, // LF Leg Hip
  {0x1C, 0x08, 0x01, 0x0E, 0x02, 0x0F}, // LF Leg Knee
  {0x1C, 0x08, 0x01, 0x0E, 0x02, 0x0F}, // RH Leg Shoulder
  {0x14, 0x04, 0x01, 0x0E, 0x02, 0x0F}, // RH Leg Hip
  {0x1C, 0x08, 0x01, 0x0E, 0x02, 0x0F}, // RH Leg Knee
  {0x1C, 0x08, 0x01, 0x0E, 0x02, 0x0F}, // LH Leg Shoulder
  {0x14, 0x04, 0x01, 0x0E, 0x02, 0x0F}, // LH Leg Hip
  {0x1C, 0x08, 0x01, 0x0E, 0x02, 0x0F}, // LH Leg Knee
  {0x0A, 0x04, 0x02, 0x0E, 0x02, 0x0F}, // Neck
  {0x08, 0x02, 0x04, 0x0E, 0x02, 0x0F}, // Pan
  {0x08, 0x04, 0x02, 0x0E, 0x02, 0x0F}, // Yaw
  {0x08, 0x00, 0x04, 0x0E, 0x02, 0x0F}, // Mouth
  {0x0A, 0x04, 0x04, 0x0E, 0x02, 0x0F}, // Tail Pan
  {0x0A, 0x04, 0x04, 0x0E, 0x02, 0x0F}  // Tail Tilt
};

static const short SERVO_PID_SETTINGS_LOW[][6] = {
  {0x10, 0x04, 0x01, 0x0E, 0x02, 0x0F}, // RF Leg Shoulder
  {0x0A, 0x04, 0x01, 0x0E, 0x02, 0x0F}, // RF Leg Hip
  {0x10, 0x04, 0x01, 0x0E, 0x02, 0x0F}, // RF Leg Knee
  {0x10, 0x04, 0x01, 0x0E, 0x02, 0x0F}, // LF Leg Shoulder
  {0x0A, 0x04, 0x01, 0x0E, 0x02, 0x0F}, // LF Leg Hip
  {0x10, 0x08, 0x01, 0x0E, 0x02, 0x0F}, // LF Leg Knee
  {0x10, 0x04, 0x01, 0x0E, 0x02, 0x0F}, // RH Leg Shoulder
  {0x0A, 0x04, 0x01, 0x0E, 0x02, 0x0F}, // RH Leg Hip
  {0x10, 0x04, 0x01, 0x0E, 0x02, 0x0F}, // RH Leg Knee
  {0x10, 0x04, 0x01, 0x0E, 0x02, 0x0F}, // LH Leg Shoulder
  {0x0A, 0x04, 0x01, 0x0E, 0x02, 0x0F}, // LH Leg Hip
  {0x10, 0x04, 0x01, 0x0E, 0x02, 0x0F}, // LH Leg Knee
  {0x0A, 0x04, 0x02, 0x0E, 0x02, 0x0F}, // Neck
  {0x08, 0x02, 0x04, 0x0E, 0x02, 0x0F}, // Pan
  {0x08, 0x04, 0x02, 0x0E, 0x02, 0x0F}, // Yaw
  {0x08, 0x00, 0x04, 0x0E, 0x02, 0x0F}, // Mouth
  {0x0A, 0x04, 0x04, 0x0E, 0x02, 0x0F}, // Tail Pan
  {0x0A, 0x04, 0x04, 0x0E, 0x02, 0x0F}  // Tail Tilt
};

static const short SERVO_PID_SETTINGS_220[][6] = {
  {0x16, 0x04, 0x08, 0x0E, 0x02, 0x0F}, // LF Leg Hip
  {0x14, 0x04, 0x06, 0x0E, 0x02, 0x0F}, // LF Leg Shoulder
  {0x23, 0x04, 0x05, 0x0E, 0x02, 0x0F}, // LF Leg Knee
  {0x16, 0x04, 0x08, 0x0E, 0x02, 0x0F}, // LH Leg Hip
  {0x14, 0x04, 0x06, 0x0E, 0x02, 0x0F}, // LH Leg Shoulder
  {0x23, 0x04, 0x05, 0x0E, 0x02, 0x0F}, // LH Leg Knee
  {0x16, 0x04, 0x08, 0x0E, 0x02, 0x0F}, // RF Leg Hip
  {0x14, 0x04, 0x06, 0x0E, 0x02, 0x0F}, // RF Leg Shoulder
  {0x23, 0x04, 0x05, 0x0E, 0x02, 0x0F}, // RF Leg Knee
  {0x16, 0x04, 0x08, 0x0E, 0x02, 0x0F}, // RH Leg Hip
  {0x14, 0x04, 0x06, 0x0E, 0x02, 0x0F}, // RH Leg Shoulder
  {0x23, 0x04, 0x05, 0x0E, 0x02, 0x0F}, // RH Leg Knee
  {0x0A, 0x08, 0x0C, 0x0E, 0x02, 0x0F}, // Neck tilt
  {0x0D, 0x08, 0x0B, 0x0E, 0x02, 0x0F}, // Neck Pan
  {0x0A, 0x08, 0x0C, 0x0E, 0x02, 0x0F}, // Neck roll
};

#endif

#endif // MotionDef_h_DEFINED

