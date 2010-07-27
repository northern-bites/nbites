##############################
#
# WELCOME
#
# This is the main cmake configuration file
# associate to your projects
# You can define here specific compilation options
#
# More about cmake : www.cmake.org
#
# Using cmake command line :
# 1. Create a build directory : mkdir build
# 2. Configure cmake project : cd build; cmake ..
#   You can also generate project with your favorite IDE :
#   Example : Under Mac osx use "cmake .. -G XCode"
#   see cmake help for more information
# 3. Edit project option : ccmake .
# 4. Build it : make



############################ PROJECT NAME
# The name of this cmake project
PROJECT( MAN )


############################ VERSION
CMAKE_POLICY(SET CMP0011 OLD)
# Check cMake version
CMAKE_MINIMUM_REQUIRED( VERSION 2.6.0 )


############################ COLORIZE MAKEFILE
# Colorized output
SET( CMAKE_COLOR_MAKEFILE TRUE )


############################ NBCOMMON
# Ensure the TRUNK_PATH variable is set

IF ("x$ENV{TRUNK_PATH}x" STREQUAL "xx")
  GET_FILENAME_COMPONENT( TRUNK_PATH ../ ABSOLUTE)
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


############################ DEFAULT BUILD TYPE
# Set which build type will be used by default, if none is set
#IF( "x${CMAKE_BUILD_TYPE}x" STREQUAL "xx" )
SET( CMAKE_BUILD_TYPE CACHE FORCE "Release")
#ENDIF( "x${CMAKE_BUILD_TYPE}x" STREQUAL "xx" )


############################ DISPLAY
# Display summary of options

MESSAGE( STATUS "" )
MESSAGE( STATUS "...:::: Configuration -  MAN ::::..." )
MESSAGE( STATUS "" )


############################ FIND PACKAGE
# Libraries specifics. Define here cmake packages
# usefull to your project

FIND_PACKAGE( NBCOMMON REQUIRED )
FIND_PACKAGE( PYTHON   REQUIRED )
FIND_PACKAGE( PTHREAD  REQUIRED )
FIND_PACKAGE( BOOTS    REQUIRED )

IF(APPLE)
  FIND_PACKAGE( FINK )
ENDIF(APPLE)

IF(WIN32 AND NOT OE_CROSS_BUILD)
  FIND_PACKAGE( MINGW )
ENDIF(WIN32 AND NOT OE_CROSS_BUILD)

IF(WEBOTS_BACKEND)
  FIND_PACKAGE(WEBOTS)
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
  ${MAN_CMAKE_DIR}/manconfig.in
  ${MAN_INCLUDE_DIR}/manconfig.h
  ESCAPE_QUOTES
  )


############################ OFFLINE

SET( OFFLINE_NAOQI_PATH
  $ENV{AL_DIR}
  CACHE STRING
  "The local path of the naoqi distribution to use with Webots"
  )



############################ REMOTE INSTALLATION
# Define this to remotely install the binary or library on the robot.
# Cannot be selected unless OE_CROSS_BUILD is set to ON.
IF(WEBOTS_BACKEND)
  OPTION( REMOTE_INSTALL
    "Remotely install the libary or binary on the robot."
    OFF
    )
ELSE(WEBOTS_BACKEND)
  OPTION( REMOTE_INSTALL
    "Remotely install the libary or binary on the robot."
    ON
    )
ENDIF(WEBOTS_BACKEND)

SET( @REMOTE_ADDRESS@
  "192.168.0.5"
  CACHE STRING
  "The remote address of the robot"
  )
SET( REMOTE_ADDRESS ${@REMOTE_ADDRESS@} )

############################ REMOTE INSTALLATION
# Define this to remotely install the binary or library on the robot.
# Cannot be selected unless OE_CROSS_BUILD is set to ON.
SET( @ROBOT_NAME@
  "zaphod"
  CACHE STRING
  "The NAME of the robot"
  )
SET( ROBOT_NAME ${@ROBOT_NAME@} )


############################ COLOR TABLE SELECTION
# Set the path to the color table you wish to be loaded into install/etc
SET( COLOR_TABLE
  "../../data/tables/uvy/lab120/andrewPostSingapore.mtb"
  CACHE STRING
  "Path relative to man directory for the desired color table"
  )

#upload.sh needs to get generated after REMOTE and OFFLINE stuff is set
CONFIGURE_FILE(
  ${MAN_CMAKE_DIR}/upload.sh.in
  ${MAN_INCLUDE_DIR}/upload.sh
  ESCAPE_QUOTES
  )


CONFIGURE_FILE(
  ${MAN_CMAKE_DIR}/nameconfig.in
  ${NBCOMMON_INCLUDE_DIR}/nameconfig.h
  ESCAPE_QUOTES
  )

############################ DEFINITIONS
# Add here definition needed.
ADD_DEFINITIONS( ${NBCOMMON_DEFINITIONS}
  ${ALCOMMON_DEFINITIONS}
  ${PTHREAD_DEFINITIONS}
  ${PYTHON_DEFINITIONS}
  ${WEBOTS_DEFINITIONS}
  )


############################ INCLUDE DIRECTORY
# Define include directories
INCLUDE_DIRECTORIES( ${NBCOMMON_INCLUDE_DIR}
  ${PTHREAD_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIR}
  ${BOOST_INCLUDE_DIR}
  ${ALCOMMON_INCLUDE_DIR}
  ${FINK_INCLUDE_DIR}
  ${MINGW_INCLUDE_DIR}
  ${COMM_INCLUDE_DIR}
  ${CORPUS_INCLUDE_DIR}
  ${MOTION_INCLUDE_DIR}
  ${NOGGIN_INCLUDE_DIR}
  ${VISION_INCLUDE_DIR}
  ${MAN_INCLUDE_DIR}
  ${WEBOTS_INCLUDE_DIR}
  )

############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project

SET( MAN_SRCS ${MAN_INCLUDE_DIR}/Man
  ${MAN_INCLUDE_DIR}/TMan
  ${MAN_INCLUDE_DIR}/TTMan
  )

IF(WEBOTS_BACKEND)
  LIST(APPEND MAN_SRCS ${MAN_INCLUDE_DIR}/nao_soccer_player
    )
ELSE(WEBOTS_BACKEND)
  LIST(APPEND MAN_SRCS ${MAN_INCLUDE_DIR}/manmodule
    )
ENDIF(WEBOTS_BACKEND)

SET( MAN_PYTHON_SRCS ${MAN_INCLUDE_DIR}/__init__.py )

############################ CHECK LIBRARY / EXECUTABLE OPTION
# Configure the output execuatable or shared library, and the install
# commands

GET_TARGET_PROPERTY( MAN_LOCATION ${MAN_TARGET} LOCATION )

IF( MAN_IS_REMOTE )
  ADD_EXECUTABLE(
    ${MAN_TARGET}
    ${MAN_SRCS}
    )
ELSE( MAN_IS_REMOTE )
  ADD_LIBRARY(
    ${MAN_TARGET}
    SHARED
    ${MAN_SRCS}
    )
ENDIF( MAN_IS_REMOTE )
INSTALL(
  TARGETS ${MAN_TARGET}
  RUNTIME DESTINATION ${OUTPUT_ROOT_DIR_BIN}
  LIBRARY DESTINATION ${OUTPUT_ROOT_DIR_LIB}
  CONFIGURATIONS "" Debug Release
  )
INSTALL(
  FILES ${MAN_PYTHON_SRCS}
  DESTINATION ${MAN_PYTHON_TARGET_DIR}
  CONFIGURATIONS "" Debug Release
  )

############################ SET LIBRARIES TO LINK WITH
TARGET_LINK_LIBRARIES(
  ${MAN_TARGET}
  ${NBCOMMON_LIBRARIES}
  ${PTHREAD_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${ALCOMMON_LIBRARIES}
  ${COMM_TARGET}
  ${SENSORS_TARGET}
  ${SYNCHRO_TARGET}
  ${ROBOT_CONNECT_TARGET}
  ${NBINCLUDE_TARGET}
  ${MOTION_TARGET}
  ${NOGGIN_TARGET}
  ${VISION_TARGET}
  ${WEBOTS_LIBRARIES}
  ${BOOST_LIBRARIES}
  )

############################ (SUB)DIRECTORY COMPILATION
# Set the sudirectories (some may not actually be subdirectories) to
# include in this package
SUBDIRS(
  ${COMM_CMAKE_DIR}
  ${CORPUS_CMAKE_DIR}
  ${MOTION_CMAKE_DIR}
  ${NOGGIN_CMAKE_DIR}
  ${VISION_CMAKE_DIR}
  )


