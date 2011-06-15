

############################ ALDEBARAN DIRECTORY
# Ensure the AL_DIR variable is set

IF( "x$ENV{AL_DIR}x" STREQUAL "xx")
    SET( AL_DIR "/usr/local/nao-1.10.37" )
  SET( ENV{AL_DIR} ${AL_DIR} )
ELSE( "x$ENV{AL_DIR}x" STREQUAL "xx")
  SET( AL_DIR $ENV{AL_DIR} )
ENDIF( "x$ENV{AL_DIR}x" STREQUAL "xx")

IF( NOT EXISTS ${AL_DIR} )
  MESSAGE( FATAL_ERROR
    "Cannot find the path to Nao directory, configuration halted."
    )
ENDIF( NOT EXISTS ${AL_DIR} )

########################## PLATFORM
IF( WIN32 )
  SET( TARGET_ARCH "windows" )
  SET( TARGET_HOST "TARGET_HOST_WINDOWS")
ENDIF( WIN32 )

IF( UNIX )
  SET( TARGET_ARCH "linux")
  SET( TARGET_HOST "TARGET_HOST_LINUX")
  SET( PLATFORM_X86 1 )
ENDIF( UNIX )

IF( APPLE )
  SET( SDK_ARCH "macosx" )
  SET( TARGET_HOST "TARGET_HOST_MACOSX")
ENDIF( APPLE )

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
SET(CMAKE_FIND_ROOT_PATH  ${OE_SYSROOT} $ENV{NBITES_DIR}/ext/)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
