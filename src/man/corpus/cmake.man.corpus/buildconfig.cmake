

#######################################
##  Build configurations for Corpus. ##
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
    PYTHON_SHARED_CORPUS
    "Compile Python sensors and _leds module as a shared library for dynamic loading"
    OFF
    )
OPTION(
    USE_PYSENSORS_FAKE_BACKEND
    "Insert a 'fake' Sensors object into the Python module"
    OFF
    )
OPTION(
    USE_PYLEDS_CXX_BACKEND
    "Turn on/off the actual backend proxy calls to the ALLeds module"
    ON
    )

OPTION(
    DEBUG_THREAD
    "Turn on/off debugging information for the Thread class."
    OFF
    )

OPTION(
    DEBUG_ALIMAGE
    "Turn on/off debugging information for ALImageTranscriber"
    OFF
    )

OPTION(
    SAVE_ALL_FRAMES
    "Turn on/off saving every frame in-game."
    OFF
    )
OPTION(
  CAN_SAVE_FRAMES
  "Enable the ability to save or stream frames. Slower performance."
  ON
  )