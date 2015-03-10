

############################ PROJECT NAME
# The name of this cmake project
PROJECT( NEW_VISION )
CMAKE_MINIMUM_REQUIRED( VERSION 2.8.3 )


############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project

SET( NEW_VISION_SRCS
  Blob.cpp
  BallDetector.cpp
  )

SET( NEW_VISION_HEADERS
  Blob.h
  Blobber.h
  FuzzyLogic.h
  BallDetector.h
)

# Add static library target
ADD_LIBRARY(
  new_vision
  STATIC
  ${NEW_VISION_SRCS}
  ${NEW_VISION_HEADERS}
  )
