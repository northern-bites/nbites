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

// names of different sensors (for variance monitoring)
const std::string sensorNames[] = {
    "accX", "accY", "accZ", "gyroX", "gyroY", "angleX", "angleY",
    "sonarLeft", "sonarRight"
};



#endif
