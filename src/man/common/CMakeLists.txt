cmake_minimum_required( VERSION 2.8.3 )

set( COMMON_SRCS Common
                 CommDef
                 NBMath
                 NBMatrixMath
                 NBVector
                 VisionDef
                 dsp
)

add_library(
  common
  STATIC
  ${COMMON_SRCS}
)