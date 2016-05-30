/*
 List of control flags.
 
 Including this file with an appropriate XTEMP definition can be used
 to generate various c/c++ tables.
 */

#ifndef XTEMP
#error "XTEMP must be defined for flags.defs.h"
#endif

//Flags that shouldn't be set outside man – or really outside logging
XTEMP(_START_INTERNAL_),
XTEMP(serv_connected),
XTEMP(control_connected),
XTEMP(_END_INTERNAL_),

//Flags that can be set anywhere
XTEMP(_START_EXTERNAL_),
XTEMP(fileio),

XTEMP(SENSORS),
XTEMP(GUARDIAN),
XTEMP(COMM),
XTEMP(LOCATION),
XTEMP(ODOMETRY),
XTEMP(OBSERVATIONS),
XTEMP(LOCALIZATION),
XTEMP(BALLTRACK),
XTEMP(VISION),

XTEMP(tripoint),
XTEMP(multiball),
XTEMP(thumbnail),

XTEMP(_END_EXTERNAL_),

//Num_flags must be last!  it has int value ( 'previous' + 1)
//which, if it is last,
//is equivalent to the number of flags previous listed.
XTEMP(num_flags)
