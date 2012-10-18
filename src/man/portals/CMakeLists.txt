
cmake_minimum_required( VERSION 2.8.3 )

############################ BASE DEFINITIONS
# Include  the basic definitions accross all projects
INCLUDE( ../cmake/base_definitions.cmake )

############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project

set( PORTALS_SRCS RoboGrams)

add_library(
  portals
  STATIC
  ${PORTALS_SRCS}
)
