

############################ Version
# Check cMake version
cmake_minimum_required( VERSION 2.6.0 )

############################ FIND PACKAGE
# Libraries specifics.
# cmake packages we use
FIND_PACKAGE( Protobuf REQUIRED )

############################ INCLUDE DIRECTORY
# Define include directories
include_directories( ${PROTOBUF_INCLUDE_DIRS}/
                     )

############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project
set( LOG_SRCS CodedFileLogger.cpp
              ImageFDLogger.cpp
              LoggingBoard.cpp)

############################ Add sources
# Add static library target
add_library(
  ${LOG_TARGET}
  STATIC
  ${LOG_SRCS}
  )

target_link_libraries(
    ${LOG_TARGET}
    ${PROTOBUF_LIBRARIES}
    ${NBINCLUDE_TARGET}
    ${MEMORY_TARGET}
    )
