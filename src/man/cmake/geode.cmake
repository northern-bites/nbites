

############################ ALDEBARAN DIRECTORY
# Ensure the AL_DIR variable is set

IF( "x$ENV{AL_DIR}x" STREQUAL "xx")
    SET( AL_DIR "/usr/local/nao-1.10" )
  SET( ENV{AL_DIR} ${AL_DIR} )
ELSE( "x$ENV{AL_DIR}x" STREQUAL "xx")
  SET( AL_DIR $ENV{AL_DIR} )
ENDIF( "x$ENV{AL_DIR}x" STREQUAL "xx")

IF( NOT EXISTS ${AL_DIR} )
  MESSAGE( FATAL_ERROR
    "Cannot find the path to Nao directory, configuration halted."
    )
ENDIF( NOT EXISTS ${AL_DIR} )

############################ DEFINES && DEFINITIONS
SET( OE_CROSS_BUILD ON )

############################# CROSS-COMPILATION VARIABLES
# Set the variable for the cross-compilation directory, cmake variables

SET( OE_CROSS_DIR "${AL_DIR}/crosstoolchain" )
SET( OE_SYSROOT "${OE_CROSS_DIR}/staging/geode-linux/" )


SET( CMAKE_CROSSCOMPILING   TRUE  )
SET( CMAKE_SYSTEM_NAME      Linux )
SET( CMAKE_SYSTEM_VERSION   1     )
SET( CMAKE_SYSTEM_PROCESSOR geode )

INCLUDE("${AL_DIR}/crosstoolchain/toolchain-geode.cmake")

# where should we look for libraries we need
SET(CMAKE_FIND_ROOT_PATH  ${OE_SYSROOT} )

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
