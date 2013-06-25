cmake_minimum_required(VERSION 2.8.3)

set(SHAREDBALL_SRCS SharedBall
)

add_library(
  sharedball
  STATIC
  ${SHAREDBALL_SRCS}
)

target_link_libraries(
  sharedball
  support
  portals
  messages
)