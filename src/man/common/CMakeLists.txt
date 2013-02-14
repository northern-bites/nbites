cmake_minimum_required( VERSION 2.8.3 )

set( COMMON_SRCS NBMath.cpp
                 NBMatrixMath.cpp 
		 CoordFrame3D.cpp
		 CoordFrame4D.cpp
		 CameraCalibrate.cpp
		 )

add_library(
  common
  STATIC
  ${COMMON_SRCS}
)

