# .:: Basic Definitions ::::::::::::::::::::::::::::::::::::::::::::::::
#
# The basic definitions for the Northern Bites cmake packages, used
# throughout different packages' configurations

############################ DISPLAY
# Display summary of options

MESSAGE( STATUS "" )
MESSAGE( STATUS "...:::: Configuration -  ${PROJECT_NAME} ::::..." )
MESSAGE( STATUS "" )

############################ VERSION
# Check cMake version
CMAKE_MINIMUM_REQUIRED( VERSION 2.6.0 )

############################### COMPILER STUFF

############################ DEFAULT BUILD TYPE
# Set which build type will be used by default, if none is set
SET( CMAKE_BUILD_TYPE CACHE FORCE "Release")

########SETTING UP THE COMPILER FLAGS ##########
# Notes: -JS, GS Feb. 2009
# Note: gcc 4.2 doesnt have a geode processor type.
#       k6-2 has a similar instruction set, so we use it instead
#       this is important for allowing linkage and running of -O1,2,3 bins/libs
#
# Note: The default flags never get set by cmake.
# Note: We override the default CMAKE release and debug flags with our own
# Note: We set the C flags to be the same as the CXX flags

# NOTE: This is clearly a hack. Only Linux has librealtime, but I was
#       having a hard time trying to figure out how to get every
#       module to link against this library without adding it to every
#       single library definition. This was the solution I came up with.
#        -- Jack
if (UNIX AND NOT APPLE)
  SET( CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -lrt")
endif()

# Default (no release specific) build flags
SET( CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -m32 -Wall -Wconversion -Wno-unused -Wno-write-strings")
SET( CMAKE_C_FLAGS
  "${CMAKE_CXX_FLAGS}" )
# Release build flags
SET( CMAKE_CXX_FLAGS_RELEASE
  "-O3 -DNDEBUG")
SET( CMAKE_C_FLAGS_RELEASE
  "${CMAKE_CXX_FLAGS_RELEASE}" )
# Debug build flags
SET( CMAKE_CXX_FLAGS_DEBUG
  "-g3" )
SET( CMAKE_C_FLAGS_DEBUG
  "${CMAKE_CXX_FLAGS_DEBUG}" )

############################ TRUNK PATH
# Ensure the TRUNK_PATH variable is set

IF( "x$ENV{TRUNK_PATH}x" STREQUAL "xx")
  GET_FILENAME_COMPONENT( TRUNK_PATH ${PROJECT_SOURCE_DIR}/ ABSOLUTE)
  SET( ENV{TRUNK_PATH} ${TRUNK_PATH} )
  MESSAGE( STATUS
    "Environment variable TRUNK_PATH was not set, reseting to default ${TRUNK_PATH}!" )
ELSE( "x$ENV{TRUNK_PATH}x" STREQUAL "xx")
  SET( TRUNK_PATH $ENV{TRUNK_PATH} )
ENDIF( "x$ENV{TRUNK_PATH}x" STREQUAL "xx")

IF( NOT EXISTS ${TRUNK_PATH} )
  MESSAGE( FATAL_ERROR
    "Cannot find the path to trunk, configuration halted."
    )
ENDIF( NOT EXISTS ${TRUNK_PATH} )

SET( NBITES_DIR ${TRUNK_PATH}/../.. )
SET( BUILD_DIR ${NBITES_DIR}/build/man )

############################ TRUNK REVISION
# Record the current revision number of the repository
#SET( REMOTE_ADDRESS ${@REMOTE_ADDRESS@} )

############################ ALDEBARAN DIRECTORY
# Ensure the AL_DIR variable is set

IF( "x$ENV{AL_DIR}x" STREQUAL "xx")
  SET( AL_DIR "/usr/local/nao-1.10.37" )
  SET( ENV{AL_DIR} ${AL_DIR} )
  MESSAGE( STATUS
    "reseting Environment variable AL_DIR to default ${AL_DIR}" )
ELSE( "x$ENV{AL_DIR}x" STREQUAL "xx")
  SET( AL_DIR $ENV{AL_DIR} )
ENDIF( "x$ENV{AL_DIR}x" STREQUAL "xx")

IF( NOT EXISTS ${AL_DIR} )
  MESSAGE( FATAL_ERROR
    "Cannot find the path to Nao directory, configuration halted."
    )
ENDIF( NOT EXISTS ${AL_DIR} )

############################ NBITES DIRECTORY
# Ensure the NBITES_DIR variable is set

IF( "x$ENV{NBITES_DIR}x" STREQUAL "xx")
  SET( NBITES_DIR "${NBITES_DIR}" )
  SET( ENV{NBITES_DIR} ${NBITES_DIR} )
  MESSAGE( STATUS
    "reseting Environment variable NBITES_DIR to default ${NBITES_DIR}" )
ELSE( "x$ENV{NBITES_DIR}x" STREQUAL "xx")
  SET( NBITES_DIR $ENV{NBITES_DIR} )
ENDIF( "x$ENV{NBITES_DIR}x" STREQUAL "xx")

IF( NOT EXISTS ${NBITES_DIR} )
  MESSAGE( FATAL_ERROR
    "Path to the nbites directory does not exist!"
    )
ENDIF( NOT EXISTS ${NBITES_DIR} )

############################ MAN INSTALL PREFIX
# Ensure the MAN_INSTALL_PREFIX variable is set

IF( "x$ENV{MAN_INSTALL_PREFIX}x" STREQUAL "xx")
  IF( OE_CROSS_BUILD )
    GET_FILENAME_COMPONENT(
      MAN_INSTALL_PREFIX ${BUILD_DIR}/cross_install ABSOLUTE
       )
    SET( ENV{MAN_INSTALL_PREFIX} ${MAN_INSTALL_PREFIX} )
  ENDIF( OE_CROSS_BUILD )
  IF( WEBOTS_BACKEND )
    GET_FILENAME_COMPONENT(
      MAN_INSTALL_PREFIX ${BUILD_DIR}/webots_install ABSOLUTE
       )
    SET( ENV{MAN_INSTALL_PREFIX} ${MAN_INSTALL_PREFIX} )
  ENDIF( WEBOTS_BACKEND )
  MESSAGE( STATUS
    "Environment variable MAN_INSTALL_PREFIX was not set, resetting to default ${MAN_INSTALL_PREFIX}!" )
ELSE( "x$ENV{MAN_INSTALL_PREFIX}x" STREQUAL "xx")
  SET( MAN_INSTALL_PREFIX $ENV{MAN_INSTALL_PREFIX} )
ENDIF( "x$ENV{MAN_INSTALL_PREFIX}x" STREQUAL "xx")


############################ CMAKE POLICY
# Settings regarding various cmake policy changes from 2.6

IF(COMMAND CMAKE_POLICY)
    # CMake policy regarding library searches
    CMAKE_POLICY(SET CMP0003 OLD)
    # CMake policy regarding excaping definitions
    CMAKE_POLICY(SET CMP0005 OLD)
#     # CMake policy regarding scoped include
      CMAKE_POLICY(SET CMP0011 OLD)
      CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)


############################ ROBOT TYPE
# Definitions for the type of robot (for compilation definitions), and
# prefixes for library, executable, and path names
IF( NOT DEFINED ROBOT_TYPE )
  SET( ROBOT_TYPE NAO_RL_33 )
ENDIF( NOT DEFINED ROBOT_TYPE )
SET( ROBOT_TYPE ${ROBOT_TYPE} CACHE STRING "Robot type" )
SET( ROBOT_PREFIX nao )

############################ OUTPUT LOCATION
# Define output directories.  Binaries, documentation, and libraries are
# copied to final locations depending on robot type.

SET(
  CMAKE_INSTALL_PREFIX ${MAN_INSTALL_PREFIX}
  CACHE INTERNAL "Install prefix"
  )

SET( OUTPUT_ROOT_DIR_BIN "${CMAKE_INSTALL_PREFIX}/bin"		  )
SET( OUTPUT_ROOT_DIR_DOC "${CMAKE_INSTALL_PREFIX}/doc"		  )
SET( OUTPUT_ROOT_DIR_LIB "${CMAKE_INSTALL_PREFIX}/lib/naoqi/"     )
SET( OUTPUT_ROOT_DIR_PREF "${CMAKE_INSTALL_PREFIX}/preferences/"  )

########################### NB Common definitions
include ( ${NBITES_DIR}/src/man/cmake/FindNBCOMMON.cmake )

########################## ADVANCED SETTINGS PREFERENCES
# Set the cache variable that we would rather not appear on the normal
# settings page
MARK_AS_ADVANCED(
  CFG_FILE
  CMAKE_BACKWARDS_COMPATIBILITY
  CMAKE_TOOLCHAIN_FILE
  EXECUTABLE_OUTPUT_PATH
  LIBRARY_OUTPUT_PATH
  AL_PERF_CALCULATION
  )
