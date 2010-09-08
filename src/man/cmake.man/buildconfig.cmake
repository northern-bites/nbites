##############################
#
# WELCOME NBITER
#
#   That's enough caps.  Build configurations for Man.  Manually-set compile
#   options and flags, along with the build option definitions (use the
#   ccmake GUI to flip those on/off).
#
#   Enjoy your configuring.
#
###


# .:: General configurations for the Northern Bites Man cmake package :::::

IF( NOT DEFINED ROBOT_TYPE )
  SET( ROBOT_TYPE NAO_RL )
ENDIF( NOT DEFINED ROBOT_TYPE )

ADD_DEFINITIONS( -DNO_ZLIB )
#ADD_DEFINITIONS( -DUNROLLED_LOOPS_THRESHOLD )
#ADD_DEFINITIONS( -DUSE_TIME_PROFILING )
ADD_DEFINITIONS( -Wno-write-strings )


########SETTING UP THE COMPILER FLAGS ##########
# Notes: -JS, GS Feb. 2009
# Note: gcc 4.2 doesnt have a geode processor type.
#       k6-2 has a similar instruction set, so we use it instead
#       this is important for allowing linkage and running of -O1,2,3 bins/libs
#
# Note: The default flags never get set by cmake.
# Note: We override the default CMAKE release and debug flags with our own
# Note: We set the C flags to be the same as the CXX flags

# Default (no release specific) build flags
SET( CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -O2 -m32 -Wall -Wconversion -Wno-unused -Wno-strict-aliasing" )
# Release build flags
SET( CMAKE_CXX_FLAGS_RELEASE
  "-O3 -DNDEBUG -Wall -Wconversion -Wno-unused -Wno-strict-aliasing")
SET( CMAKE_C_FLAGS_RELEASE
  "${CMAKE_CXX_FLAGS_RELEASE}" )
# Debug build flags
SET( CMAKE_CXX_FLAGS_DEBUG
  " -g3 -Wall -Wconversion -Wno-unused -Wno-strict-aliasing" )


############################ Configure Options
# Definitions for the CMake configurable build options.  Defined here, they
# are set at build/configure time.  Corresponding C/C++ MACRO definitions
# should reside in the [module]config.in files.  The [module]config.h headers
# will be auto-generated my cmake and dependant file recompiled after a
# build change.  Some re-configurat bugs may still need to be worked out.
#
# IF all else fails, just `make clean` and `make cross` or straight, configure
# again, and you should be set.
#

# See documentation strings for descriptions
IF( WEBOTS_BACKEND )
  OPTION( USING_LAB_FIELD
    "Set field constants to use the lab field constants"
    OFF
    )
ELSE( WEBOTS_BACKEND )
  OPTION( USING_LAB_FIELD
    "Set field constants to use the lab field constants"
    ON
    )
ENDIF( WEBOTS_BACKEND )

IF( OE_CROSS_BUILD )
  OPTION( OFFLINE
    "turn offline vision debugging on for compatability with tool"
    OFF
    )
ELSE( OE_CROSS_BUILD )
  OPTION( OFFLINE
    "turn offline vision debugging on for compatability with tool"
    ON
    )
ENDIF( OE_CROSS_BUILD )

OPTION(
    MAN_IS_REMOTE_
    "Compile as a remote binary, versus a dynamic library (ON/OFF)"
    OFF
    )
# DO NOT add any cache settings or documentation to this variable
#  It is included directly in source files and such things will come
#  along with it
SET(
    MAN_IS_REMOTE ${MAN_IS_REMOTE_}
    )
OPTION(
  DEBUG_MAN_INITIALIZATION
  "Turn on/off debug printing while initializing the Man class"
  ON
  )
OPTION(
  DEBUG_MAN_THREADING
  "Turn on/off debug printing while starting threads in the Man class"
  ON
  )
OPTION(
  DEBUG_IMAGE_REQUESTS
  "Turn on/off debug printing on requesting images"
  OFF
  )

OPTION(
  USE_VISION
  "Turn on/off all vision processing"
  ON
  )
OPTION(
  USE_NOGGIN
  "Turn on/off python behaviors"
  ON
  )
OPTION(
  USE_MOTION
  "Turn on/off all motion actions"
  ON
  )
OPTION(
  USE_DCM
  "Send commands directly to the DCM. Turn this off in REMOTE mode"
  ON
  )
OPTION(
  USE_SENSORS_IMAGE_LOCKING
  "Customize image locking configuration.  Man uses locking."
  ON
  )
OPTION(
  REDIRECT_C_STDERR
  "Redirect the standard error to standard out in C++"
  ON
  )
