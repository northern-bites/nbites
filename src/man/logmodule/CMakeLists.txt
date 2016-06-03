cmake_minimum_required( VERSION 2.8.3 )

set(LOG_SRCS LogModule.cpp LogModule.h )

add_library(
  logmodule
  STATIC
  ${LOG_SRCS} )

target_link_libraries(
  logmodule

  logging
  logshare
  portals
  messages
)
