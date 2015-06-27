cmake_minimum_required( VERSION 2.8.3 )

set(CONTROL_SRCS control.cpp)

add_library(
  control
  STATIC
  ${CONTROL_SRCS} )

target_link_libraries(
  control
  logshare
  messages
)
