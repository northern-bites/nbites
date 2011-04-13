

############################ Version
# Check cMake version
cmake_minimum_required( VERSION 2.6.0 )


############################ FIND PACKAGE
# Libraries specifics.
# cmake packages we use
FIND_PACKAGE( Protobuf REQUIRED )


############################ INCLUDE DIRECTORY
# Define include directories
include_directories( ${PROTOBUF_INCLUDE_DIR}/
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
    ${PROTOBUF_LIBRARY}
    )
