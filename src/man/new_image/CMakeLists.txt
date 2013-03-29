cmake_minimum_required( VERSION 2.8.3 )

set( IMAGE_SRCS
  TranscriberModule.cpp
)

add_library(
  image
  STATIC
  ${IMAGE_SRCS}
)

target_link_libraries(
  image
  common
)