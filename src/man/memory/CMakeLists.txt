

############################ Project Name
# The name of this cmake project
project( Memory )
cmake_minimum_required( VERSION 2.6.0 )


############################ BUILDCONFIG
# Include the config file for this build
INCLUDE( cmake.memory/buildconfig.cmake )


############################ BASE DEFINITIONS
# Include  the basic definitions accross all projects
INCLUDE( ../cmake/base_definitions.cmake )


############################ FIND PACKAGE
# Libraries specifics. Define here cmake packages
# usefull to your project
FIND_PACKAGE( Boost    REQUIRED )


############################ CONFIG.H GENERATION
# Generate the config.h file, inserting values of CMake cache variable

CONFIGURE_FILE(
  cmake.memory/memoryconfig.h.in
  ${CFG_DIR}/memoryconfig.h
    ESCAPE_QUOTES
  )


############################ INCLUDE DIRECTORY
# Define include directories
INCLUDE_DIRECTORIES( ${NBCOMMON_INCLUDE_DIR}/
                     ${BOOST_INCLUDE_DIR}/
                     ${PROJECT_BINARY_DIR}/
                     )


############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project

set( MEMORY_SRCS Memory
                 MVision
                 MVisionSensors
                 MMotionSensors
                 MImage
                 )
############################ (SUB)DIRECTORY COMPILATION
# add the protobuffer C++ source subdirectory
# it's where we house all our generated protobuggers

set( PROTOS_TARGET protos )
set( LOG_TARGET log )
set( PARSE_TARGET parse )

add_subdirectory( protos )
add_subdirectory( log )
add_subdirectory( parse )

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
  ${PARSE_TARGET}
  ${LOG_TARGET}
  ${VISION_TARGET}
  ${SENSORS_TARGET}
  )



