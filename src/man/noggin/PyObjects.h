/*
 * Wrapper for CombinationObjects.*. Exposes Location, RobotLocation,
 * RelLocation, LocObject, FieldObject, and MyInfo to python. These objects
 * provide direct access to C++ values from python and combine loc and vision
 * information. 
 */

#ifndef PyObjects_h_DEFINED
#define PyObjects_h_DEFINED

#include "CombinationObjects.h"

// Called in Noggin.cpp to initialize the python module
void c_init_objects();


#endif //PyObjects_h_DEFINED
