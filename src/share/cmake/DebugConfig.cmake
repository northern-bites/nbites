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
  USE_LAB_FIELD
  "Turn on if we are in the lab and not on a full-size field"
  OFF
)

option(
  USE_SPL_COMM
  "Use the SPL Standard Message rather than protobufs for comm"
  OFF
)

option(
  USE_NAOQI_2
  "Installing using the Naoqi 2.1 cross compiler"
  ON
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
  ON
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
  USE_LOGGING
  "Enable logging on the robot."
  OFF
)

include(CMakeDependentOption)

CMAKE_DEPENDENT_OPTION(START_WITH_FILEIO
    "Start with fileio ON"
    OFF
    "USE_LOGGING"
    OFF
)

CMAKE_DEPENDENT_OPTION(START_WITH_THUMBNAIL
    "Start with thumbnails logging"
    OFF
    "USE_LOGGING"
    OFF
)

endif( NOT OFFLINE )
