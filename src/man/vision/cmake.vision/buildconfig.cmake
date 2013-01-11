

#######################################
##  Build configurations for Vision. ##
#######################################


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
  USE_TIME_PROFILING
  "Turn on/off profiling function calls"
  OFF
  )
# Options pertaining to running the vision code OFFLINE
OPTION( OFFLINE
  "Debug flag for vision when we are running offline"
  OFF
  )

OPTION( USE_MMX
  "Enable the MMX vision code."
  ON
  )
