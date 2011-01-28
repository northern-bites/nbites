##############################
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
MESSAGE( STATUS "...:::: Configuration - Protos ::::..." )
MESSAGE( STATUS "" )

############################ FIND PACKAGE
# Libraries specifics.
# cmake packages we use

FIND_PACKAGE( NBCOMMON REQUIRED )
FIND_PACKAGE( PROTOBUF REQUIRED )

############################ DEFINITION
# Add here definition needed.
add_definitions( ${NBCOMMON_DEFINITIONS}
                 -Wno-write-strings )


############################ INCLUDE DIRECTORY
# Define include directories
include_directories( ${NBCOMMON_INCLUDE_DIR}/
                     ${PROTOBUF_INCLUDE_DIR}/
                     ${MEMORY_INCLUDE_DIR}/
                     ${MEMORY_CONFIG_DIR}/
                     )

########################### PROTOBUF GENERATION

#protobuf input - finds all .proto files in the current dir
file(GLOB PROTO_INPUT
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "*.proto")

#protobuf compiler
find_program(PROTOC protoc)
set(PROTOC_C_OUT_FLAG --cpp_out)

#protobuf output
set(PROTO_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR})
foreach(PROTO_FILE ${PROTO_INPUT})
  #get the name of the file without extension
  get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)
  #add the generated files
  set(PROTO_GEN ${PROTO_GEN}
  ${PROTO_GEN_DIR}/${PROTO_NAME}.pb.h
  ${PROTO_GEN_DIR}/${PROTO_NAME}.pb.cc)
endforeach(PROTO_FILE ${PROTO_INPUT})

# add the custom command that will generate all the files
add_custom_command(
  OUTPUT ${PROTO_GEN}
  COMMAND ${PROTOC} ${PROTO_INPUT} ${PROTOC_C_OUT_FLAG} ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS ${PROTOC} ${PROTO_INPUT}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )

############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project
set( PROTOS_SRCS ${PROTO_GEN} )

############################ Add sources

# Add static library target
add_library(
  ${PROTOS_TARGET}
  STATIC
  ${PROTOS_SRCS}
  )

target_link_libraries(
  ${PROTOS_TARGET}
  ${PROTOBUF_LIBRARIES}
  )
