cmake_minimum_required(VERSION 2.8.3)

# Set source files here that need to be compiled.
set(WORLDVIEW_SRCS
  WorldView.cpp
  WorldViewPainter.cpp
)

set(WORLDVIEW_HEADERS
  WorldView.h
  WorldViewPainter.h
)

Qt4_wrap_cpp( WORLDVIEW_HEADERS_MOC ${WORLDVIEW_HEADERS} )

#configure_file(
#  "${CMAKE_CURRENT_LIST_DIR}/cmake/RobotConfig.in"
#  "${PROJECT_BINARY_DIR}/RobotConfig.h"
#)

# Add project as a library.
add_library(worldview
  STATIC
  ${WORLDVIEW_SRCS}
  ${WORLDVIEW_HEADERS_MOC}
)

# Link project to appropriate libraries.
target_link_libraries(worldview
  comm
  tool_common
  portals
  messages
  ${QT_LIBRARIES}
)