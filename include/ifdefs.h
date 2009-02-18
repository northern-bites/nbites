
//
//  ifdefs.h
//     All macros defined in this file are/should be compile-time switches, for
//  ease of use and change on the robot (ex. Compiling for different robots, or
//  online versus offline).  As such, all macros are also required to check for
//  redefinition (i.e. via ifndef) so as to allow command-line specified
//  definitions to take priority.
//


// Offline switch
#ifndef OFFLINE
//#  define OFFLINE
#endif

// Cortex 2.0 -- not used
#if defined(OFFLINE) && !defined(CORTEX__2_0)
//#  define CORTEX__2_0
#endif


//-----System Switches (uncomment the define to activate the system)----//
// Turn on/off the entire Vision-processing system
#ifndef USE_VISION
//#  define USE_VISION //js - should be set with cmake now
#endif
// it's baaack. due to battery life, here's an easy ifdef to enable/disable
// Python
//#ifndef USE_PYTHON
//#  define USE_PYTHON 
//#endif
// Use the old, small calibration tables
#ifndef SMALL_TABLES
//#  define SMALL_TABLES
#endif

#ifndef USE_TOOL_CONNECT
//#  define USE_TOOL_CONNECT
#endif

// Switches for debugging purposes


#ifndef USE_TIME_PROFILING
//#  define USE_TIME_PROFILING
//#  define USE_PROFILER_AUTO_PRINT
#endif
#ifndef FRAME_BY_FRAME_PROFILING
//#  define FRAME_BY_FRAME_PROFILING
#endif
#ifndef USE_JPEG
//#  define USE_JPEG
#endif
#ifndef USE_AES
//#  define USE_AES
#endif
#ifndef NEW_LOGGING
//#  define NEW_LOGGING
#endif

// Other switches

#ifndef USE_FAST_WALK
#  define USE_FAST_WALK
#endif
