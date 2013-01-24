# Main CMake file for the Tool!
project( tool )
cmake_minimum_required( VERSION 2.8.3 )

# Dependencies

find_package( Qt4 REQUIRED )

include( ${QT_USE_FILE} )
add_definitions( ${QT_DEFINITIONS} )

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

# Qt4 goodness
Qt4_wrap_cpp( tool_headers_moc ${tool_headers} )

# The main executable
add_executable( ${TOOL_EXECUTABLE}
  ${tool_srcs}
  ${tool_headers_moc} )

target_link_libraries(
  ${TOOL_EXECUTABLE}
  portals
  ${QT_LIBRARIES}
  )
