#
# Welcome
#

############################ Version
# Check cMake version
cmake_minimum_required( VERSION 2.6.0 )

############################ Colorize Makefile
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


############################ Base definitions
# Include  the basic definitions accross all projects
INCLUDE( ${CMAKE_MODULE_PATH}/base_definitions.cmake )

############################ DISPLAY
# Display summary of options

MESSAGE( STATUS "" )
MESSAGE( STATUS "...:::: Configuration - Logger ::::..." )
MESSAGE( STATUS "" )

############################ FIND PACKAGE
# Libraries specifics.
# cmake packages we use

FIND_PACKAGE( NBCOMMON REQUIRED )

############################ DEFINITION
# Add here definition needed.
add_definitions( ${NBCOMMON_DEFINITIONS}
                 -Wno-write-strings )


############################ INCLUDE DIRECTORY
# Define include directories
include_directories( ${NBCOMMON_INCLUDE_DIR}/
                     ${MEMORY_CONFIG_DIR}/
                     )

############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project
set( LOGGER_SRCS FileLogger.cpp )

############################ Add sources

  # Add static library target
  add_library(
    ${LOGGER_TARGET}
    STATIC
    ${LOGGER_SRCS}
    )

target_link_libraries(
    ${LOGGER_TARGET}
    )
