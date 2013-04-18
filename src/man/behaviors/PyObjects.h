/*
 * Wrapper for CombinationObjects.*. Exposes Location, RobotLocation,
 * RelLocation, LocObject, FieldObject, and MyInfo to python. These objects
 * provide direct access to C++ values from python and combine loc and vision
 * information.
 */

#pragma once

#include "CombinationObjects.h"

// Called in BehaviorsModule.cpp to initialize the python module
void c_init_objects();

