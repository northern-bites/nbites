

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
  PYTHON_SHARED_VISION
  "Compile VISION as a shared library for Python dynamic loading"
  OFF
  )
OPTION(
  USE_PYVISION_FAKE_BACKEND
  "Insert a 'fake' Vision object into the Python module"
  OFF
  )
OPTION(
  USE_TIME_PROFILING
  "Turn on/off profiling function calls"
  OFF
  )
OPTION(
  USE_PROFILER_AUTO_HALT
  "Turn on/off automatic profiling summary printing"
  ON
  )

