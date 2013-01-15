#######################################
##  Build configurations for Noggin. ##
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
SET(
  @PYTHON_PLAYER@  pNone
  CACHE STRING
  "Choose the player to be imported in Switch.py"
  )
# DO NOT add any cache settings or documentation to this variable
#  It is included directly in source files and such things will come
#  along with it
SET(
  PYTHON_PLAYER ${@PYTHON_PLAYER@}
  )