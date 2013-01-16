project( man )
cmake_minimum_required( VERSION 2.8.3 )

# A list of options that should be advanced
include( cmake/Advanced.cmake )
# A list of options that we do want
include( cmake/RobotConfig.cmake )

# So that we can use find_package commands
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# For Aldebaran's headers
find_package(ALCOMMON REQUIRED)
include_directories( ${ALCOMMON_INCLUDE_DIR} )


# Configure the copy script based on our settings
configure_file(
  "${CMAKE_CURRENT_LIST_DIR}/cmake/upload.sh.in"
  "${PROJECT_BINARY_DIR}/upload.sh"
)

set( MAN_SRCS main.cpp
  NewMan.cpp
)

add_subdirectory( ../portals ${CMAKE_CURRENT_BINARY_DIR}/portals/ )

add_library(
  man
  SHARED
  ${MAN_SRCS}
)

target_link_libraries(
  man
  portals
)

install( TARGETS man
  LIBRARY DESTINATION lib
)
