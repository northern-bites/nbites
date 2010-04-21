

############################ PROJECT NAME
# The name of this cmake project
PROJECT( CORPUS )


############################ VERSION
# Check cMake version
CMAKE_MINIMUM_REQUIRED( VERSION 2.6.0 )


############################ COLORIZE MAKEFILE
# Colorized output
SET( CMAKE_COLOR_MAKEFILE TRUE )


############################ NBCOMMON
# Ensure the TRUNK_PATH variable is set

IF ("x$ENV{TRUNK_PATH}x" STREQUAL "xx")
  GET_FILENAME_COMPONENT( TRUNK_PATH ../../ ABSOLUTE)
  SET( ENV{TRUNK_PATH} ${TRUNK_PATH} )
  MESSAGE( STATUS 
    "Environment variable TRUNK_PATH was not set, reseting to default ${TRUNK_PATH}!" )
ELSE ("x$ENV{TRUNK_PATH}x" STREQUAL "xx")
  SET( TRUNK_PATH $ENV{TRUNK_PATH} )
ENDIF ("x$ENV{TRUNK_PATH}x" STREQUAL "xx")


############################ BUILDCONFIG
# Include the config file for this build
INCLUDE( buildconfig.cmake )


############################ BASE DEFINITIONS
# Include  the basic definitions accross all projects
INCLUDE( ${TRUNK_PATH}/cmake/base_definitions.cmake )


############################ DISPLAY
# Display summary of options

MESSAGE( STATUS "" )
MESSAGE( STATUS "...:::: Configuration -  CORPUS ::::..." )
MESSAGE( STATUS "" )


############################ FIND PACKAGE
# Libraries specifics. Define here cmake packages
# usefull to your project

FIND_PACKAGE( NBCOMMON REQUIRED )
FIND_PACKAGE( PTHREAD  REQUIRED )
FIND_PACKAGE( PYTHON   REQUIRED )


IF(WEBOTS_BACKEND)
  FIND_PACKAGE(WEBOTS REQUIRED)
ELSE(WEBOTS_BACKEND)

  IF( APPLE )
    MESSAGE( STATUS "WARNING - Apple OS, some compilation may fail")
    #On MAC OS X, we need to build w/o linking against non-existant ALCOMMON libs
    #when we want to compile just the vision/sensors libraries for the TOOL
    FIND_PACKAGE( ALCOMMON )
  ELSE( APPLE )
    FIND_PACKAGE( ALCOMMON REQUIRED )
  ENDIF( APPLE )
ENDIF(WEBOTS_BACKEND)

############################ CONFIG.H GENERATION
CONFIGURE_FILE(
  ${CORPUS_CMAKE_DIR}/corpusconfig.h.in
  ${CORPUS_INCLUDE_DIR}/corpusconfig.h
  ESCAPE_QUOTES
  )


############################ DEFINITIONS
# Add here definition needed.
ADD_DEFINITIONS( ${NBCOMMON_DEFINITIONS}
  ${PTHREAD_DEFINITIONS}
  ${PYTHON_DEFINITIONS}
  )


############################ INCLUDE DIRECTORY
# Define include directories
INCLUDE_DIRECTORIES( ${NBCOMMON_INCLUDE_DIR}
  ${PTHREAD_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIR}
  ${VISION_INCLUDE_DIR}
  ${CORPUS_INCLUDE_DIR}
  )


#Possibly, don't need to include any AL or WEBOTS stuff in this directory
# since we may inherit it from man
IF(WEBOTS_BACKEND)
  INCLUDE_DIRECTORIES(${WEBOTS_INCLUDE_DIR})

  ADD_DEFINITIONS( ${WEBOTS_DEFINITIONS})

ELSE(WEBOTS_BACKEND)
  INCLUDE_DIRECTORIES(${ALCOMMON_INCLUDE_DIR})

  ADD_DEFINITIONS( ${ALCOMMON_DEFINITIONS})

ENDIF(WEBOTS_BACKEND)

############################ PROJECT SOURCES FILES 
# Add here source files needed to compile this project
SET( SENSORS_SRCS ${CORPUS_INCLUDE_DIR}/Sensors
  ${CORPUS_INCLUDE_DIR}/PySensors
  ${CORPUS_INCLUDE_DIR}/NaoPose )


###### SYNCHRO ############

SET( SYNCHRO_SRCS ${CORPUS_INCLUDE_DIR}/synchro
  ${CORPUS_INCLUDE_DIR}/_synchromodule
  ${CORPUS_INCLUDE_DIR}/messaging
  )

########### ROBOT CONNET #########
SET( ROBOT_CONNECT_SRCS ${CORPUS_INCLUDE_DIR}/AccEKF
  ${CORPUS_INCLUDE_DIR}/RoboGuardian
  ${CORPUS_INCLUDE_DIR}/ClickableButton
  ${CORPUS_INCLUDE_DIR}/PyRoboGuardian
  ${CORPUS_INCLUDE_DIR}/Lights
  ${CORPUS_INCLUDE_DIR}/PyLights)

IF(WEBOTS_BACKEND)
  LIST( APPEND ROBOT_CONNECT_SRCS ${CORPUS_INCLUDE_DIR}/WBEnactor
    ${CORPUS_INCLUDE_DIR}/AngleEKF
    ${CORPUS_INCLUDE_DIR}/WBTranscriber
    ${CORPUS_INCLUDE_DIR}/WBImageTranscriber
    )
ELSE(WEBOTS_BACKEND)
  LIST( APPEND ROBOT_CONNECT_SRCS ${CORPUS_INCLUDE_DIR}/ALEnactor
    ${CORPUS_INCLUDE_DIR}/NaoEnactor
    ${CORPUS_INCLUDE_DIR}/ALTranscriber
    ${CORPUS_INCLUDE_DIR}/ALImageTranscriber
    ${CORPUS_INCLUDE_DIR}/NaoLights
    ${CORPUS_INCLUDE_DIR}/NaoRGBLight)
ENDIF(WEBOTS_BACKEND)


#IF( PYTHON_SHARED_CORPUS )
#  LIST( APPEND SENSORS_SRCS ${VISION_INCLUDE_DIR}/Pose
#                            ${VISION_INCLUDE_DIR}/PoseMatrix
#                            )
#ENDIF( PYTHON_SHARED_CORPUS )

SET( CORPUS_PYTHON_SRCS ${CORPUS_INCLUDE_DIR}/__init__.py
  ${CORPUS_INCLUDE_DIR}/leds.py
  ${CORPUS_INCLUDE_DIR}/sensors.py
  ${CORPUS_INCLUDE_DIR}/synchro.py
  )

SET( CORPUS_TEST_DIR ${CORPUS_INCLUDE_DIR}/test )


###### NBINCLUDE STUFF ####

SET( NBINCLUDE_SRCS ${NBCOMMON_INCLUDE_DIR}/NBMatrixMath
  ${NBCOMMON_INCLUDE_DIR}/NBMath
  ${CORPUS_INCLUDE_DIR}/CoordFrame4D
  ${CORPUS_INCLUDE_DIR}/CoordFrame3D
  )

############################ CHECK LIBRARY / EXECUTABLE OPTION
# Configure the output execuatable or shared library, and the install
# commands

IF( PYTHON_SHARED_CORPUS )
  ADD_LIBRARY(
    ${SENSORS_TARGET}
    SHARED
    ${SENSORS_SRCS}
    )

  ADD_LIBRARY(
    ${SYNCHRO_TARGET}
    SHARED
    ${SYNCHRO_SRCS}
    )
  ADD_LIBRARY(
    ${ROBOT_CONNECT_TARGET}
    SHARED
    ${ROBOT_CONNECT_SRCS}
    )
  ADD_LIBRARY(
    ${NBINCLUDE_TARGET}
    SHARED
    ${NBINCLUDE_SRCS}
    )

  GET_TARGET_PROPERTY( SENSORS_TARGET_LOCATION ${SENSORS_TARGET} LOCATION )
  GET_TARGET_PROPERTY( SYNCHRO_TARGET_LOCATION   ${SYNCHRO_TARGET}   LOCATION )
  GET_TARGET_PROPERTY( ROBOT_CONNECT_TARGET_LOCATION
    ${ROBOT_CONNECT_TARGET}   LOCATION )
  GET_TARGET_PROPERTY( NBINCLUDE_TARGET_LOCATION
    ${NBINCLUDE_TARGET}   LOCATION )
  # Install to TARGETmodule.so, instead of lib_TARGET.so
  INSTALL(
    FILES ${SENSORS_TARGET_LOCATION}
    DESTINATION ${CORPUS_PYTHON_TARGET_DIR}
    CONFIGURATIONS "" Debug Release
    RENAME ${SENSORS_TARGET}module.so
    )
  INSTALL(
    FILES ${SYNCHRO_TARGET_LOCATION}
    DESTINATION ${CORPUS_PYTHON_TARGET_DIR}
    CONFIGURATIONS "" Debug Release
    RENAME ${SYNCHRO_TARGET}module.so
    )
  INSTALL(
    FILES ${ROBOT_CONNECT_TARGET_LOCATION}
    DESTINATION ${CORPUS_PYTHON_TARGET_DIR}
    CONFIGURATIONS "" Debug Release
    RENAME ${ROBOT_CONNECT_TARGET}module.so
    )
  INSTALL(
    FILES ${NBINCLUDE_TARGET_LOCATION}
    DESTINATION ${CORPUS_PYTHON_TARGET_DIR}
    CONFIGURATIONS "" Debug Release
    RENAME ${NBINCLUDE_TARGET_LOCATION}module.so
    )

ELSE( PYTHON_SHARED_CORPUS )
  ADD_LIBRARY(
    ${SENSORS_TARGET}
    STATIC
    ${SENSORS_SRCS}
    )
  ADD_LIBRARY(
    ${SYNCHRO_TARGET}
    STATIC
    ${SYNCHRO_SRCS}
    )
  ADD_LIBRARY(
    ${ROBOT_CONNECT_TARGET}
    STATIC
    ${ROBOT_CONNECT_SRCS}
    )
  ADD_LIBRARY(
    ${NBINCLUDE_TARGET}
    STATIC
    ${NBINCLUDE_SRCS}
    )
  # No INSTALL target when static
ENDIF( PYTHON_SHARED_CORPUS )

INSTALL(
  FILES ${CORPUS_PYTHON_SRCS}
  DESTINATION ${CORPUS_PYTHON_TARGET_DIR}
  CONFIGURATIONS "" Debug Release
  )


############################ LINKER LANGUAGE
# For Mac's, set the target link language to use

#IF( APPLE )
SET_TARGET_PROPERTIES( ${SENSORS_TARGET} PROPERTIES LINKER_LANGUAGE CXX )
#ENDIF( APPLE )


############################ SET LIBRARIES TO LINK WITH
TARGET_LINK_LIBRARIES( ${SENSORS_TARGET} ${NBCOMMON_LIBRARIES}
  ${ALCOMMON_LIBRARIES}

  ${PTHREAD_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${VISION_LIBRARIES}
  )
TARGET_LINK_LIBRARIES( ${SYNCHRO_TARGET} ${NBCOMMON_LIBRARIES}
  ${ALCOMMON_LIBRARIES}
  ${PTHREAD_LIBRARIES}
  ${PYTHON_LIBRARIES}
  )

TARGET_LINK_LIBRARIES( ${ROBOT_CONNECT_TARGET} ${NBCOMMON_LIBRARIES}
  ${ALCOMMON_LIBRARIES}
  ${PTHREAD_LIBRARIES}
  ${PYTHON_LIBRARIES}
  )
TARGET_LINK_LIBRARIES( ${NBINCLUDE_TARGET} ${NBCOMMON_LIBRARIES}
  )

############################ (SUB)DIRECTORY COMPILATION
# Set the sudirectories (some may not actually be subdirectories) to
# include in this package
#SUBDIRS(
#  )


