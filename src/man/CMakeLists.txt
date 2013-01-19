project( man )
cmake_minimum_required( VERSION 2.8.3 )

# A list of options that we do want
include( cmake/RobotConfig.cmake )
# A list of options for debugging various modules
include( cmake/DebugConfig.cmake)
# A list of options that should be advanced
include( cmake/Advanced.cmake )
# Compiler flags
include( cmake/Flags.cmake )

# Configure the debug options
configure_file(
  "${CMAKE_CURRENT_LIST_DIR}/cmake/DebugConfig.in"
  "${PROJECT_BINARY_DIR}/DebugConfig.h"
)

# Option to turn rebuilding our protobuf libs on/off
option( BUILD_PROTOBUF "Rebuild the protobuf libraries." OFF )
if( BUILD_PROTOBUF )
  # Only build them if the user wants to
  include( cmake/BuildProtobuf.cmake )
endif()

# So that we can use find_package commands
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/" )

# For Aldebaran's headers
find_package( ALCOMMON REQUIRED )
include_directories( ${ALCOMMON_INCLUDE_DIR} )

find_package( PROTOBUF REQUIRED )
include_directories( SYSTEM ${PROTOBUF_INCLUDE_DIR} )

# Configure the copy script based on our settings
configure_file(
  "${CMAKE_CURRENT_LIST_DIR}/cmake/upload.sh.in"
  "${PROJECT_BINARY_DIR}/upload.sh"
)

set( MAN_SRCS main.cpp
  NewMan.cpp
  Threads.cpp
)

# Compile portals, which is outside the source tree so specify a destination
add_subdirectory( ../portals ${CMAKE_CURRENT_BINARY_DIR}/portals/ )
# Make portals available to everything
include_directories( ../portals )
# Also messages
add_subdirectory( ../messages ${CMAKE_CURRENT_BINARY_DIR}/messages/ )
# And make sure generated pb.h files are visible
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/messages/ )
include_directories( ../messages )

# Actual subdirectories
add_subdirectory( common )
include_directories( common )
add_subdirectory( synchro )

# Actual modules
add_subdirectory( image )
add_subdirectory( log )

add_subdirectory( comm )

add_library(
  man
  SHARED
  ${MAN_SRCS}
)

# If protobuf libs don't exist yet, make sure they build before man
if( BUILD_PROTOBUF )
add_dependencies(messages protobuf_libs)
endif()

target_link_libraries(
  man
  portals
  messages
  comm
  synchro
  image
  log
)

install( TARGETS man
  LIBRARY DESTINATION lib
)
