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

ADD_DEFINITIONS( -DNO_ZLIB -DNO_CHROME )
ADD_DEFINITIONS( -DUNROLLED_LOOPS_THRESHOLD )
ADD_DEFINITIONS( -DUSE_TIME_PROFILING )
ADD_DEFINITIONS( -Wno-write-strings )

# Default (no release specific) build flags
SET( CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -O2 -Wall -Wconversion" )
# Release build flags
SET( CMAKE_CXX_FLAGS_RELEASE
  "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wall -Wconversion" )
# Debug build flags
SET( CMAKE_CXX_FLAGS_DEBUG
  "${CMAKE_CXX_FLAGS_DEBUG} -g3 -O3 -DNDEBUG -Wall -Wconversion" )



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
OPTION(
    @MAN_IS_REMOTE@
    "Compile as a remote binary, versus a dynamic library (ON/OFF)"
    OFF
    )
# DO NOT add any cache settings or documentation to this variable
#  It is included directly in source files and such things will come
#  along with it
SET(
    MAN_IS_REMOTE ${@MAN_IS_REMOTE@}
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
  DEBUG_BALL_DETECTION
  "Turn on/off debug for frames that have a ball"
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
  USE_SENSORS_IMAGE_LOCKING
  "Customize image locking configuration.  Man uses locking."
  ON
  )
OPTION(
  REDIRECT_C_STDERR
  "Redirect the standard error to standard out in C++"
  ON
  )
  

