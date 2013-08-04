cmake_minimum_required(VERSION 2.8.3)

set(ARMS_SRCS ArmContactModule)

add_library(
  arms
  STATIC
  ${ARMS_SRCS}
)

target_link_libraries(
  arms
  portals
  messages
)