cmake_minimum_required( VERSION 2.8.3 )

set(LOG_SRCS LogModule.cpp log_fileio.cpp log_main.cpp log_streamio.cpp)

add_library(
  log
  STATIC
  ${LOG_SRCS} )

target_link_libraries(
  log
  control
  logshare
  portals
  messages
)
