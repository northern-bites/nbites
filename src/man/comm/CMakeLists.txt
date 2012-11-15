
############################ PROJECT NAME
# The name of this cmake project
PROJECT( COMM )
CMAKE_MINIMUM_REQUIRED( VERSION 2.8.3 )

############################ BASE DEFINITIONS
# Include  the basic definitions accross all projects
INCLUDE( ../cmake/base_definitions.cmake )


############################ FIND PACKAGE
# Libraries specifics. Define here cmake packages
# usefull to your project
FIND_PACKAGE( PythonLibs   REQUIRED )
#FIND_PACKAGE( Threads  REQUIRED )


############################ BUILDCONFIG
# Include the config file for this build
INCLUDE( ${COMM_CMAKE_DIR}/buildconfig.cmake )

############################ TESTS
# Define main test script location
#SET( TEST_RUN "${AL_DIR}/extern/python/aldebaran/test_run.py" )

#<ADD_TEST> don't remove this comment
### test example ADD_TEST(My_test python ${TEST_RUN} -p ${NAOQI} -s ${CMAKE_CURRENT_SOURCE_DIR}/test/myRubyTest.rb)

#</ADD_TEST> don't remove this comment


############################ CONFIG.H GENERATION
CONFIGURE_FILE(
  ${COMM_CMAKE_DIR}/commconfig.in
  ${CFG_DIR}/commconfig.h
    ESCAPE_QUOTES
  )


############################ DEFINITION
# Add here definition needed.
ADD_DEFINITIONS(
  ${CMAKE_THREAD_LIBS_INIT}
  )


############################ INCLUDE DIRECTORY
# Define include directories
INCLUDE_DIRECTORIES(
  ${NBCOMMON_INCLUDE_DIR}
  ${PYTHON_INCLUDE_PATH}
  ${VISION_INCLUDE_DIR}
  ${CFG_DIR}
  ${MAN_MOTION_INCLUDE_DIR}
  )


############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project
SET( COMM_SRCS Comm
               CommTimer
	       NetworkMonitor
               TeamConnect
               TeamMember
               GameConnect
               GameData
               UDPSocket
               RoboCupGameControlData
               PyComm
               )

IF( PYTHON_SHARED_COMM )
  LIST( APPEND COMM_SRCS ${CORPUS_INCLUDE_DIR}/Sensors
                         ${CORPUS_INCLUDE_DIR}/NaoPose
                         )
ENDIF( PYTHON_SHARED_COMM )

SET( COMM_PYTHON_SRCS )

############################ CHECK LIBRARY / EXECUTABLE OPTION
# Configure the output execuatable or shared library, and the install
# commands

IF( PYTHON_SHARED_COMM )
  ADD_LIBRARY(
    ${COMM_TARGET}
    SHARED
    ${COMM_SRCS}
    )
  # Install to _motionmodule.so, instead of lib_motion.so
  GET_TARGET_PROPERTY(
    COMM_TARGET_LOCATION ${COMM_TARGET} LOCATION
    )
  INSTALL(
    FILES ${COMM_TARGET_LOCATION}
    DESTINATION ${COMM_PYTHON_TARGET_DIR}
    CONFIGURATIONS "" Debug Release
    RENAME ${COMM_TARGET}module.so
    )
ELSE( PYTHON_SHARED_COMM )
  ADD_LIBRARY(
    ${COMM_TARGET}
    STATIC
    ${COMM_SRCS}
    )
  # No INSTALL target when static
ENDIF( PYTHON_SHARED_COMM )

INSTALL(
  FILES ${COMM_PYTHON_SRCS}
  DESTINATION ${COMM_PYTHON_TARGET_DIR}
  CONFIGURATIONS "" Debug Release
  )

############################ SET LIBRARIES TO LINK WITH
TARGET_LINK_LIBRARIES(
  ${COMM_TARGET}
  ${PYTHON_LIBRARY}
  ${VISION_TARGET}
  )


############################ (SUB)DIRECTORY COMPILATION
# Set the sudirectories (some may not actually be subdirectories) to
# include in this package
#  SUBDIRS(
#    ${VISION_CMAKE_DIR}
#    )
