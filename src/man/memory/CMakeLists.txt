##############################
#
# WELCOME
#
#

############################ Project Name
# The name of this cmake project
project( Memory )


############################ Version
# Check cMake version
cmake_minimum_required( VERSION 2.6.0 )


############################ Colorize Makefile
# Colorized output
SET( CMAKE_COLOR_MAKEFILE TRUE )


############################ Set config file dir
SET( MEMORY_CONFIG_DIR ${CMAKE_CURRENT_BINARY_DIR}/)


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
INCLUDE( cmake.memory/buildconfig.cmake )


############################ BASE DEFINITIONS
# Include  the basic definitions accross all projects
INCLUDE( ${CMAKE_MODULE_PATH}/base_definitions.cmake )


############################ DISPLAY
# Display summary of options

MESSAGE( STATUS "" )
MESSAGE( STATUS "...:::: Configuration -  Memory ::::..." )
MESSAGE( STATUS "" )


############################ FIND PACKAGE
# Libraries specifics. Define here cmake packages
# usefull to your project

FIND_PACKAGE( NBCOMMON REQUIRED )
FIND_PACKAGE( BOOTS    REQUIRED )
FIND_PACKAGE( PROTOBUF REQUIRED )


############################ CONFIG.H GENERATION
# Generate the config.h file, inserting values of CMake cache variable

CONFIGURE_FILE(
  cmake.memory/memoryconfig.h.in
  ${MEMORY_CONFIG_DIR}/memoryconfig.h
    ESCAPE_QUOTES
  )


############################ DEFINITION
# Add here definition needed.
ADD_DEFINITIONS( ${NBCOMMON_DEFINITIONS}
                 -Wno-write-strings )


############################ INCLUDE DIRECTORY
# Define include directories

INCLUDE_DIRECTORIES( ${NBCOMMON_INCLUDE_DIR}
                     ${PROTOBUF_INCLUDE_DIR}
                     ${MEMORY_INCLUDE_DIR}
                     ${BOOST_INCLUDE_DIR}
                     ${MEMORY_CONFIG_DIR}/
                     )


############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project

set( MEMORY_SRCS Memory
                 MVision
                                  )
############################ (SUB)DIRECTORY COMPILATION
# add the protobuffer C++ source subdirectory
# it's where we house all our generated protobuggers

set( PROTOS_TARGET protos )
set( LOGGER_TARGET logger )

add_subdirectory( gen )
add_subdirectory( logger )

############################ CHECK LIBRARY / EXECUTABLE OPTION
# Configure the output executable or shared library, and the install
# commands

  # Add static library target
  add_library(
    ${MEMORY_TARGET}
    STATIC
    ${MEMORY_SRCS}
    )

############################ Set linked libraries
target_link_libraries(
  ${MEMORY_TARGET}
  ${PROTOS_TARGET}
  ${LOGGER_TARGET}
  )



