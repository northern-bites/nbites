cmake_minimum_required( VERSION 2.8.3 )

project (common)

set( COMMON_SRCS NBMath
                 NBMatrixMath )

add_library(
  common
  STATIC
  ${COMMON_SRCS}
)