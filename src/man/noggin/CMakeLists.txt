

############################ PROJECT NAME
# The name of this cmake project
PROJECT( NOGGIN )
CMAKE_MINIMUM_REQUIRED( VERSION 2.6.0 )

############################ CMAKE VERSION
# Check cMake version
CMAKE_MINIMUM_REQUIRED( VERSION 2.6.0 )


############################ BASE DEFINITIONS
# Include  the basic definitions accross all projects
INCLUDE( ../cmake/base_definitions.cmake )


############################ FIND PACKAGE
# Libraries specifics. Define here cmake packages
# usefull to your project
FIND_PACKAGE( PythonLibs   REQUIRED )
FIND_PACKAGE( Boost COMPONENTS python REQUIRED )

############################ BUILDCONFIG
# Include the config file for this build
INCLUDE( ${NOGGIN_CMAKE_DIR}/buildconfig.cmake )


############################ TeamConfig.py variables
SET(@GC_TEAM_NUMBER@
  "101"
  CACHE STRING
  "The team number for the robot (GameController)"
  )
SET( GC_TEAM_NUMBER ${@GC_TEAM_NUMBER@} )
SET(@GC_PLAYER_NUMBER@
  "2"
  CACHE STRING
  "The player number for the robot (GameController)"
  )
SET( GC_PLAYER_NUMBER ${@GC_PLAYER_NUMBER@} )
############################ CONFIG.H GENERATION
CONFIGURE_FILE(
  ${NOGGIN_CMAKE_DIR}/nogginconfig.in
  ${CFG_DIR}/nogginconfig.h
  ESCAPE_QUOTES
  )

CONFIGURE_FILE(
  ${NOGGIN_CMAKE_DIR}/Switch.py.in
  ${CMAKE_CURRENT_BINARY_DIR}/players/Switch.py
  )

CONFIGURE_FILE(
  ${NOGGIN_CMAKE_DIR}/TeamConfig.py.in
  ${CMAKE_CURRENT_BINARY_DIR}/TeamConfig.py
  )

CONFIGURE_FILE(
  ${NOGGIN_CMAKE_DIR}/WebotsConfig.py.in
  ${CMAKE_CURRENT_BINARY_DIR}/WebotsConfig.py
  )

############################ INCLUDE DIRECTORY
# Define include directories
INCLUDE_DIRECTORIES( ${NBCOMMON_INCLUDE_DIR}
                     ${PYTHON_INCLUDE_DIR}
                     ${VISION_INCLUDE_DIR}
                     ${CORPUS_INCLUDE_DIR}
                     ${CFG_DIR}
		     ${BOOST_INCLUDE_DIR}
                     )


############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project

SET( NOGGIN_SRCS Noggin
                 Observation
                 # MCL
                 BallEKF
                 PyLoc
                 # LocEKF
		 MultiMeasurementEKF
		 MultiLocEKF
		 PyConstants
		 PyObjects
		 CombinationObjects
		 ParticleFilter
                 )


SET( NOGGIN_PYTHON_SRCS __init__.py
                        Brain.py
                        GameController.py
                        GameStates.py
                        FallController.py
			Stability.py
                        FallStates.py
                        Leds.py
                        ${CMAKE_CURRENT_BINARY_DIR}/TeamConfig.py
                        ${CMAKE_CURRENT_BINARY_DIR}/WebotsConfig.py
                        )

SET ( NOGGIN_SWITCH ${CMAKE_CURRENT_BINARY_DIR}/players/Switch.py)

SET( NOGGIN_PYTHON_SUBDIRS headTracking
     			   kickDecider
                           navigator
                           playbook
                           players
                           robots
                           typeDefs
                           util )


############################ BUILD TARGETS

ADD_LIBRARY(
  ${NOGGIN_TARGET}
  STATIC
  ${NOGGIN_SRCS}
  )
#INSTALL(
#  TARGETS ${NOGGIN_TARGET}
#  ARCHIVE DESTINATION ${OUTPUT_ROOT_DIR_LIB}
#  CONFIGURATIONS Debug
#  )
INSTALL(
  FILES ${NOGGIN_PYTHON_SRCS}
  DESTINATION ${NOGGIN_PYTHON_TARGET_DIR}
  CONFIGURATIONS "" Debug Release
  )
INSTALL(
  FILES ${NOGGIN_SWITCH}
  DESTINATION ${NOGGIN_PYTHON_TARGET_DIR}/players/
  CONFIGURATIONS "" Debug Release
  )
INSTALL(
  DIRECTORY ${NOGGIN_PYTHON_SUBDIRS}
  DESTINATION ${NOGGIN_PYTHON_TARGET_DIR}/
  CONFIGURATIONS "" Debug Release
  PATTERN "*.py"
  )


############################ LINKER LANGUAGE
# For Macs, set the target link language to use

#IF( APPLE )
SET_TARGET_PROPERTIES(
    ${NOGGIN_TARGET}
    PROPERTIES LINKER_LANGUAGE CXX
    )
#ENDIF( APPLE )


############################ SET LIBRARIES TO LINK WITH
TARGET_LINK_LIBRARIES(
  ${NOGGIN_TARGET}
  ${LOG_TARGET}
  ${PYTHON_LIBRARY}
  ${VISION_TARGET}
  ${SENSORS_TARGET}
  log
  ${Boost_PYTHON_LIBRARY}
  )


############################ (SUB)DIRECTORY COMPILATION
# Set the sudirectories (some may not actually be subdirectories) to
# include in this package
#SUBDIRS(
#  ${VISION_CMAKE_DIR}
#  )

