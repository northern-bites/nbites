# Main CMake file for the Tool!
project( tool )
cmake_minimum_required( VERSION 2.8.3 )

# Make sure we can find the shared files
set( SHARE_DIR "${NBITES_DIR}/src/share/" )
set( MAN_DIR "${NBITES_DIR}/src/man" )
set( OFFLINE ON )
include_directories( ${MAN_DIR})

# So that we can use find_package commands
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SHARE_DIR}/cmake/" )
mark_as_advanced( CMAKE_INSTALL_PREFIX )
mark_as_advanced( CMAKE_TOOLCHAIN_FILE )

# Option to turn rebuilding our protobuf libs on/off
option( BUILD_PROTOBUF "Rebuild the protobuf libraries." OFF )
if( BUILD_PROTOBUF )
  # Only build them if the user wants to
  include( ${SHARE_DIR}/cmake/BuildProtobuf.cmake )
endif()

include( ${MAN_DIR}/cmake/RobotConfig.cmake )

# If protobuf libs don't exist yet, make sure they build before man
if( BUILD_PROTOBUF )
add_dependencies(messages protobuf_libs)
endif()

configure_file(
  "${CMAKE_CURRENT_LIST_DIR}/PathConfig.in"
  "${PROJECT_BINARY_DIR}/PathConfig.h"
)
# Configure the robot options
configure_file(
  "${MAN_DIR}/cmake/RobotConfig.in"
  "${PROJECT_BINARY_DIR}/RobotConfig.h"
)
include_directories( ${PROJECT_BINARY_DIR} )
include_directories( ${CMAKE_SOURCE_DIR} )

# Dependencies
find_package( Qt4 REQUIRED )

include( ${QT_USE_FILE} )
add_definitions( ${QT_DEFINITIONS} )

mark_as_advanced( QT_QMAKE_EXECUTABLE )

find_package( PROTOBUF REQUIRED )
include_directories( SYSTEM ${PROTOBUF_INCLUDE_DIR} )

# Our core includes come from here
include_directories( ${SHARE_DIR}/include )

# Compile portals into portals folder
add_subdirectory( ${SHARE_DIR}/portals ${CMAKE_CURRENT_BINARY_DIR}/portals )
include_directories( ${SHARE_DIR}/portals )

# Compile messages into messages folder
add_subdirectory( ${SHARE_DIR}/messages ${CMAKE_CURRENT_BINARY_DIR}/messages )
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/messages )
include_directories( ${SHARE_DIR}/messages )

# Man modules that we need to use
include_directories( ${MAN_DIR}/support )
add_subdirectory( ${MAN_DIR}/support ${CMAKE_CURRENT_BINARY_DIR}/man/support)

add_subdirectory( ${MAN_DIR}/image ${CMAKE_CURRENT_BINARY_DIR}/man/image)
add_subdirectory( ${MAN_DIR}/vision ${CMAKE_CURRENT_BINARY_DIR}/man/vision)

# Actual subdirectories!
add_subdirectory( unlog )
add_subdirectory( logview )
add_subdirectory( image )
add_subdirectory( colorcreator )
add_subdirectory( vision_offline )

# Sources
set( tool_srcs main.cpp
  Tool.cpp
  ToolDiagram.cpp
  DataSelector.cpp
  )

set( tool_headers
  Tool.h
  ToolDiagram.h
  DataSelector.h
)

# Qt4 goodness
Qt4_wrap_cpp( tool_headers_moc ${tool_headers} )

# The main executable
add_executable( ${TOOL_EXECUTABLE}
  ${tool_srcs}
  ${tool_headers_moc} )

target_link_libraries(
  ${TOOL_EXECUTABLE}
  portals
  messages
  unlog
  logview
  image_display
  image # man image
  vision # man vision
  colorcreator
  vision_offline
  ${QT_LIBRARIES}
  )
