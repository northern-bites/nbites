

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

############################# CROSS-COMPILATION VARIABLES
# Set the variable for the cross-compilation directory, cmake variables

SET( OE_CROSS_DIR "${AL_DIR}/crosstoolchain" )

SET( CMAKE_CROSSCOMPILING   TRUE  )
SET( CMAKE_SYSTEM_NAME      Linux )
SET( CMAKE_SYSTEM_VERSION   1     )
SET( CMAKE_SYSTEM_PROCESSOR geode )

INCLUDE("${AL_DIR}/crosstoolchain/toolchain-geode.cmake")