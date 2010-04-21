/* Contains the Most basic common information/types/defs */

#ifndef SensorDef_h_DEFINED
#define SensorDef_h_DEFINED

#include "Common.h"

// Sensor locators for the sensor array returned from the robot. 
const int NUM_ERS7_SENSORS = 16;
const int NUM_ERS220_SENSORS = 16;
// 8 FSR, 4 foot bumpers, 7 Inertial, 2 Sonar, 1 - which support foot
const int NUM_NAO_SENSORS = 22;
const int NUM_LED_LIGHTS = 20;
const int NUM_SENSOR_IDS = 34;

const int NUM_SENSORS = NUM_NAO_SENSORS;
const int NUM_INERTIAL_SENSORS = 7;

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

#endif // Sensors_h_DEFINED

