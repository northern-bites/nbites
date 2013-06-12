# Main CMake file for the Tool!
project( tool )
cmake_minimum_required( VERSION 2.8.3 )

# Make sure we can find the shared files
set( SHARE_DIR "${NBITES_DIR}/src/share/" )
set( MAN_DIR "${NBITES_DIR}/src/man" )
include_directories( ${MAN_DIR} )

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

include( ${SHARE_DIR}/cmake/DebugConfig.cmake)

# Configure the debug options
configure_file(
  "${SHARE_DIR}/cmake/DebugConfig.in"
  "${PROJECT_BINARY_DIR}/DebugConfig.h"
)

configure_file(
  "${CMAKE_CURRENT_LIST_DIR}/PathConfig.in"
  "${PROJECT_BINARY_DIR}/PathConfig.h"
)

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

# If protobuf libs don't exist yet, make sure they build before man
if( BUILD_PROTOBUF )
add_dependencies(messages protobuf_libs)
endif()

# Man modules that we need to use
include_directories( ${MAN_DIR}/support )
add_subdirectory( ${MAN_DIR}/support ${CMAKE_CURRENT_BINARY_DIR}/man/support)

add_subdirectory( ${MAN_DIR}/image ${CMAKE_CURRENT_BINARY_DIR}/man/image)
add_subdirectory( ${MAN_DIR}/vision ${CMAKE_CURRENT_BINARY_DIR}/man/vision)
add_subdirectory( ${MAN_DIR}/localization ${CMAKE_CURRENT_BINARY_DIR}/man/localization)

# Actual subdirectories!
add_subdirectory( ${MAN_DIR}/comm ${CMAKE_CURRENT_BINARY_DIR}/man/comm)
add_subdirectory( unlog )
add_subdirectory( logview )
add_subdirectory( image )
add_subdirectory( colorcreator )
add_subdirectory( vision_offline )
add_subdirectory( fieldview )
add_subdirectory( common )
add_subdirectory( worldview )
add_subdirectory( calibrator )

## SOURCES ##

# QTool Sources
set( tool_srcs main.cpp
  ToolDiagram.cpp
  DataSelector.cpp
  Tool.cpp
  ${MAN_DIR}/DiagramThread.cpp
  )

set( tool_headers
  ToolDiagram.h
  DataSelector.h

  Tool.h
)

# LocTool Sources
set( loc_srcs loc_main.cpp
  EmptyTool.cpp
  ToolDiagram.cpp
  DataSelector.cpp

  LocTool.cpp
  )

set( loc_headers
  EmptyTool.h
  ToolDiagram.h
  DataSelector.h

  LocTool.h
)

# VisionTool Sources
set( vision_srcs vision_main.cpp
  EmptyTool.cpp
  ToolDiagram.cpp
  DataSelector.cpp

  VisionTool.cpp
  )

set( vision_headers
  EmptyTool.h
  ToolDiagram.h
  DataSelector.h

  VisionTool.h
)

# CalibrationTool Sources
set( calibrate_srcs calibrate_main.cpp
  EmptyTool.cpp
  ToolDiagram.cpp
  DataSelector.cpp

  CalibrationTool.cpp
  )

set( calibrate_headers
  EmptyTool.h
  ToolDiagram.h
  DataSelector.h

  CalibrationTool.h
)

# Qt4 goodness (aka moc the headers)
Qt4_wrap_cpp( tool_headers_moc ${tool_headers} )
Qt4_wrap_cpp( loc_headers_moc ${loc_headers} )
Qt4_wrap_cpp( vision_headers_moc ${vision_headers} )
Qt4_wrap_cpp( calibrate_headers_moc ${calibrate_headers} )

## EXECUTABLES ##

# The QTool executable
add_executable( tool
  ${tool_srcs}
  ${tool_headers_moc} )

target_link_libraries(
  tool
  portals
  messages
  unlog
  logview
  image_display
  image # man image
  vision # man vision
  comm # man comm
  fieldview
  tool_common
  colorcreator
  vision_offline
  worldview
  ${QT_LIBRARIES}
  )

# The LocTool executable
add_executable( loc_tool
  ${loc_srcs}
  ${loc_headers_moc} )

target_link_libraries(
  loc_tool
  portals
  messages
  unlog
  image_display
  logview
  fieldview
  tool_common
  worldview
  localization
  ${QT_LIBRARIES}
  )

# The VisionTool executable
add_executable( vision_tool
  ${vision_srcs}
  ${vision_headers_moc} )

target_link_libraries(
  vision_tool
  portals
  messages
  unlog
  logview
  image_display
  image # man image
  image_display
  fieldview
  image_display
  tool_common
  colorcreator
  vision_offline
  ${QT_LIBRARIES}
  )

# The CalibrationTool executable
add_executable( calibrate_tool
  ${calibrate_srcs}
  ${calibrate_headers_moc}
)

target_link_libraries(
  calibrate_tool
  portals
  messages
  unlog
  logview
  image_display
  calibrator
  ${QT_LIBRARIES}
)