# Configuration Options for Debugging
# Definitions for the CMake configurable build options.  Defined here, they
# are set at build/configure time.  Corresponding C/C++ MACRO definitions
# should reside in the DebugConfig.in file.  The DebugConfig.h header
# will be auto-generated my cmake and dependant file recompiled after a
# build change.
#
# IF all else fails, just `make clean` and `make cross` or straight, configure
# again, and you should be set.
#

option(
  USING_LAB_FIELD
  "Turn on if we are in the lab and not on a full-size field"
  OFF
)

if( NOT OFFLINE )

option(
  DEBUG_COMM
  "Toggle comm debug mode."
  OFF
)

option(
  DEBUG_BALLTRACK
  "Toggle balltrack debug mode."
  OFF
)

option(
  DEBUG_LOC
  "Toggle localization debug mode."
  OFF
)

option(
  DEBUG_THREADS
  "Toggle threads debug mode."
  OFF
)

option(
  DEBUG_LOGGING
  "Toggle logging debug mode."
  OFF
)

option(
  PROFILE
  "Enable time profiling for the robot"
  OFF
)

option(
  LOG_EVERY_FIFTH
  "Enable to log every fifth frame in order to stay synced with image logging"
  ON
)

option(
  USE_LOGGING
  "Enable logging on the robot."
  OFF
)

if(USE_LOGGING)
  option(
    LOG_SENSORS
    "Log the robot's sensor readings."
    OFF
    )

  option(
    LOG_GUARDIAN
    "Log the robot's guardian state."
    OFF
    )

  option(
    LOG_COMM
    "Log the robot's communication."
    OFF
    )

  option(
    LOG_VISION
    "Log the robot's vision output."
    OFF
    )
  option(
    LOG_IMAGES
    "Log the raw images from the camera."
    OFF
    )
  option(
    LOG_LOCATION
    "Log the robots location belief"
    OFF
    )
  option(
    LOG_LOCALIZATION
    "Log the robots localization particle belief"
    OFF
    )
  option(
    LOG_OBSERVATIONS
    "Log the vision field proto (observations given to loc)"
    OFF
    )
  option(
    LOG_ODOMETRY
    "Log the odometry from motion"
    OFF
    )

else(USE_LOGGING)
  unset(LOG_SENSORS CACHE)
  unset(LOG_GUARDIAN CACHE)
  unset(LOG_COMM CACHE)
  unset(LOG_VISION CACHE)
  unset(LOG_IMAGES CACHE)
  unset(LOG_LOCATION CACHE)
  unset(LOG_LOCALIZATION CACHE)
  unset(LOG_OBSERVATIONS CACHE)
  unset(LOG_ODOMETRY CACHE)
endif(USE_LOGGING)

endif( NOT_OFFLINE )
