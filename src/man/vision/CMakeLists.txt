

############################ PROJECT NAME
# The name of this cmake project
PROJECT( VISION )
CMAKE_MINIMUM_REQUIRED( VERSION 2.8.3 )


############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project

SET( VISION_SRCS
  Blob.cpp
  BallDetector.cpp
  PostDetector.cpp
  DiffOfGaussianFilter.cpp
  )

SET( VISION_HEADERS
  Blob.h
  Blobber.h
  FuzzyLogic.h
  BallDetector.h
  PostDetector.h
  DiffOfGaussianFilter.h
)

# Add static library target
ADD_LIBRARY(
  vision
  STATIC
  ${VISION_SRCS}
  ${VISION_HEADERS}
  )
