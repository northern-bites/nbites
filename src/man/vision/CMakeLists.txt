

############################ PROJECT NAME
# The name of this cmake project
PROJECT( VISION )
CMAKE_MINIMUM_REQUIRED( VERSION 2.8.3 )




ADD_DEFINITIONS( -mmmx )

INCLUDE_DIRECTORIES( geom )
INCLUDE_DIRECTORIES( fieldlines )
INCLUDE_DIRECTORIES( . )


############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project
SET( FIELD_LINES_SRCS
  FieldLines/FieldLinesDetector
  FieldLines/CornerDetector
  FieldLines/HoughSpace
  FieldLines/HoughSpaceImpl
  FieldLines/EdgeDetector
  FieldLines/Gradient
  FieldLines/HoughVisualLine
  FieldLines/HoughVisualCorner
  geom/HoughLine
  )

SET( VISION_SRCS Ball
                 Blob
                 Blobs
                 ConcreteCorner
                 ConcreteLandmark
                 ConcreteFieldObject
                 ConcreteCross
                 ConcreteLine
                 Context
                 Cross
                 Field
                 FieldLines.cpp
                 ObjectFragments
                 Robots
                 Threshold
                 Utility
                 Vision
		 VisionModule
                 VisualBall
                 VisualCrossbar
                 VisualCorner
                 VisualDetection
                 VisualFieldEdge
                 VisualLandmark
                 VisualFieldObject
		 VisualObstacle
                 VisualRobot
                 VisualCross
                 VisualLine
                 VisualDetector
		 ${FIELD_LINES_SRCS}
		 NaoPose
                 )

# Add the assembly files if we're not on Mac OSX
IF (UNIX AND NOT APPLE)
  SET( ASM_SRCS
    FieldLines/EdgeDetection.s
    FieldLines/Hough.s
    FieldLines/HoughAlt.s
    )
  LIST( APPEND VISION_SRCS ${ASM_SRCS})
  SET_PROPERTY(SOURCE ${ASM_SRCS} PROPERTY LANGUAGE C)
ENDIF()


############################ CHECK LIBRARY / EXECUTABLE OPTION
# Configure the output execuatable or shared library, and the install
# commands

# Add static library target
ADD_LIBRARY(
  vision
  STATIC
  ${VISION_SRCS}
  )

############################ SET LIBRARIES TO LINK WITH
TARGET_LINK_LIBRARIES(
  vision
  portals
  common
  messages)


