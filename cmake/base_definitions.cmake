# .:: Basic Definitions ::::::::::::::::::::::::::::::::::::::::::::::::
#
# The basic diefinitions for the Northern Bites cmake packages, used
# throughout different packages' configurations

############################ TRUNK PATH
# Ensure the TRUNK_PATH variable is set

IF( "x$ENV{TRUNK_PATH}x" STREQUAL "xx")
  GET_FILENAME_COMPONENT( TRUNK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/.. ABSOLUTE)
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

############################ TRUNK REVISION
# Record the current revision number of the repository
#SET( REMOTE_ADDRESS ${@REMOTE_ADDRESS@} )
############################ ALDEBARAN DIRECTORY
# Ensure the AL_DIR variable is set

IF( "x$ENV{AL_DIR}x" STREQUAL "xx")
  IF (WEBOTS_BACKEND)
    SET( AL_DIR "/usr/local/nao-1.2" )
  ELSE (WEBOTS_BACKEND)
    SET( AL_DIR "/usr/local/nao-1.6" )
  ENDIF (WEBOTS_BACKEND)
  SET( ENV{AL_DIR} ${AL_DIR} )
  MESSAGE( STATUS
    "Environment variable AL_DIR was not set, reseting to default ${AL_DIR}!" )
ELSE( "x$ENV{AL_DIR}x" STREQUAL "xx")
  SET( AL_DIR $ENV{AL_DIR} )
ENDIF( "x$ENV{AL_DIR}x" STREQUAL "xx")

IF( NOT EXISTS ${AL_DIR} )
  MESSAGE( FATAL_ERROR
    "Cannot find the path to Nao directory, configuration halted."
    )
ENDIF( NOT EXISTS ${AL_DIR} )


############################ MAN INSTALL PREFIX
# Ensure the MAN_INSTALL_PREFIX variable is set

IF( "x$ENV{MAN_INSTALL_PREFIX}x" STREQUAL "xx")
  GET_FILENAME_COMPONENT(
    MAN_INSTALL_PREFIX ${TRUNK_PATH}/install ABSOLUTE
    )
  SET( ENV{MAN_INSTALL_PREFIX} ${MAN_INSTALL_PREFIX} )
  MESSAGE( STATUS
    "Environment variable MAN_INSTALL_PREFIX was not set, resetting to default ${MAN_INSTALL_PREFIX}!" )
ELSE( "x$ENV{MAN_INSTALL_PREFIX}x" STREQUAL "xx")
  SET( MAN_INSTALL_PREFIX $ENV{MAN_INSTALL_PREFIX} )
ENDIF( "x$ENV{MAN_INSTALL_PREFIX}x" STREQUAL "xx")

# Make it an editable cache variable
#  ** Right now, can't figure this out, not editable ** - jfishman@
#SET(
#  MAN_INSTALL_PREFIX ${MAN_INSTALL_PREFIX}
#  CACHE STRING "Install prefix."
#  )


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


########################### MODULE PATH
# Set the path from which CMake should load modules
SET( CMAKE_MODULE_PATH ${TRUNK_PATH}/cmake )


############################ ROBOT TYPE
# Definitions for the type of robot (for compilation definitions), and
# prefixes for library, executable, and path names

IF( NOT DEFINED ROBOT_TYPE )
  SET( ROBOT_TYPE NAO_RL )
ENDIF( NOT DEFINED ROBOT_TYPE )
SET( ROBOT_TYPE ${ROBOT_TYPE} CACHE STRING "Robot type" )

IF( ${ROBOT_TYPE} STREQUAL AIBO_ERS7 OR ${ROBOT_TYPE} STREQUAL AIBO_220)
  SET( ROBOT_PREFIX aibo )
  SET( ROBOT_AIBO TRUE  )
  SET( ROBOT_NAO  FALSE )
ELSE( ${ROBOT_TYPE} STREQUAL AIBO_ERS7 OR ${ROBOT_TYPE} STREQUAL AIBO_220)
  SET( ROBOT_PREFIX nao )
  SET( ROBOT_AIBO FALSE )
  SET( ROBOT_NAO  TRUE  )
ENDIF( ${ROBOT_TYPE} STREQUAL AIBO_ERS7 OR ${ROBOT_TYPE} STREQUAL AIBO_220)


############################ OUTPUT LOCATION
# Define output directories.  Binaries, documentation, and libraries are
# copied to final locations depending on robot type.

SET(
  CMAKE_INSTALL_PREFIX ${MAN_INSTALL_PREFIX}
  CACHE INTERNAL "Install prefix"
  )

SET( OUTPUT_ROOT_DIR_BIN "${CMAKE_INSTALL_PREFIX}/bin" )
SET( OUTPUT_ROOT_DIR_DOC "${CMAKE_INSTALL_PREFIX}/doc" )
SET( OUTPUT_ROOT_DIR_LIB "${CMAKE_INSTALL_PREFIX}/lib" )


############################ PREFIX CONFIRUGATION
# Depending on the robot and whether cross-compiling, the include and
# library prefixes must be adjusted

  # Nao
IF( AL_DIR STREQUAL "" )
  MESSAGE( FATAL_ERROR "Environment variable 'AL_DIR' is not set !" )
ENDIF( AL_DIR STREQUAL "" )

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
  SET( TARGET_ARCH "macosx" )
  SET( TARGET_HOST "TARGET_HOST_MACOSX")
ENDIF( APPLE )

IF( OE_CROSS_BUILD )
  SET( OE_SYSROOT ${OE_CROSS_DIR}/staging/geode-linux/)
ENDIF (OE_CROSS_BUILD )

IF( FINAL_RELEASE )
  ADD_DEFINITIONS(-DFINAL_RELEASE)
ENDIF( FINAL_RELEASE )

INCLUDE( "${CMAKE_MODULE_PATH}/proxies.cmake" )

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

