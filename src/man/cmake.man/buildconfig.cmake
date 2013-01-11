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

#ADD_DEFINITIONS( -DUNROLLED_LOOPS_THRESHOLD )
#ADD_DEFINITIONS( -DUSE_TIME_PROFILING )

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
OPTION( USING_LAB_FIELD
  "Set field constants to use the lab field constants"
  ON
  )

IF( BUILDING_FOR_A_REMOTE_NAO )
  OPTION( OFFLINE
    "turn offline vision debugging on for compatibility with tool"
    OFF
    )
ELSE()
  OPTION( OFFLINE
    "turn offline vision debugging on for compatibility with tool"
    ON
    )
ENDIF()

IF( BUILDING_FOR_A_REMOTE_NAO )
  OPTION( USE_ALSPEECH
    "turn Aldebaran speech on for debugging"
    OFF
    )
ELSE()
  OPTION( USE_ALSPEECH
    "turn Aldebaran speech on for debugging"
    OFF
    )
ENDIF()
OPTION(
  USE_MOTION
  "Turn on/off all motion actions"
  ON
  )
OPTION(
  USE_MEMORY
  "Use memory and logging"
  OFF
  )