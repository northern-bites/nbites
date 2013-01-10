

#######################################
##  Build configurations for Motoin. ##
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
  DEBUG_MOTION
  "Turn on/off a variety of motion-specific debugging. (Like logging trajectories to files)"
  OFF
)

OPTION(
  USE_MOTION_ACTUATORS
  "Turn on/off commands being sent from motion to the actuators"
  ON
)
