cmake_minimum_required(VERSION 2.8.3)

set(OBSTACLE_SRCS ObstacleModule)

add_library(
  obstacle
  STATIC
  ${OBSTACLE_SRCS}
)

target_link_libraries(
  obstacle
  portals
  messages
)