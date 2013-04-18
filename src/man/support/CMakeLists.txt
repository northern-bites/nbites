cmake_minimum_required( VERSION 2.8.3 )

set( COMMON_SRCS NBMath
  NBVector
  NBMatrixMath
  dsp
  Common
  CommDef
  RoboCupGameControlData
  CoordFrame3D.cpp
  CoordFrame4D.cpp
  CameraCalibrate.cpp
  ALLedNames
  NogginStructs
  ColorParams.cpp
  )

add_library(
  support
  STATIC
  ${COMMON_SRCS}
)
