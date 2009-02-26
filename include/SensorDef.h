/* Contains the Most basic common information/types/defs */

#ifndef SensorDef_h_DEFINED
#define SensorDef_h_DEFINED

#include "Common.h"

// Sensor locators for the sensor array returned from the robot. 
const int NUM_ERS7_SENSORS = 16;
const int NUM_ERS220_SENSORS = 16;
const int NUM_NAO_SENSORS = 21; // 8 FSR, 4 foot bumpers, 7 Inertial, 2 Sonar
const int NUM_LED_LIGHTS = 20;
const int NUM_SENSOR_IDS = 34;

#if ROBOT(AIBO)
const int NUM_SENSORS = NUM_ERS7_SENSORS; // since both ers7 and ers220 have
                                          // the same number of sensors
#elif ROBOT(NAO)
const int NUM_SENSORS = NUM_NAO_SENSORS;
const int NUM_INERTIAL_SENSORS = 7;
#else
#  error "Undefined robot type"
#endif

static const int NUM_PY_SENSORS = 19;

#define ACCELOMETER_Y 0
#define ACCELOMETER_X 1
#define ACCELOMETER_Z 2
#define BODY_PSD 3
#define BACK_REAR_SENSOR 4
#define BACK_MIDDLE_SENSOR 5
#define BACK_FRONT_SENSOR 6
#define WLAN_SWITCH 7
#define HEAD_SENSOR 8
#define CHIN_SENSOR 9
#define HEAD_PSD_NEAR 10
#define HEAD_PSD_FAR 11
#define LF_PAW 12
#define LR_PAW 13
#define RF_PAW 14
#define RR_PAW 15

// camera sensor locator
static const char* const IMAGE_SENSOR_LOCATOR = "PRM:/r1/c1/c2/c3/i1-FbkImageSensor:F1";

static const char* const ERS7_SENSOR_LOCATOR[] = {
    // BODY
    "PRM:/a1-Sensor:a1",                // ACCELEROMETER Y
    "PRM:/a2-Sensor:a2",                // ACCELEROMETER X
    "PRM:/a3-Sensor:a3",                // ACCELEROMETER Z
    "PRM:/p1-Sensor:p1",                // BODY PSD
    "PRM:/t2-Sensor:t2",                // BACK SENSOR (REAR)
    "PRM:/t3-Sensor:t3",                // BACK SENSOR (MIDDLE)
    "PRM:/t4-Sensor:t4",                // BACK SENSOR (FRONT)
    "PRM:/b1-Sensor:b1",                // WIRELESS LAN SWITCH 

    // HEAD
    "PRM:/r1/c1/c2/c3/t1-Sensor:t1",    // HEAD SENSOR
    "PRM:/r1/c1/c2/c3/c4/s5-Sensor:s5", // CHIN SENSOR
    "PRM:/r1/c1/c2/c3/p1-Sensor:p1",    // HEAD PSD (NEAR)
    "PRM:/r1/c1/c2/c3/p2-Sensor:p2",    // HEAD PSD (FAR)

    // PAWS
    "PRM:/r2/c1/c2/c3/c4-Sensor:24",    // LEFT-FRONT PAW SENSOR
    "PRM:/r3/c1/c2/c3/c4-Sensor:34",    // LEFT-REAR PAW SENSOR
    "PRM:/r4/c1/c2/c3/c4-Sensor:44",    // RIGHT-FRONT PAW SENSOR
    "PRM:/r5/c1/c2/c3/c4-Sensor:54"    // RIGHT-REAR PAW SENSOR
};

static const char* const ERS220_SENSOR_LOCATOR[] = {
  "PRM:/r1/c1/c2/c3/f1-Sensor:f1",      // Head sensor 1 
  "PRM:/r1/c1/c2/c3/f2-Sensor:f2",     // Head sensor 2 
  "PRM:/r1/c1/c2/c3/p1-Sensor:p1",      // Distance Sensor
  "PRM:/r1/c1/c2/c3/m1-Mic:M1",         // Microphone
  "PRM:/r1/c1/c2/c3/c4/s5-Sensor:s5",   // Face sensor 
  "PRM:/r2/c1/c2/c3/c4-Sensor:s4",      // Paw sensor(Left fore leg) 
  "PRM:/r3/c1/c2/c3/c4-Sensor:s4",      // Paw sensor(Left hind leg) 
  "PRM:/r4/c1/c2/c3/c4-Sensor:s4",      // Paw sensor(Right fore leg) 
  "PRM:/r5/c1/c2/c3/c4-Sensor:s4",      // Paw sensor(Right hind leg) 
  "PRM:/r6/s1-Sensor:s1",               // Back sensor 
  "PRM:/r6/s2-Sensor:s2",   //Tail sensor (Left from behind) 
  "PRM:/r6/s3-Sensor:s3",   //Tail sensor (Center from behind) 
  "PRM:/r6/s4-Sensor:s4",   //Tail sensor (Right from behind) 
  "PRM:/a1-Sensor:a1",       // y-axis (Front-back direction (Front positive)) 
  "PRM:/a2-Sensor:a2",       // x-axis (Right-left direction (Right positive)) 
  "PRM:/a3-Sensor:a3"       // z-axis (Up-down direction (Up positive)) 
};

#endif // Sensors_h_DEFINED

