/**
 * @author Aldebaran-robotics
 * Aldebaran Robotics (c) 2007 All Rights Reserved.\n
 * 
 * This copy of Robocup Software Toolkit is licensed to you under the terms described in the
 * ALDEBARAN_LICENSE file included in this distribution.
 *
 * Version : $Id: aldefinitions.h 3106 2008-01-24 14:42:53Z dgouaillier $
 */

//#ifndef AL_DEFINITIONS_H
//#define AL_DEFINITIONS_H
#ifndef NAODEF_H
#define NAODEF_H

// *** This is a preliminary release for webots. Interfaces may be subject to change. ***

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <string.h>
//#include <device/robot.h>
//#include <device/servo.h>
//#include <device/camera.h>
//#include <device/distance_sensor.h>
//#include <device/accelerometer.h>
//#include <device/touch_sensor.h>
//#include <device/led.h>
//#include <device/emitter.h>
//#include <device/receiver.h>
#include <pthread.h>

// The number of joints in the robot. Note that LHipYawPitch and RHipYawPitch
// are controlled by one motor, so will always have the same value.
#define AL_NUMBER_OF_JOINTS 22
#define NUM_ACTUATORS AL_NUMBER_OF_JOINTS
// The number of chains in the robot. Note that the 6th chain is a fictional
// chain comprised of all the other chains. Please avoid using this. Use
// the 'body' related functions instead.
#define AL_NUMBER_OF_CHAINS 6

// Number of FSR Force sensitive resistor
#define AL_NUMBER_OF_FSR 8

// milliseconds in one cycle
#define AL_OUTER_CONTROL_LOOP_CYCLE_DURATION 40

// milliseconds in one camera cycle
#define AL_CAMERA_CYCLE_DURATION 80

// The interfaces use a 40 milisecond timestep. In many cases, the underlying
// controllers talk to actuators and sensors at a higher frequency.
#define TIME_STEP 40

/**
 * Value returned by methods to indicate success
 */
const int kNoError = 0;
/**
 * Value returned by methods to indicate the device was not found
 */
const int kNoSuchDevice = -10;
/**
 * Value returned by methods to indicate that the method is not impletemented
 */
const int kNotImplemented = -20;
/**
 * Value returned by methods to indicate that the interpolation could be achieved
 */
const int kInterpolationError = -30;
/**
 * Value returned by methods to indicate that the vector size is unexpected
 */
const int kWrongNumberOfArguments = -40;
/**
 * Value returned by methods to indicate general argument error
 */
const int kArgumentError = -45;
/**
 * Value returned by methods to indicate that the position command exceeded the limits
 */
const int kOutOfBoundsPosition = -50;
/**
 * Value returned by methods to indicate that the resultant velocity exceeded the limits
 */
const int kOutOfBoundsVelocity = -60;

typedef unsigned char uint8;

/**
 * Conversions between degrees and radians
 */
inline float degreesToRadians(float pDegrees){	return pDegrees * 0.0174532925f;};
inline float radiansToDegrees(float pRadians){  return pRadians * 57.29577951f;};

/**
 * @return number of cycles in pTime milliseconds
 */
//int durationToAbsoluteCycleNumber(float pTime);

#endif
