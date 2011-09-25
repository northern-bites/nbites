

############################ PROJECT NAME
# The name of this cmake project
PROJECT( VISION )
CMAKE_MINIMUM_REQUIRED( VERSION 2.6.0 )

############################ BASE DEFINITIONS
# Include  the basic definitions accross all projects
INCLUDE( ../cmake/base_definitions.cmake )


############################ FIND PACKAGE
# Libraries specifics. Define here cmake packages
# useful to your project
FIND_PACKAGE( PythonLibs   REQUIRED )
FIND_PACKAGE( Boost        REQUIRED )


############################ BUILDCONFIG
# Include the config file for this build
INCLUDE( ${VISION_CMAKE_DIR}/buildconfig.cmake )


############################ TESTS
# Define main test script location
#SET( TEST_RUN "${AL_DIR}/extern/python/aldebaran/test_run.py" )

#<ADD_TEST> don't remove this comment
### test example ADD_TEST(My_test python ${TEST_RUN} -p ${NAOQI} -s ${CMAKE_CURRENT_SOURCE_DIR}/test/myRubyTest.rb)

#</ADD_TEST> don't remove this comment


############################ CONFIG.H GENERATION
# Generate the config.h file, inserting values of CMake cache variable
CONFIGURE_FILE(
  ${VISION_CMAKE_DIR}/profileconfig.h.in
  ${CFG_DIR}/profileconfig.h
    ESCAPE_QUOTES
  )

CONFIGURE_FILE(
  ${VISION_CMAKE_DIR}/visionconfig.h.in
  ${CFG_DIR}/visionconfig.h
    ESCAPE_QUOTES
  )


############################ INCLUDE DIRECTORY
# Define include directories
INCLUDE_DIRECTORIES( ${NBCOMMON_INCLUDE_DIR}
                     ${PYTHON_INCLUDE_PATH}
                     ${BOOST_INCLUDE_DIR}
                     ${CFG_DIR}
                     )

############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project
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
                 FieldLines
                 ObjectFragments
                 Profiler
                 PyVision
                 Robots
                 Threshold
                 Utility
                 Vision
                 VisualBall
                 VisualCrossbar
                 VisualCorner
                 VisualDetection
                 VisualFieldEdge
                 VisualLandmark
                 VisualFieldObject
                 VisualRobot
                 VisualCross
                 VisualLine
                 EdgeDetector
                 Gradient
                 HoughSpace
                 geom/HoughLine
                 VisualDetector
                 FieldLinesDetector
                 )

# Add the assembly files if we're not on Mac OSX
IF (UNIX AND NOT APPLE)
  LIST( APPEND VISION_SRCS
    EdgeDetection.s
    )

SET_PROPERTY(SOURCE
  EdgeDetection.s
 PROPERTY LANGUAGE C)
ENDIF()

SET( VISION_PYTHON_SRCS
   )

############################ CHECK LIBRARY / EXECUTABLE OPTION
# Configure the output execuatable or shared library, and the install
# commands

IF( PYTHON_SHARED_VISION )
  # Add shared library target
  ADD_LIBRARY(
    ${VISION_TARGET}
    SHARED
    ${VISION_SRCS}
    )

  # Install to XXXvisionmodule.so, instead of libvision.so
  GET_TARGET_PROPERTY( VISION_TARGET_LOCATION ${VISION_TARGET} LOCATION )
  INSTALL(
    FILES ${VISION_TARGET_LOCATION}
    DESTINATION ${VISION_PYTHON_TARGET_DIR}
    CONFIGURATIONS "" Debug Release
    RENAME ${ROBOT_PREFIX}${VISION_TARGET}module.so
    )
ELSE( PYTHON_SHARED_VISION )
  # Add static library target
  ADD_LIBRARY(
    ${VISION_TARGET}
    STATIC
    ${VISION_SRCS}
    )
ENDIF( PYTHON_SHARED_VISION )

INSTALL(
  FILES ${VISION_PYTHON_SRCS}
  DESTINATION ${VISION_PYTHON_TARGET_DIR}
  CONFIGURATIONS "" Debug Release
  )

############################ SET LIBRARIES TO LINK WITH
TARGET_LINK_LIBRARIES(
  ${VISION_TARGET}
  ${MEMORY_TARGET}
  ${NBINCLUDE_TARGET}
  ${PYTHON_LIBRARY} )


############################ VISIONLINK
IF( VISIONLINK )

  FIND_PACKAGE( JNI REQUIRED )

  INCLUDE_DIRECTORIES( ${VISIONLINK_HEADER_DIR}
                       ${JNI_INCLUDE_DIRS} )

  SET( VISIONLINK_SRCS ${VISIONLINK_FILE})
  ADD_LIBRARY(
    ${VISIONLINK_TARGET}
    SHARED
    ${VISIONLINK_SRCS}
    )

  IF( WIN32 )
    ADD_DEFINITIONS( -D_JNI_IMPLEMENTATION_ )
    SET_TARGET_PROPERTIES(${VISIONLINK_TARGET} PROPERTIES LINK_FLAGS -Wl,--kill-at)
  ENDIF( WIN32 )

  TARGET_LINK_LIBRARIES( ${VISIONLINK_TARGET}
                        ${VISION_TARGET}
                        ${SENSORS_TARGET}
                        ${NBINCLUDE_TARGET}
                       )
ENDIF( VISIONLINK )


############################ (SUB)DIRECTORY COMPILATION
# Set the sudirectories (some may not actually be subdirectories) to
# include in this package
#SUBDIRS(
#  ${CORPUS_CMAKE_DIR}
#  )

########################### VISION TEST EXECUTABLES

# set( VISION_TEST_DIR ${VISION_INCLUDE_DIR}/test)
# add_executable( EdgeDetectorTest ${VISION_INCLUDE_DIR}/EdgeDetector.cpp
#   ${VISION_TEST_DIR}/EdgeDetectorTest.cpp
#   ${VISION_INCLUDE_DIR}/EdgeDetection.s
#   ${VISION_INCLUDE_DIR}/Gradient.cpp )

# target_link_libraries( EdgeDetectorTest ${NBCOMMON_LIBRARIES} ${VISION_TARGET})


# add_executable( HoughSpaceTest ${VISION_INCLUDE_DIR}/HoughSpace.cpp
#   ${VISION_TEST_DIR}/HoughSpaceTest.cpp
#   ${VISION_INCLUDE_DIR}/geom/HoughLine.cpp
#   ${VISION_INCLUDE_DIR}/EdgeDetection.s
#   ${VISION_INCLUDE_DIR}/Gradient.cpp )

# target_link_libraries( HoughSpaceTest ${VISION_TARGET})



# add_executable( OfflineVision ${VISION_INCLUDE_DIR}/offline/OfflineVision.cpp )
# target_link_libraries( OfflineVision
#   ${VISION_TARGET}
#   ${SENSORS_TARGET}
#   ${NBINCLUDE_TARGET}
#   )

# enable_testing()
# add_test( EdgeDetectorTest  EdgeDetectorTest )
# add_test( HoughSpaceTest  HoughSpaceTest )


# add_executable( ConvertFrame ${VISION_INCLUDE_DIR}/offline/ConvertFrames.cpp )
