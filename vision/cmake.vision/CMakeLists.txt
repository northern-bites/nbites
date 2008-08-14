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
PROJECT( VISION )


############################ VERSION
# Check cMake version
CMAKE_MINIMUM_REQUIRED( VERSION 2.2.0 )


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
MESSAGE( STATUS "...:::: Configuration -  VISION ::::..." )
MESSAGE( STATUS "" )


############################ FIND PACKAGE
# Libraries specifics. Define here cmake packages
# usefull to your project

FIND_PACKAGE( NBCOMMON REQUIRED )
FIND_PACKAGE( PYTHON   REQUIRED )


############################ TESTS
# Define main test script location
#SET( TEST_RUN "${AL_DIR}/extern/python/aldebaran/test_run.py" )

#<ADD_TEST> don't remove this comment
### test example ADD_TEST(My_test python ${TEST_RUN} -p ${NAOQI} -s ${CMAKE_CURRENT_SOURCE_DIR}/test/myRubyTest.rb)

#</ADD_TEST> don't remove this comment 


############################ CONFIG.H GENERATION
# Generate the config.h file, inserting values of CMake cache variable
CONFIGURE_FILE(
  "${VISION_CMAKE_DIR}/profileconfig.h.in"
  "${VISION_INCLUDE_DIR}/profileconfig.h"
    ESCAPE_QUOTES
  )

CONFIGURE_FILE(
  "${VISION_CMAKE_DIR}/visionconfig.h.in"
  "${VISION_INCLUDE_DIR}/visionconfig.h"
    ESCAPE_QUOTES
  )


############################ DEFINITION
# Add here definition needed.
ADD_DEFINITIONS( ${NBCOMMON_DEFINITIONS} ${PYTHON_DEFINITIONS}
                 -Wno-write-strings )


############################ INCLUDE DIRECTORY
# Define include directories
INCLUDE_DIRECTORIES( ${NBCOMMON_INCLUDE_DIR}
                     ${PYTHON_INCLUDE_DIR}
                     ${VISION_INCLUDE_DIR}
                     )


############################ PROJECT SOURCES FILES 
# Add here source files needed to compile this project
SET( VISION_SRCS ${VISION_INCLUDE_DIR}/Ball
                 ${VISION_INCLUDE_DIR}/ConcreteCorner
                 ${VISION_INCLUDE_DIR}/ConcreteLandmark
                 ${VISION_INCLUDE_DIR}/FieldLines
                 ${VISION_INCLUDE_DIR}/FieldObjects
                 ${VISION_INCLUDE_DIR}/ObjectFragments
                 ${VISION_INCLUDE_DIR}/Pose
                 ${VISION_INCLUDE_DIR}/PoseMatrix
                 ${VISION_INCLUDE_DIR}/PotentialField
                 ${VISION_INCLUDE_DIR}/potentialfieldmodule
                 ${VISION_INCLUDE_DIR}/Profiler
                 ${VISION_INCLUDE_DIR}/PyVision
                 ${VISION_INCLUDE_DIR}/Threshold
                 ${VISION_INCLUDE_DIR}/Utility
                 ${VISION_INCLUDE_DIR}/Vision
                 ${VISION_INCLUDE_DIR}/VisualCorner
                 ${VISION_INCLUDE_DIR}/VisualLandmark
                 ${VISION_INCLUDE_DIR}/VisualLine
                 #${VISION_INCLUDE_DIR}/Zlib
                 ${VISION_INCLUDE_DIR}/matrix
                 ${VISION_INCLUDE_DIR}/_matrixmodule
                 )

SET( VISION_PYTHON_SRCS ${VISION_INCLUDE_DIR}/__init__.py )

SET( MATRIX_SRCS ${VISION_INCLUDE_DIR}/matrix
                 ${VISION_INCLUDE_DIR}/_matrixmodule
                 )


############################ CHECK LIBRARY / EXECUTABLE OPTION
# Configure the output execuatable or shared library, and the install
# commands

IF( PYTHON_SHARED_VISION )
  # Add shared library target
  ADD_LIBRARY(
    ${VISION_TARGET}
    SHARED
    ${VISION_SRCS}
    )
  ADD_LIBRARY(
    ${MATRIX_TARGET}
    SHARED
    ${MATRIX_SRCS}
    )
  # Install to XXXvisionmodule.so, instead of libvision.so
  GET_TARGET_PROPERTY( VISION_TARGET_LOCATION ${VISION_TARGET} LOCATION )
  INSTALL(
    FILES ${VISION_TARGET_LOCATION}
    DESTINATION ${VISION_PYTHON_TARGET_DIR}
    CONFIGURATIONS "" Debug Release
    RENAME ${ROBOT_PREFIX}${VISION_TARGET}module.so
    )
  GET_TARGET_PROPERTY( MATRIX_TARGET_LOCATION ${MATRIX_TARGET} LOCATION )
  INSTALL(
    FILES ${MATRIX_TARGET_LOCATION}
    DESTINATION ${VISION_PYTHON_TARGET_DIR}
    CONFIGURATIONS "" Debug Release
    RENAME ${MATRIX_TARGET}module.so
    )
ELSE( PYTHON_SHARED_VISION )
  # Add static library target
  ADD_LIBRARY(
    ${VISION_TARGET}
    STATIC
    ${VISION_SRCS}
    )
ENDIF( PYTHON_SHARED_VISION )

INSTALL(
  FILES ${VISION_PYTHON_SRCS}
  DESTINATION ${VISION_PYTHON_TARGET_DIR}
  CONFIGURATIONS "" Debug Release
  )

############################ SET LIBRARIES TO LINK WITH
TARGET_LINK_LIBRARIES( ${VISION_TARGET} ${PYTHON_LIBRARIES} z )


############################ (SUB)DIRECTORY COMPILATION
# Set the sudirectories (some may not actually be subdirectories) to
# include in this package
#SUBDIRS(
#  ${CORPUS_CMAKE_DIR}
#  )

