

############################ PROJECT NAME
# The name of this cmake project
PROJECT( MOTION )
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
INCLUDE( ${MOTION_CMAKE_DIR}/buildconfig.cmake )


############################ CONFIG.H GENERATION
CONFIGURE_FILE(
  ${MOTION_CMAKE_DIR}/motionconfig.in
  ${CFG_DIR}/motionconfig.h
  ESCAPE_QUOTES
  )

############################ DEFINITIONS
# Add here definition needed.
ADD_DEFINITIONS(
               ${CMAKE_THREAD_LIBS_INIT}
                )


############################ INCLUDE DIRECTORY
# Define include directories
INCLUDE_DIRECTORIES( ${NBCOMMON_INCLUDE_DIR}
                     ${ALCOMMON_INCLUDE_DIR}
                     ${PYTHON_INCLUDE_PATH}
                     ${CORPUS_INCLUDE_DIR}
		             ${CFG_DIR}
		             )


############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project
SET( MOTION_SRCS
  #CallbackAction
                     BodyJointCommand
		     HeadJointCommand
		     JointCommand
		     MotionCommand
                     MotionConstants
                     Motion
                     MotionInterface
		     MotionSwitchboard
		     ScriptedProvider
		     ChoppedCommand
		     LinearChoppedCommand
		     SmoothChoppedCommand
		     PreviewChoppedCommand
		     COMPreview
		     BalancingChoppedCommand
		     BaseFreezeCommand
		     HeadProvider
		     ChopShop
		     NullProvider
		     WalkProvider
		     Step
		     Gait
		     AbstractGait
		     MetaGait
		     SensorAngles
		     SpringSensor
		     WalkingLeg
		     OdometryFilter
		     WalkingArm
		     BalancingArm
		     PreviewController
		     Observer
		     ZmpExp
		     ZmpAccExp
		     ZmpEKF
		     ZmpAccEKF
		     StepGenerator
                     ${CORPUS_INCLUDE_DIR}/InverseKinematics
                     ${CORPUS_INCLUDE_DIR}/COMKinematics
                     PyMotion
                     )

IF( PYTHON_SHARED_MOTION AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
  MESSAGE( STATUS "Looking for extra motion sources in ${MAN_INCLUDE_DIR}" )
  LIST( APPEND MOTION_SRCS ${MAN_INCLUDE_DIR}/Sensors )
ENDIF( PYTHON_SHARED_MOTION AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )

SET( MOTION_PYTHON_SRCS __init__.py
                        MotionConstants.py
                        SweetMoves.py
                        HeadMoves.py
                        RobotGaits.py
                        StiffnessModes.py
                        )

SET( MOTION_PYTHON_SUBDIRS gaits )


############################ CHECK LIBRARY / EXECUTABLE OPTION
# Configure the output execuatable or shared library, and the install
# commands

GET_TARGET_PROPERTY( MOTION_LOCATION ${MOTION_TARGET} LOCATION )

IF( PYTHON_SHARED_MOTION )
  ADD_LIBRARY(
    ${MOTION_TARGET}
    SHARED
    ${MOTION_SRCS}
    )
  # Install to _motionmodule.so, instead of lib_motion.so
  GET_TARGET_PROPERTY(
    MOTION_TARGET_LOCATION ${MOTION_TARGET} LOCATION
    )
  INSTALL(
    FILES ${MOTION_TARGET_LOCATION}
    DESTINATION ${MOTION_PYTHON_TARGET_DIR}
    CONFIGURATIONS "" Debug Release
    RENAME ${MOTION_TARGET}module.so
    )
ELSE( PYTHON_SHARED_MOTION )
  ADD_LIBRARY(
    ${MOTION_TARGET}
    STATIC
    ${MOTION_SRCS}
    )
  # No INSTALL target when static
ENDIF( PYTHON_SHARED_MOTION )

INSTALL(
  FILES ${MOTION_PYTHON_SRCS}
  DESTINATION ${MOTION_PYTHON_TARGET_DIR}
  CONFIGURATIONS "" Debug Release
  )

INSTALL(
  DIRECTORY ${MOTION_PYTHON_SUBDIRS}
  DESTINATION ${MOTION_PYTHON_TARGET_DIR}/
  CONFIGURATIONS "" Debug Release
  PATTERN "*.py"
  )

############################ SET LIBRARIES TO LINK WITH
TARGET_LINK_LIBRARIES(
  ${MOTION_TARGET}
  ${PYTHON_LIBRARY}
  ${SENSORS_TARGET}
  ${Boost_PYTHON_LIBRARY}
  bhwalk
  )


############################ (SUB)DIRECTORY COMPILATION
# Set the sudirectories (some may not actually be subdirectories) to
# include in this package
add_subdirectory( bhwalk )
#SUBDIRS(
#  ${CORPUS_CMAKE_DIR}
#  )
