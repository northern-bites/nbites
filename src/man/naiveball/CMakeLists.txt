cmake_minimum_required(VERSION 2.8.3)

set(NAIVEBALL_SRCS NaiveBallModule)

add_library(
  naiveball
  STATIC
  ${NAIVEBALL_SRCS}
)

target_link_libraries(
  naiveball
  support
  portals
  messages
)