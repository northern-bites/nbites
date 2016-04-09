############################ PROJECT NAME
PROJECT( VISION )
CMAKE_MINIMUM_REQUIRED( VERSION 2.8.3 )

ADD_DEFINITIONS( -mmmx )

############################ PROJECT SOURCES FILES
SET( VISION_SRCS
  VisionModule.cpp
  Edge.cpp
  FastBlob.cpp
  Field.cpp
  FrontEnd.cpp
  Kinematics.cpp
  Homography.cpp
  Hough.cpp
  Vision.cpp
  BallDetector.cpp
  RobotObstacle.cpp
  Ransac.cpp
  )

IF (UNIX AND NOT APPLE)
  SET( ASM_SRCS
    Acquire.s
    AsmBlob.s
    AsmHough.s
    Gradient.s
    RunLength.s
    )
  LIST( APPEND VISION_SRCS ${ASM_SRCS})
  SET_PROPERTY(SOURCE ${ASM_SRCS} PROPERTY LANGUAGE C)
ENDIF()

############################ CHECK LIBRARY / EXECUTABLE OPTION
# Add static library target
ADD_LIBRARY(
  vision
  STATIC
  ${VISION_SRCS}
  ${VISION_HEADERS}
  )

############################ SET LIBRARIES TO LINK WITH
TARGET_LINK_LIBRARIES(
  vision
  portals
  support
  messages
  )
