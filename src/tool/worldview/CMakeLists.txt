cmake_minimum_required(VERSION 2.8.3)

# Set source files here that need to be compiled.
set(WORLDVIEW_SRCS
  WorldView
)

# Add project as a library.
add_library(
  worldview
  STATIC
  ${WORLDVIEW_SRCS}
)

# Link project to appropriate libraries.
target_link_libraries(worldview
  portals
  messages
  support
  tool_common
  ${ALCOMMON_LIBRARIES}
)