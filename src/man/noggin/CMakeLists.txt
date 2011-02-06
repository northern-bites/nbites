

############################ PROJECT NAME
# The name of this cmake project
PROJECT( NOGGIN )


############################ CMAKE VERSION
# Check cMake version
CMAKE_MINIMUM_REQUIRED( VERSION 2.6.0 )


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


############################ CMAKE PACKAGE DIRECTORY
# Set cmakeModules folder
SET( CMAKE_MODULE_PATH $ENV{TRUNK_PATH}/cmake )


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


############################ BUILDCONFIG
# Include the config file for this build
INCLUDE( ${NOGGIN_CMAKE_DIR}/buildconfig.cmake )


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
  ${NOGGIN_CMAKE_DIR}/nogginconfig.in
  ${CFG_DIR}/nogginconfig.h
  ESCAPE_QUOTES
  )
CONFIGURE_FILE(
  ${NOGGIN_CMAKE_DIR}/Switch.py.in
  players/Switch.py
  )
CONFIGURE_FILE(
  ${NOGGIN_CMAKE_DIR}/TeamConfig.py.in
  TeamConfig.py
  )

CONFIGURE_FILE(
  ${NOGGIN_CMAKE_DIR}/WebotsConfig.py.in
  WebotsConfig.py
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
                     ${CFG_DIR}
                     )


############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project

SET( NOGGIN_SRCS Noggin
                 Observation
                 # MCL
                 BallEKF
                 PyLoc
                 LocEKF
                 MMLocEKF
                 NogginStructs.h
                 )


SET( NOGGIN_PYTHON_SRCS __init__.py
                        Brain.py
                        GameController.py
                        GameStates.py
                        FallController.py
                        FallStates.py
                        NogginConstants.py
                        Leds.py
                        TeamConfig.py
                        WebotsConfig.py
                        )

SET( NOGGIN_PYTHON_SUBDIRS headTracking
     			   kickDecider
                           navigator
                           playbook
                           players
                           robots
                           typeDefs
                           util )


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

