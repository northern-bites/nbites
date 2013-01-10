

############################ PROJECT NAME
# The name of this cmake project
PROJECT( CORPUS )
CMAKE_MINIMUM_REQUIRED( VERSION 2.8.3 )

############################ BASE DEFINITIONS
# Include  the basic definitions accross all projects
INCLUDE( ../cmake/base_definitions.cmake )


############################ FIND PACKAGE
# Libraries specifics. Define here cmake packages
# usefull to your project
#FIND_PACKAGE( Threads  REQUIRED )
FIND_PACKAGE( PythonLibs   REQUIRED )
FIND_PACKAGE( Boost COMPONENTS python REQUIRED )


include( ../cmake/FindALCOMMON.cmake )

############################ BUILDCONFIG
# Include the config file for this build
INCLUDE( ${CORPUS_CMAKE_DIR}/buildconfig.cmake )

############################ CONFIG.H GENERATION
CONFIGURE_FILE(
  ${CORPUS_CMAKE_DIR}/corpusconfig.h.in
  ${CFG_DIR}/corpusconfig.h
  ESCAPE_QUOTES
  )

############################ INCLUDE DIRECTORY
# Define include directories
INCLUDE_DIRECTORIES(
  ${NBCOMMON_INCLUDE_DIR}
  ${PYTHON_INCLUDE_PATH}
  ${VISION_INCLUDE_DIR}
  ${CFG_DIR}
  ${BOOST_INCLUDE_DIR}
  )

#Possibly, don't need to include any AL stuff in this directory
# since we may inherit it from man
INCLUDE_DIRECTORIES(${ALCOMMON_INCLUDE_DIR})
ADD_DEFINITIONS( ${ALCOMMON_DEFINITIONS})

############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project
SET( SENSORS_SRCS Sensors
  PySensors
  NaoPose
  CameraCalibrate
  )

########### ROBOT CONNECT #########
SET( ROBOT_INTERFACE_SRCS
  AccEKF
  RoboGuardian
  guardian/ConnectionAngel
  guardian/WifiAngel
  ImageAcquisition
  ColorParams
  ClickableButton
  PyRoboGuardian
  Lights
  PyLights
  Speech
  PySpeech
  )

IF(OFFLINE)
  SET( ROBOT_CONNECT_SRCS
    offlineconnect/OfflineTranscriber
    offlineconnect/OfflineImageTranscriber
    )
ENDIF()

IF(BUILDING_FOR_A_REMOTE_NAO)
  SET( ROBOT_CONNECT_SRCS
    alconnect/ALEnactor
    alconnect/NaoEnactor
    alconnect/ALTranscriber
    alconnect/NaoLights
    alconnect/NaoRGBLight
    alconnect/ALSpeech
    V4L2ImageTranscriber
    NaoImageTranscriber
    )
ENDIF()

SET( CORPUS_PYTHON_SRCS
  leds.py
  sensors.py
  )

# Add the assembly files if we're on Linux
IF (UNIX AND NOT APPLE)
  LIST( APPEND ROBOT_INTERFACE_SRCS
    ImageAcquisition.s
    ImageAcquisitionFast.s
    CopyImage.s
    )

  SET_PROPERTY(SOURCE
    ImageAcquisition.s
    ImageAcquisitionFast.s
    CopyImage.s
    PROPERTY LANGUAGE C )

ENDIF()

SET( CORPUS_TEST_DIR test )

############################ CHECK LIBRARY / EXECUTABLE OPTION
# Configure the output execuatable or shared library, and the install
# commands

IF( PYTHON_SHARED_CORPUS )
  ADD_LIBRARY(
    ${SENSORS_TARGET}
    SHARED
    ${SENSORS_SRCS}
    )
  ADD_LIBRARY(
    ${ROBOT_CONNECT_TARGET}
    SHARED
    ${ROBOT_CONNECT_SRCS}
    )
  GET_TARGET_PROPERTY( SENSORS_TARGET_LOCATION ${SENSORS_TARGET} LOCATION )
  GET_TARGET_PROPERTY( ROBOT_CONNECT_TARGET_LOCATION
    ${ROBOT_CONNECT_TARGET}   LOCATION )
  # Install to TARGETmodule.so, instead of lib_TARGET.so
  INSTALL(
    FILES ${SENSORS_TARGET_LOCATION}
    DESTINATION ${CORPUS_PYTHON_TARGET_DIR}
    CONFIGURATIONS "" Debug Release
    RENAME ${SENSORS_TARGET}module.so
    )
  INSTALL(
    FILES ${ROBOT_CONNECT_TARGET_LOCATION}
    DESTINATION ${CORPUS_PYTHON_TARGET_DIR}
    CONFIGURATIONS "" Debug Release
    RENAME ${ROBOT_CONNECT_TARGET}module.so
    )

ELSE( PYTHON_SHARED_CORPUS )
  ADD_LIBRARY(
    ${SENSORS_TARGET}
    STATIC
    ${SENSORS_SRCS}
    )
  ADD_LIBRARY(
    ${ROBOT_CONNECT_TARGET}
    STATIC
    ${ROBOT_CONNECT_SRCS}
    )
  ADD_LIBRARY(
    ${ROBOT_INTERFACE_TARGET}
    STATIC
    ${ROBOT_INTERFACE_SRCS}
    )
  # No INSTALL target when static
ENDIF( PYTHON_SHARED_CORPUS )

INSTALL(
  FILES ${CORPUS_PYTHON_SRCS}
  DESTINATION ${CORPUS_PYTHON_TARGET_DIR}
  CONFIGURATIONS "" Debug Release
  )


############################ LINKER LANGUAGE
# For Macs, set the target link language to use
SET_TARGET_PROPERTIES( ${SENSORS_TARGET} PROPERTIES LINKER_LANGUAGE CXX )


############################ SET LIBRARIES TO LINK WITH
TARGET_LINK_LIBRARIES( ${SENSORS_TARGET}
  ${PYTHON_LIBRARY}
  ${Boost_PYTHON_LIBRARY}
  memory
  ${NBINCLUDE_TARGET}
  ${VISION_TARGET}
  )

TARGET_LINK_LIBRARIES( ${ROBOT_INTERFACE_TARGET}
  ${SENSORS_TARGET}
  ${PYTHON_LIBRARIES}
  ${MOTION_TARGET} #for motioninterface in roboguardian
  ${NBINCLUDE_TARGET}
  ${Boost_PYTHON_LIBRARY}
  )

TARGET_LINK_LIBRARIES( ${ROBOT_CONNECT_TARGET}
  ${ALCOMMON_LIBRARIES}
  ${ROBOT_INTERFACE_TARGET}
  )

############################ (SUB)DIRECTORY COMPILATION
# Set the sudirectories (some may not actually be subdirectories) to
# include in this package

############################ TESTS!

# Broken currently
# add_executable( ImageAcquisitionTest
#   ${CORPUS_TEST_DIR}/ImageAcquisitionTest
#   ${CORPUS_INCLUDE_DIR}/ImageAcquisition.s
#   ${CORPUS_INCLUDE_DIR}/ImageAcquisitionFast.s
#   ${CORPUS_INCLUDE_DIR}/ColorParams
#   )

# target_link_libraries( ImageAcquisitionTest
#   ${NBCOMMON_LIBRARIES}
#   )

# add_test( NAME ImageAcquisitionTest COMMAND ImageAcquisitionTest )
