/*
 List of control flags.

 Including this file with an appropriate FLAG_XM definition can be used
 to generate various c/c++ tables.
 */

#ifndef FLAG_XM
#error "FLAG_XM must be defined for flags.defs.h"
#endif

//Flags that shouldn't be set outside man – or really outside logging
FLAG_XM(_START_INTERNAL_),
FLAG_XM(connected),
FLAG_XM(_END_INTERNAL_),

//Flags that can be set anywhere
FLAG_XM(_START_EXTERNAL_),
FLAG_XM(tripoint),
FLAG_XM(tripoint_bottom_only),

FLAG_XM(locswarm),

FLAG_XM(state_playing_override),
FLAG_XM(state_penalty_override),

FLAG_XM(SENSORS),
FLAG_XM(GUARDIAN),
FLAG_XM(COMM),
FLAG_XM(LOCATION),
FLAG_XM(ODOMETRY),
FLAG_XM(OBSERVATIONS),
FLAG_XM(LOCALIZATION),
FLAG_XM(BALLTRACK),
FLAG_XM(VISION),

FLAG_XM(multiball),
FLAG_XM(thumbnail),

FLAG_XM(logToFilesystem),
FLAG_XM(logToStream),

FLAG_XM(_END_EXTERNAL_),

//Num_flags must be last!  In the enum, it has int value ( 'previous' + 1)
//which, if it is last,
//is equivalent to the number of flags previous listed.
FLAG_XM(num_flags)
