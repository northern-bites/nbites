cmake_minimum_required( VERSION 2.8.3 )

set( COMMON_SRCS NBMath
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
)

add_library(
  common
  STATIC
  ${COMMON_SRCS}
)