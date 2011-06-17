/**
 * This file contains lots of useful things having to do with sensors, including
 * their names (enum/text) and variance distributions. The variance monitor in
 * Sensors.cpp uses limits here to tell whether or not a particular sensor is
 * operating normally.
 *
 * @author Nathan Merritt
 * @date June 2011
 */

#pragma once
#ifndef SENSOR_CONFIG_H
#define SENSOR_CONFIG_H

#include <string>

// naming convention: SENSOR_SIDEofBODY_POSTION
enum SensorNames {
    FSR_LEFT_F_L = 0,
    FSR_LEFT_F_R,
    FSR_LEFT_B_L,
    FSR_LEFT_B_R,
    FSR_RIGHT_F_L,
    FSR_RIGHT_F_R,
    FSR_RIGHT_B_L,
    FSR_RIGHT_B_R,
    BUMPER_LEFT_L,
    BUMPER_LEFT_R,
    BUMPER_RIGHT_L,
    BUMPER_RIGHT_R,
    ACC_X,
    ACC_Y,
    ACC_Z,
    GYRO_X,
    GYRO_Y,
    ANGLE_X,
    ANGLE_Y,
    SONAR_LEFT,
    SONAR_RIGHT,
    SUPPORT_FOOT,
    SENSOR_COUNT,
};

enum SensorMonitorOrder {
    ACCX = 0,
    ACCY,
    ACCZ,
    GYROX,
    GYROY,
    ANGLEX,
    ANGLEY,
    SONARL,
    SONARR,
    MONITOR_COUNT,
};

const std::string fsrNames[] = {
    "left_f_L", "left_f_r", "left_b_l", "left_b_r",
    "right_f_L", "right_f_r", "right_b_l", "right_b_r"
};

// names of different sensors (for variance monitoring)
const std::string sensorNames[] = {
    "accX", "accY", "accZ", "gyroX", "gyroY", "angleX", "angleY",
    "sonarLeft", "sonarRight"
};

/*
 * Limits for sensor variance - if a specific sensor reports a rolling variance
 * outside these boundaries then that might be an early warning that the sensor
 * is malfunctioning. Data to determine these boundaries was collected from
 * several robots as they ran pBrunswick.
 */

const float GYRO_LOW = 0.0005f;

const float ACCELEROMETER_HIGH_XY = 75.0f;
const float ACCELEROMETER_HIGH_Z = 100.0f; // Z is generally noiser

const float SONAR_HIGH = 2.0f;

const float ANGLE_XY_HIGH = 5.0f;

#endif
