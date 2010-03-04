

############################ PROJECT NAME
# The name of this cmake project
PROJECT( MOTION )


############################ VERSION
# Check cMake version
CMAKE_MINIMUM_REQUIRED( VERSION 2.2.0 )


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
MESSAGE( STATUS "...:::: Configuration -  MOTION ::::..." )
MESSAGE( STATUS "" )


############################ FIND PACKAGE
# Libraries specifics. Define here cmake packages
# usefull to your project

FIND_PACKAGE( NBCOMMON REQUIRED )
FIND_PACKAGE( PTHREAD  REQUIRED )
FIND_PACKAGE( PYTHON   REQUIRED )
FIND_PACKAGE( BOOST_PYTHON REQUIRED )

IF( APPLE )
  MESSAGE( STATUS "WARNING - Apple OS, some compilation may fail")
 #On MAC OS X, we need to build w/o linking against non-existant ALCOMMON libs
 #when we want to compile just the vision/sensors libraries for the TOOL
  FIND_PACKAGE( ALCOMMON )
ELSE( APPLE )
  FIND_PACKAGE( ALCOMMON REQUIRED )
ENDIF( APPLE )


############################ CONFIG.H GENERATION
CONFIGURE_FILE(
  ${MOTION_CMAKE_DIR}/motionconfig.in
  ${MOTION_INCLUDE_DIR}/motionconfig.h
  ESCAPE_QUOTES
  )


############################ DEFINITIONS
# Add here definition needed.
ADD_DEFINITIONS( ${NBCOMMON_DEFINITIONS}
                 ${ALCOMMON_DEFINITIONS}
                 ${PTHREAD_DEFINITIONS}
                 ${PYTHON_DEFINITIONS}
                 )


############################ INCLUDE DIRECTORY
# Define include directories
INCLUDE_DIRECTORIES( ${NBCOMMON_INCLUDE_DIR}
                     ${ALCOMMON_INCLUDE_DIR}
                     ${PTHREAD_INCLUDE_DIR}
                     ${PYTHON_INCLUDE_DIR}
                     ${MOTION_INCLUDE_DIR}
                     ${CORPUS_INCLUDE_DIR}
                     )


############################ PROJECT SOURCES FILES 
# Add here source files needed to compile this project
SET( MOTION_SRCS
  #${MOTION_INCLUDE_DIR}/CallbackAction
                     ${MOTION_INCLUDE_DIR}/BodyJointCommand
		     ${MOTION_INCLUDE_DIR}/HeadJointCommand
		     ${MOTION_INCLUDE_DIR}/JointCommand
		     ${MOTION_INCLUDE_DIR}/MotionCommand
                     ${MOTION_INCLUDE_DIR}/MotionConstants
                     ${MOTION_INCLUDE_DIR}/Motion.cpp
                     ${MOTION_INCLUDE_DIR}/MotionInterface
		     ${MOTION_INCLUDE_DIR}/MotionSwitchboard
		     ${MOTION_INCLUDE_DIR}/ScriptedProvider
		     ${MOTION_INCLUDE_DIR}/ChoppedCommand
		     ${MOTION_INCLUDE_DIR}/LinearChoppedCommand
		     ${MOTION_INCLUDE_DIR}/SmoothChoppedCommand
		     ${MOTION_INCLUDE_DIR}/BaseFreezeCommand
		     ${MOTION_INCLUDE_DIR}/HeadProvider
		     ${MOTION_INCLUDE_DIR}/ChopShop
		     ${MOTION_INCLUDE_DIR}/NullProvider
		     ${MOTION_INCLUDE_DIR}/WalkProvider
		     ${MOTION_INCLUDE_DIR}/Step
		     ${MOTION_INCLUDE_DIR}/Gait
		     ${MOTION_INCLUDE_DIR}/AbstractGait
		     ${MOTION_INCLUDE_DIR}/MetaGait
		     ${MOTION_INCLUDE_DIR}/SensorAngles
		     ${MOTION_INCLUDE_DIR}/SpringSensor
		     ${MOTION_INCLUDE_DIR}/WalkingLeg
		     ${MOTION_INCLUDE_DIR}/WalkingArm
		     ${MOTION_INCLUDE_DIR}/PreviewController
		     ${MOTION_INCLUDE_DIR}/Observer
		     ${MOTION_INCLUDE_DIR}/ZmpEKF
		     ${MOTION_INCLUDE_DIR}/ZmpAccEKF
		     ${MOTION_INCLUDE_DIR}/StepGenerator
                     ${CORPUS_INCLUDE_DIR}/InverseKinematics
                     ${CORPUS_INCLUDE_DIR}/COMKinematics
                     ${MOTION_INCLUDE_DIR}/PyMotion
                     )

IF( PYTHON_SHARED_MOTION AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
  MESSAGE( STATUS "Looking for extra motion sources in ${MAN_INCLUDE_DIR}" )
  LIST( APPEND MOTION_SRCS ${MAN_INCLUDE_DIR}/Sensors )
ENDIF( PYTHON_SHARED_MOTION AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )

SET( MOTION_PYTHON_SRCS ${MOTION_INCLUDE_DIR}/__init__.py
                        ${MOTION_INCLUDE_DIR}/MotionConstants.py
                        ${MOTION_INCLUDE_DIR}/SweetMoves.py
                        ${MOTION_INCLUDE_DIR}/HeadMoves.py
                        ${MOTION_INCLUDE_DIR}/RobotGaits.py
                        ${MOTION_INCLUDE_DIR}/StiffnessModes.py
                        )

SET( MOTION_PYTHON_SUBDIRS ${MOTION_INCLUDE_DIR}/gaits )


############################ CHECK LIBRARY / EXECUTABLE OPTION
# Configure the output execuatable or shared library, and the install
# commands

GET_TARGET_PROPERTY( MOTION_LOCATION ${MOTION_TARGET} LOCATION )

IF( PYTHON_SHARED_MOTION )
  ADD_LIBRARY(
    ${MOTION_TARGET}
    SHARED
    ${MOTION_SRCS}
    )
  # Install to _motionmodule.so, instead of lib_motion.so
  GET_TARGET_PROPERTY(
    MOTION_TARGET_LOCATION ${MOTION_TARGET} LOCATION
    )
  INSTALL(
    FILES ${MOTION_TARGET_LOCATION}
    DESTINATION ${MOTION_PYTHON_TARGET_DIR}
    CONFIGURATIONS "" Debug Release
    RENAME ${MOTION_TARGET}module.so
    )
ELSE( PYTHON_SHARED_MOTION )
  ADD_LIBRARY(
    ${MOTION_TARGET}
    STATIC
    ${MOTION_SRCS}
    )
  # No INSTALL target when static
ENDIF( PYTHON_SHARED_MOTION )

INSTALL(
  FILES ${MOTION_PYTHON_SRCS}
  DESTINATION ${MOTION_PYTHON_TARGET_DIR}
  CONFIGURATIONS "" Debug Release
  )

INSTALL(
  DIRECTORY ${MOTION_PYTHON_SUBDIRS}
  DESTINATION ${MOTION_PYTHON_TARGET_DIR}/
  CONFIGURATIONS "" Debug Release
  FILES_MATCHING PATTERN "*.py"
  )

############################ LINKER LANGUAGE
# For Mac's, set the target link language to use

#IF( APPLE )
  SET_TARGET_PROPERTIES(
    ${MOTION_TARGET} 
    PROPERTIES LINKER_LANGUAGE CXX
    )
#ENDIF( APPLE )


############################ SET LIBRARIES TO LINK WITH
TARGET_LINK_LIBRARIES(
  ${MOTION_TARGET}
  ${NBCOMMON_LIBRARIES}
  ${ALCOMMON_LIBRARIES}
  ${PTHREAD_LIBRARIES}
  ${PYTHON_LIBRARIES}
  ${SENSORS_TARGET}
  )


############################ (SUB)DIRECTORY COMPILATION
# Set the sudirectories (some may not actually be subdirectories) to
# include in this package
#SUBDIRS(
#  ${CORPUS_CMAKE_DIR}
#  )


