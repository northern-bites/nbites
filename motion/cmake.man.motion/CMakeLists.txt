

############################ PROJECT NAME
# The name of this cmake project
PROJECT( MAN_MOTION )


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
MESSAGE( STATUS "...:::: Configuration -  MAN_MOTION ::::..." )
MESSAGE( STATUS "" )


############################ FIND PACKAGE
# Libraries specifics. Define here cmake packages
# usefull to your project

FIND_PACKAGE( NBCOMMON REQUIRED )
FIND_PACKAGE( ALCOMMON REQUIRED )
FIND_PACKAGE( PTHREAD  REQUIRED )
FIND_PACKAGE( PYTHON   REQUIRED )
FIND_PACKAGE( TINYXML  REQUIRED )


############################ CONFIG.H GENERATION
CONFIGURE_FILE(
  ${MAN_MOTION_CMAKE_DIR}/motionconfig.in
  ${MAN_MOTION_INCLUDE_DIR}/motionconfig.h
  ESCAPE_QUOTES
  )


############################ DEFINITIONS
# Add here definition needed.
ADD_DEFINITIONS( ${NBCOMMON_DEFINITIONS}
                 ${ALCOMMON_DEFINITIONS}
		 ${TINYXML_DEFINITIONS}
                 ${PTHREAD_DEFINITIONS}
                 ${PYTHON_DEFINITIONS}
                 )


############################ INCLUDE DIRECTORY
# Define include directories
INCLUDE_DIRECTORIES( ${NBCOMMON_INCLUDE_DIR}
                     ${ALCOMMON_INCLUDE_DIR}
		     ${TINYXML_INCLUDE_DIR}
                     ${PTHREAD_INCLUDE_DIR}
                     ${PYTHON_INCLUDE_DIR}
                     ${MAN_MOTION_INCLUDE_DIR}
                     ${CORPUS_INCLUDE_DIR}
                     )


############################ PROJECT SOURCES FILES 
# Add here source files needed to compile this project
SET( MAN_MOTION_SRCS ${MAN_MOTION_INCLUDE_DIR}/ActionCommand
                     ${MAN_MOTION_INCLUDE_DIR}/CallbackAction
                     ${MAN_MOTION_INCLUDE_DIR}/BodyJointCommand
		     ${MAN_MOTION_INCLUDE_DIR}/HeadJointCommand
		     ${MAN_MOTION_INCLUDE_DIR}/WalkParameters
                     ${MAN_MOTION_INCLUDE_DIR}/MotionConstants
                     ${MAN_MOTION_INCLUDE_DIR}/Motion.cpp
                     ${MAN_MOTION_INCLUDE_DIR}/MotionCore
                     ${MAN_MOTION_INCLUDE_DIR}/MotionInterface
                     ${MAN_MOTION_INCLUDE_DIR}/_motionmodule
                     )

IF( PYTHON_SHARED_MAN_MOTION AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
  LIST( APPEND MAN_MOTION_SRCS ${MAN_INCLUDE_DIR}/Sensors )
ENDIF( PYTHON_SHARED_MAN_MOTION AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )

SET( MAN_MOTION_PYTHON_DIRS ${MAN_MOTION_INCLUDE_DIR}/motion )

SET( MAN_MOTION_TEST_DIR ${MAN_MOTION_INCLUDE_DIR}/test )


############################ CHECK LIBRARY / EXECUTABLE OPTION
# Configure the output execuatable or shared library, and the install
# commands

GET_TARGET_PROPERTY( MAN_MOTION_LOCATION ${MAN_MOTION_TARGET} LOCATION )

IF( ROBOT_NAO )
  IF( PYTHON_SHARED_MAN_MOTION )
    ADD_LIBRARY(
      ${MAN_MOTION_TARGET}
      SHARED
      ${MAN_MOTION_SRCS}
      )
    # Install to _motionmodule.so, instead of lib_motion.so
    GET_TARGET_PROPERTY(
      MAN_MOTION_TARGET_LOCATION ${MAN_MOTION_TARGET} LOCATION
      )
    INSTALL(
      FILES ${MAN_MOTION_TARGET_LOCATION}
      DESTINATION ${OUTPUT_ROOT_DIR_LIB}
      CONFIGURATIONS Debug
      RENAME ${MAN_MOTION_TARGET}module.so
      )
  ELSE( PYTHON_SHARED_MAN_MOTION )
    ADD_LIBRARY(
      ${MAN_MOTION_TARGET}
      STATIC
      ${MAN_MOTION_SRCS}
      )
    # No INSTALL target when static
  ENDIF( PYTHON_SHARED_MAN_MOTION )
ENDIF( ROBOT_NAO )
INSTALL(
  DIRECTORY ${MAN_MOTION_PYTHON_DIRS}
  DESTINATION ${OUTPUT_ROOT_DIR_LIB}
  CONFIGURATIONS "" Debug Release
  PATTERN .svn EXCLUDE
  )
INSTALL(
  FILES ${MAN_MOTION_TEST_DIR}/test_motion.py
  DESTINATION ${OUTPUT_ROOT_DIR_LIB}
  CONFIGURATIONS Debug
  )


############################ LINKER LANGUAGE
# For Mac's, set the target link language to use

#IF( APPLE )
  SET_TARGET_PROPERTIES(
    ${MAN_MOTION_TARGET} 
    PROPERTIES LINKER_LANGUAGE CXX
    )
#ENDIF( APPLE )


############################ SET LIBRARIES TO LINK WITH
TARGET_LINK_LIBRARIES(
  ${MAN_MOTION_TARGET}
  ${NBCOMMON_LIBRARIES}
  ${ALCOMMON_LIBRARIES}
  ${TINYXML_LIBRARIES}
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


