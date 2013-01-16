project( man )
cmake_minimum_required( VERSION 2.8.3 )

# A list of options that should be advanced
include( cmake/Advanced.cmake )
# A list of options that we do want
include( cmake/RobotConfig.cmake )

# Configure the copy script based on our settings
configure_file(
  "${CMAKE_CURRENT_LIST_DIR}/cmake/upload.sh.in"
  "${PROJECT_BINARY_DIR}/upload.sh"
)

set( MAN_SRCS main.cpp
  NewMan.cpp
)

add_library(
  man
  SHARED
  ${MAN_SRCS}
)

install( TARGETS man
  LIBRARY DESTINATION lib
)
