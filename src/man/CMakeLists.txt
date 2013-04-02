project( man )
cmake_minimum_required( VERSION 2.8.3 )

# Make sure we can find the shared files
set( SHARE_DIR "${NBITES_DIR}/src/share/" )
# So that we can use find_package commands
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SHARE_DIR}/cmake/" )

# A list of options that we do want
include( cmake/RobotConfig.cmake )
# A list of options for debugging various modules
include( cmake/DebugConfig.cmake)
# A list of options that should be advanced
include( cmake/Advanced.cmake )
# Compiler flags
include( cmake/Flags.cmake )

# Configure the robot options
configure_file(
  "${CMAKE_CURRENT_LIST_DIR}/cmake/RobotConfig.in"
  "${PROJECT_BINARY_DIR}/RobotConfig.h"
)

# Configure the debug options
configure_file(
  "${CMAKE_CURRENT_LIST_DIR}/cmake/DebugConfig.in"
  "${PROJECT_BINARY_DIR}/DebugConfig.h"
)

# Configure the copy script based on our settings
configure_file(
  "${CMAKE_CURRENT_LIST_DIR}/cmake/upload.sh.in"
  "${PROJECT_BINARY_DIR}/upload.sh"
)

include_directories( ${PROJECT_BINARY_DIR} )

# Option to turn rebuilding our protobuf libs on/off
option( BUILD_PROTOBUF "Rebuild the protobuf libraries." OFF )
if( BUILD_PROTOBUF )
  # Only build them if the user wants to
  include( ${SHARE_DIR}/cmake/BuildProtobuf.cmake )
endif()

# testing set up
if ( OFFLINE )
  include ( ${SHARE_DIR}/cmake/gtest.cmake )
  enable_testing()
  set(CTEST_OUTPUT_ON_FAILURE TRUE)

  include_directories(
    ${GTEST_INCLUDE_DIR}
    )
endif()

# For Aldebaran's headers
if ( NOT OFFLINE )
  find_package(ALCOMMON REQUIRED)
  include_directories( ${ALCOMMON_INCLUDE_DIR} )
endif()

find_package( PROTOBUF REQUIRED )
include_directories( SYSTEM ${PROTOBUF_INCLUDE_DIR} )

# Our core includes come from here
include_directories( ${SHARE_DIR}/include )

# Compile portals, which is outside the source tree so specify a destination
add_subdirectory( ${SHARE_DIR}/portals ${CMAKE_CURRENT_BINARY_DIR}/portals/ )
# Make portals available to everything
include_directories( ${SHARE_DIR}/portals )
# Also messages
add_subdirectory( ${SHARE_DIR}/messages ${CMAKE_CURRENT_BINARY_DIR}/messages/ )
# And make sure generated pb.h files are visible
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/messages/ )
include_directories( ${SHARE_DIR}/messages )

# If protobuf libs don't exist yet, make sure they build before man
if( BUILD_PROTOBUF )
add_dependencies(messages protobuf_libs)
endif()

# Actual subdirectories
add_subdirectory( common )
include_directories( common )

# Actual modules
add_subdirectory( image )
add_subdirectory( log )
add_subdirectory( vision )
add_subdirectory( sensors )
add_subdirectory( comm )
add_subdirectory( guardian )
add_subdirectory( audio )

set( MAN_SRCS main.cpp
  Man.cpp
  DiagramThread.cpp
)

add_library(
  man
  SHARED
  ${MAN_SRCS}
)

target_link_libraries(
  man
  portals
  messages
  log
  sensors
  comm
  image
  vision
  guardian
  audio
  common
)

install( TARGETS man
  LIBRARY DESTINATION lib
)
