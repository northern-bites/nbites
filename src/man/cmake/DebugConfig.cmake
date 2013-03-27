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
  DEBUG_COMM
  "Toggle comm debug mode."
  OFF
)

option(
  DEBUG_THREADS
  "Toggle threads debug mode."
  OFF
)