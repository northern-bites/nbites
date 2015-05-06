############################ PROJECT NAME
# The name of this cmake project
PROJECT( VISION )
CMAKE_MINIMUM_REQUIRED( VERSION 2.8.3 )

ADD_DEFINITIONS( -mmmx )

############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project
SET( VISION_SRCS
  VisionModule.cpp
  Blob.cpp
  BallDetector.cpp
  Ball.cpp
  PostDetector.cpp
  DiffOfGaussianFilter.cpp
  EdgeDetector.cpp
  Gradient.cpp
  HighResTimer.cpp
  )

SET( VISION_HEADERS
  VisionModule.h
  Blob.h
  Blobber.h
  FuzzyLogic.h
  BallDetector.h
  Ball.h
  PostDetector.h
  DiffOfGaussianFilter.h
  EdgeDetector.h
  Gradient.h
  HighResTimer.h
)

# Add the assembly files if we're not on Mac OSX
IF (UNIX AND NOT APPLE)
  SET( ASM_SRCS
    EdgeDetection.s
    )
  LIST( APPEND VISION_SRCS ${ASM_SRCS})
  SET_PROPERTY(SOURCE ${ASM_SRCS} PROPERTY LANGUAGE C)
ENDIF()

# Add static library target
ADD_LIBRARY(
  vision
  STATIC
  ${VISION_SRCS}
  ${VISION_HEADERS}
  )
