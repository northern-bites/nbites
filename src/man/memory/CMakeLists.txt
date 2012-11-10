

############################ Project Name
# The name of this cmake project
project( Memory )
cmake_minimum_required( VERSION 2.8.3 )


############################ BUILDCONFIG
# Include the config file for this build
INCLUDE( cmake.memory/buildconfig.cmake )


############################ BASE DEFINITIONS
# Include  the basic definitions accross all projects
INCLUDE( ../cmake/base_definitions.cmake )


############################ FIND PACKAGE
# Libraries specifics. Define here cmake packages
# usefull to your project
FIND_PACKAGE( Boost COMPONENTS python REQUIRED )
FIND_PACKAGE( PythonLibs REQUIRED )

############################ CONFIG.H GENERATION
# Generate the config.h file, inserting values of CMake cache variable

CONFIGURE_FILE(
  cmake.memory/memoryconfig.h.in
  ${CFG_DIR}/memoryconfig.h
    ESCAPE_QUOTES
  )


############################ INCLUDE DIRECTORY
# Define include directories
INCLUDE_DIRECTORIES( ${NBCOMMON_INCLUDE_DIR}
                     ${PROTOBUF_INCLUDE_DIR}
                     ${Boost_INCLUDE_DIR}
                     ${PROJECT_BINARY_DIR}
                     )


############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project

set( MEMORY_SRCS RobotMemory )

set( PARSE_SRCS parse/MessageParser
                parse/ParsingBoard )

set( LOG_SRCS log/MessageLogger
              log/LoggingBoard
              log/OutputProviderFactory
              log/PyLoggingBoard)
############################ (SUB)DIRECTORY COMPILATION
# add the protobuffer C++ source subdirectory
# it's where we house all our generated protobuggers

set( PROTOS_TARGET protos )
set( LOG_TARGET log )
set( PARSE_TARGET parse )

add_subdirectory( protos )

############################ CHECK LIBRARY / EXECUTABLE OPTION
# Configure the output executable or shared library, and the install
# commands

# Add static library target
add_library(
  ${MEMORY_TARGET}
  STATIC
  ${MEMORY_SRCS}
  )

add_library(
  ${LOG_TARGET}
  STATIC
  ${LOG_SRCS}
  )

add_library(
  ${PARSE_TARGET}
  STATIC
  ${PARSE_SRCS}
  )

############################ Set linked libraries
target_link_libraries(
  ${MEMORY_TARGET}
  ${PROTOBUF_LIBRARY}
  ${PROTOS_TARGET}
  )

target_link_libraries(
    ${LOG_TARGET}
    ${PROTOBUF_LIBRARY}
    ${Boost_PYTHON_LIBRARY}
    ${PYTHON_LIBRARY}
    ${NBINCLUDE_TARGET}
    )

target_link_libraries(
    ${PARSE_TARGET}
    ${PROTOBUF_LIBRARY}
    )
