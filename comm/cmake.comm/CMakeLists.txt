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
PROJECT( COMM )


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


############################ CMAKE PACKAGE DIRECTORY
# Set cmakeModules folder
SET( CMAKE_MODULE_PATH $ENV{TRUNK_PATH}/cmake )


############################ BUILDCONFIG
# Include the config file for this build
INCLUDE( buildconfig.cmake )


############################ BASE DEFINITIONS
# Include  the basic definitions accross all projects
INCLUDE( ${CMAKE_MODULE_PATH}/base_definitions.cmake )


############################ DISPLAY
# Display summary of options

MESSAGE( STATUS "" )
MESSAGE( STATUS "...:::: Configuration -  COMM ::::..." )
MESSAGE( STATUS "" )


############################ FIND PACKAGE
# Libraries specifics. Define here cmake packages
# usefull to your project

FIND_PACKAGE( NBCOMMON REQUIRED )
FIND_PACKAGE( PYTHON   REQUIRED )
IF( ROBOT_NAO )
  FIND_PACKAGE( PTHREAD  REQUIRED )
ENDIF( ROBOT_NAO )


############################ TESTS
# Define main test script location
#SET( TEST_RUN "${AL_DIR}/extern/python/aldebaran/test_run.py" )

#<ADD_TEST> don't remove this comment
### test example ADD_TEST(My_test python ${TEST_RUN} -p ${NAOQI} -s ${CMAKE_CURRENT_SOURCE_DIR}/test/myRubyTest.rb)

#</ADD_TEST> don't remove this comment 


############################ CONFIG.H GENERATION
CONFIGURE_FILE(
  "${COMM_CMAKE_DIR}/commconfig.in"
  "${COMM_INCLUDE_DIR}/commconfig.h"
    ESCAPE_QUOTES
  )


############################ DEFINITION
# Add here definition needed.
IF( ROBOT_NAO )
  ADD_DEFINITIONS(
    ${NBCOMMON_DEFINITIONS}
    ${PTHREAD_DEFINITIONS}
    ${PYTHON_DEFINITIONS}
    )
ELSE( ROBOT_NAO )
  ADD_DEFINITIONS(
    ${NBCOMMON_DEFINITIONS}
    ${PYTHON_DEFINITIONS}
    )
ENDIF( ROBOT_NAO )


############################ INCLUDE DIRECTORY
# Define include directories

INCLUDE_DIRECTORIES(
  ${NBCOMMON_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIR}
  ${VISION_INCLUDE_DIR}
  ${COMM_INCLUDE_DIR}
  )
IF( ROBOT_NAO )
  INCLUDE_DIRECTORIES(
    ${PTHREAD_INCLUDE_DIR}
    ${MAN_MOTION_INCLUDE_DIR}
    )
ENDIF( ROBOT_NAO )


############################ PROJECT SOURCES FILES 
# Add here source files needed to compile this project
SET( COMM_SRCS ${COMM_INCLUDE_DIR}/Comm
               ${COMM_INCLUDE_DIR}/CommTimer
               ${COMM_INCLUDE_DIR}/DataSerializer
               ${COMM_INCLUDE_DIR}/GameController
               ${COMM_INCLUDE_DIR}/RoboCupGameControlData
               ${COMM_INCLUDE_DIR}/TOOLConnect
               )

IF( PYTHON_SHARED_COMM )
  LIST( APPEND COMM_SRCS ${CORPUS_INCLUDE_DIR}/Sensors
                         ${CORPUS_INCLUDE_DIR}/NaoPose
                         )
ENDIF( PYTHON_SHARED_COMM )

SET( COMM_PYTHON_SRCS ${COMM_INCLUDE_DIR}/__init__.py )

############################ CHECK LIBRARY / EXECUTABLE OPTION
# Configure the output execuatable or shared library, and the install
# commands

IF( PYTHON_SHARED_COMM )
  ADD_LIBRARY(
    ${COMM_TARGET}
    SHARED
    ${COMM_SRCS}
    )
  # Install to _motionmodule.so, instead of lib_motion.so
  GET_TARGET_PROPERTY(
    COMM_TARGET_LOCATION ${COMM_TARGET} LOCATION
    )
  INSTALL(
    FILES ${COMM_TARGET_LOCATION}
    DESTINATION ${COMM_PYTHON_TARGET_DIR}
    CONFIGURATIONS "" Debug Release
    RENAME ${COMM_TARGET}module.so
    )
ELSE( PYTHON_SHARED_COMM )
  ADD_LIBRARY(
    ${COMM_TARGET}
    STATIC
    ${COMM_SRCS}
    )
  # No INSTALL target when static
ENDIF( PYTHON_SHARED_COMM )

INSTALL(
  FILES ${COMM_PYTHON_SRCS}
  DESTINATION ${COMM_PYTHON_TARGET_DIR}
  CONFIGURATIONS "" Debug Release
  )

############################ SET LIBRARIES TO LINK WITH
IF( ROBOT_NAO )
  TARGET_LINK_LIBRARIES(
    ${COMM_TARGET}
      ${PTHREAD_LIBRARIES}
      ${PYTHON_LIBRARIES}
      ${VISION_TARGET}
      ${SYNCHRO_TARGET}
    )
ELSE( ROBOT_NAO )
  TARGET_LINK_LIBRARIES(
    ${COMM_TARGET}
      ${PYTHON_LIBRARIES}
      ${VISION_TARGET}
    )
ENDIF( ROBOT_NAO )


############################ (SUB)DIRECTORY COMPILATION
# Set the sudirectories (some may not actually be subdirectories) to
# include in this package
#IF( PYTHON_SHARED_COMM AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
#  SUBDIRS(
#    ${VISION_CMAKE_DIR}
#    )
#ENDIF( PYTHON_SHARED_COMM AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
