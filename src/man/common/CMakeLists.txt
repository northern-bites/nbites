cmake_minimum_required( VERSION 2.8.3 )

set( COMMON_SRCS NBMath
  NBMatrixMath
  dsp
  Common
  CommDef
)

add_library(
  common
  STATIC
  ${COMMON_SRCS}
)