cmake_minimum_required( VERSION 2.8.3 )

set( PORTALS_SRCS RoboGrams)

add_library(
  portals
  STATIC
  ${PORTALS_SRCS}
)
