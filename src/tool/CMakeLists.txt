# Main CMake file for the Tool!
project( tool )
cmake_minimum_required( VERSION 2.8.3 )

include( ../cmake/FindNBITES_DIR.cmake )
# So that we can use find_package commands
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${NBITES_DIR}/src/cmake/" )

# Dependencies
find_package( Qt4 REQUIRED )

include( ${QT_USE_FILE} )
add_definitions( ${QT_DEFINITIONS} )

find_package( PROTOBUF REQUIRED )
include_directories( SYSTEM ${PROTOBUF_INCLUDE_DIR} )

# Sources
set( tool_srcs main.cpp
  Tool.cpp
  )

set( tool_headers
  Tool.h
)

# Compile portals into portals folder
add_subdirectory( ../portals ${CMAKE_CURRENT_BINARY_DIR}/portals )
include_directories( ../portals )

# Compile messages into messages folder
add_subdirectory( ../messages ${CMAKE_CURRENT_BINARY_DIR}/messages )

add_subdirectory( unlog )

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
  ${QT_LIBRARIES}
  )
