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
PROJECT( NOGGIN )


############################ CMAKE VERSION
# Check cMake version
CMAKE_MINIMUM_REQUIRED( VERSION 2.6.0 )


############################ COLORIZE MAKEFILE
# Colorized output
SET( CMAKE_COLOR_MAKEFILE TRUE )


############################ NBCOMMON
# Ensure the TRUNK_PATH variable is set

IF ("x$ENV{TRUNK_PATH}x" STREQUAL "xx")
  GET_FILENAME_COMPONENT( TRUNK_PATH ../../../ ABSOLUTE)
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
MESSAGE( STATUS "...:::: Configuration -  NOGGIN ::::..." )
MESSAGE( STATUS "" )


############################ FIND PACKAGE
# Libraries specifics. Define here cmake packages
# usefull to your project

FIND_PACKAGE( NBCOMMON REQUIRED )
FIND_PACKAGE( PYTHON   REQUIRED )


############################ TeamConfig.py variables
SET(@GC_TEAM_NUMBER@
  "101"
  CACHE STRING
  "The team number for the robot (GameController)"
  )
SET( GC_TEAM_NUMBER ${@GC_TEAM_NUMBER@} )
SET(@GC_PLAYER_NUMBER@
  "2"
  CACHE STRING
  "The player number for the robot (GameController)"
  )
SET( GC_PLAYER_NUMBER ${@GC_PLAYER_NUMBER@} )
############################ CONFIG.H GENERATION
CONFIGURE_FILE(
  "${NOGGIN_CMAKE_DIR}/nogginconfig.in"
  "${NOGGIN_INCLUDE_DIR}/nogginconfig.h"
    ESCAPE_QUOTES
  )
CONFIGURE_FILE(
  "${NOGGIN_CMAKE_DIR}/Switch.py.in"
  "${NOGGIN_INCLUDE_DIR}/players/Switch.py"
  )
CONFIGURE_FILE(
  "${NOGGIN_CMAKE_DIR}/TeamConfig.py.in"
  "${NOGGIN_INCLUDE_DIR}/TeamConfig.py"
  )

CONFIGURE_FILE(
  "${NOGGIN_CMAKE_DIR}/WebotsConfig.py.in"
  "${NOGGIN_INCLUDE_DIR}/WebotsConfig.py"
  )

############################ DEFINITION
# Add here definition needed.
ADD_DEFINITIONS( ${NBCOMMON_DEFINITIONS} ${PYTHON_DEFINITIONS}
                 ${VISION_DEFINITIONS} )


############################ INCLUDE DIRECTORY
# Define include directories
INCLUDE_DIRECTORIES( ${NBCOMMON_INCLUDE_DIR}
                     ${PYTHON_INCLUDE_DIR}
                     ${VISION_INCLUDE_DIR}
                     ${CORPUS_INCLUDE_DIR}
                     ${NOGGIN_INCLUDE_DIR}
                     )


############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project

SET( NOGGIN_SRCS ${NOGGIN_INCLUDE_DIR}/Noggin
                 ${NOGGIN_INCLUDE_DIR}/Observation
                 ${NOGGIN_INCLUDE_DIR}/MCL
                 #${NOGGIN_INCLUDE_DIR}/EKF
                 ${NOGGIN_INCLUDE_DIR}/BallEKF
                 ${NOGGIN_INCLUDE_DIR}/PyLoc
                 ${NOGGIN_INCLUDE_DIR}/LocEKF
                 ${NOGGIN_INCLUDE_DIR}/NogginStructs.h
                 )


SET( NOGGIN_PYTHON_SRCS ${NOGGIN_INCLUDE_DIR}/__init__.py
                        ${NOGGIN_INCLUDE_DIR}/Brain.py
                        ${NOGGIN_INCLUDE_DIR}/GameController.py
                        ${NOGGIN_INCLUDE_DIR}/GameStates.py
                        ${NOGGIN_INCLUDE_DIR}/FallController.py
                        ${NOGGIN_INCLUDE_DIR}/FallStates.py
                        ${NOGGIN_INCLUDE_DIR}/NavStates.py
                        ${NOGGIN_INCLUDE_DIR}/NavConstants.py
                        ${NOGGIN_INCLUDE_DIR}/Navigator.py
                        ${NOGGIN_INCLUDE_DIR}/NogginConstants.py
                        ${NOGGIN_INCLUDE_DIR}/Leds.py
                        ${NOGGIN_INCLUDE_DIR}/TeamConfig.py
                        ${NOGGIN_INCLUDE_DIR}/WebotsConfig.py
                        )

SET( NOGGIN_PYTHON_SUBDIRS ${NOGGIN_INCLUDE_DIR}/headTracking
                           ${NOGGIN_INCLUDE_DIR}/playbook
                           ${NOGGIN_INCLUDE_DIR}/players
                           ${NOGGIN_INCLUDE_DIR}/robots
                           ${NOGGIN_INCLUDE_DIR}/typeDefs
                           ${NOGGIN_INCLUDE_DIR}/util )


############################ BUILD TARGETS

ADD_LIBRARY(
  ${NOGGIN_TARGET}
  STATIC
  ${NOGGIN_SRCS}
  )
#INSTALL(
#  TARGETS ${NOGGIN_TARGET}
#  ARCHIVE DESTINATION ${OUTPUT_ROOT_DIR_LIB}
#  CONFIGURATIONS Debug
#  )
INSTALL(
  FILES ${NOGGIN_PYTHON_SRCS}
  DESTINATION ${NOGGIN_PYTHON_TARGET_DIR}
  CONFIGURATIONS "" Debug Release
  )
INSTALL(
  DIRECTORY ${NOGGIN_PYTHON_SUBDIRS}
  DESTINATION ${NOGGIN_PYTHON_TARGET_DIR}/
  CONFIGURATIONS "" Debug Release
  FILES_MATCHING PATTERN "*.py"
  )


############################ LINKER LANGUAGE
# For Mac's, set the target link language to use

#IF( APPLE )
  SET_TARGET_PROPERTIES(
    ${NOGGIN_TARGET}
    PROPERTIES LINKER_LANGUAGE CXX
    )
#ENDIF( APPLE )


############################ SET LIBRARIES TO LINK WITH
TARGET_LINK_LIBRARIES(
  ${NOGGIN_TARGET}
  ${NBCOMMON_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${VISION_TARGET}
  ${SENSORS_TARGET}
  )


############################ (SUB)DIRECTORY COMPILATION
# Set the sudirectories (some may not actually be subdirectories) to
# include in this package
#SUBDIRS(
#  ${VISION_CMAKE_DIR}
#  )

