##############################
#
# WELCOME
#
# This is the main cmake configuration file
# associate to your projects
# You can define here specific compilation options
#
# More about cmake : www.cmake.org
#
# Using cmake command line :
# 1. Create a build directory : mkdir build
# 2. Configure cmake project : cd build; cmake ..
#   You can also generate project with your favorite IDE :
#   Example : Under Mac osx use "cmake .. -G XCode"
#   see cmake help for more information
# 3. Edit project option : ccmake .
# 4. Build it : make

############################ PROJECT NAME
# The name of this cmake project
PROJECT( MAN )
CMAKE_MINIMUM_REQUIRED( VERSION 2.6.0 )


############################ BASE DEFINITIONS
# Include  the basic definitions accross all projects
INCLUDE( cmake/base_definitions.cmake )


############################ FIND PACKAGE
# Libraries specifics. Define here cmake packages
# usefull to your project
FIND_PACKAGE( PythonLibs  REQUIRED )

#SET(CMAKE_THREADS_PREFER_PTHREADS ON)
#FIND_PACKAGE( Threads  REQUIRED )

SET(Boost_USE_MULTITHREADED ON)
SET(Boost_USE_STATIC_LIBS OFF)
FIND_PACKAGE( Boost REQUIRED )

IF(WEBOTS_BACKEND)
  include ( cmake/FindWEBOTS.cmake )
ELSE(WEBOTS_BACKEND)
  include ( cmake/FindALCOMMON.cmake )
ENDIF(WEBOTS_BACKEND)

############################ BUILDCONFIG
# Include the config file for this build
INCLUDE( ${MAN_CMAKE_DIR}/buildconfig.cmake )

############################ CONFIG.H GENERATION
CONFIGURE_FILE(
  ${MAN_CMAKE_DIR}/manconfig.in
  ${CFG_DIR}/manconfig.h
  ESCAPE_QUOTES
  )


############################ OFFLINE
SET( OFFLINE_NAOQI_PATH
  $ENV{AL_DIR}
  CACHE STRING
  "The local path of the naoqi distribution to use with Webots"
  )

############################ REMOTE INSTALLATION
# Define this to remotely install the binary or library on the robot.
# Cannot be selected unless OE_CROSS_BUILD is set to ON.
IF(WEBOTS_BACKEND)
  OPTION( REMOTE_INSTALL
    "Remotely install the libary or binary on the robot."
    OFF
    )
ELSE(WEBOTS_BACKEND)
  OPTION( REMOTE_INSTALL
    "Remotely install the libary or binary on the robot."
    ON
    )
ENDIF(WEBOTS_BACKEND)

SET( @REMOTE_ADDRESS@
  "192.168.0.5"
  CACHE STRING
  "The remote address of the robot"
  )
SET( REMOTE_ADDRESS ${@REMOTE_ADDRESS@} )

############################ REMOTE INSTALLATION
# Define this to remotely install the binary or library on the robot.
# Cannot be selected unless OE_CROSS_BUILD is set to ON.
SET( @ROBOT_NAME@
  "spock"
  CACHE STRING
  "The NAME of the robot"
  )
SET( ROBOT_NAME ${@ROBOT_NAME@} )


############################ COLOR TABLE SELECTION
# Set the path to the color table you wish to be loaded into install/etc
SET( COLOR_TABLE
  "../../data/tables/lab120/lab120-11.mtb"
  CACHE STRING
  "Path relative to man directory for the desired color table"
  )

#upload.sh needs to get generated after REMOTE and OFFLINE stuff is set
CONFIGURE_FILE(
  ${MAN_CMAKE_DIR}/upload.sh.in
  ${CMAKE_CURRENT_BINARY_DIR}/upload.sh
  ESCAPE_QUOTES
  )

CONFIGURE_FILE(
  ${MAN_CMAKE_DIR}/nameconfig.in
  ${CFG_DIR}/nameconfig.h
  ESCAPE_QUOTES
  )

IF ( NOT USE_ALSPEECH )
  SET( USE_ALSPEECH_POUND "#" )
ELSE()
  SET( USE_ALSPEECH_POUND "" )
ENDIF()

SET( AUTOLOAD_INI ${CFG_DIR}/autoload.ini )

CONFIGURE_FILE(
  ${MAN_CMAKE_DIR}/autoload.ini.in
  ${CFG_DIR}/autoload.ini
  ESCAPE_QUOTES
  )

############################ DEFINITIONS
# Add here definition needed.
ADD_DEFINITIONS(
  ${CMAKE_THREAD_LIBS_INIT}
  ${WEBOTS_DEFINITIONS}
  )


############################ INCLUDE DIRECTORY
# Define include directories
INCLUDE_DIRECTORIES(
  ${NBCOMMON_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
  ${PYTHON_INCLUDE_PATH}
  ${ALCOMMON_INCLUDE_DIR}
  ${MINGW_INCLUDE_DIR}
  ${MAN_INCLUDE_DIR}
  ${COMM_INCLUDE_DIR}
  ${CORPUS_INCLUDE_DIR}
  ${MOTION_INCLUDE_DIR}
  ${NOGGIN_INCLUDE_DIR}
  ${VISION_INCLUDE_DIR}
  ${WEBOTS_INCLUDE_DIR}
  ${MEMORY_INCLUDE_DIR}
  ${PROJECT_BINARY_DIR}
  ${CFG_DIR}
  )

############################ Setting up Man
SET( MAN_SRCS Man
              TMan
              TTMan
              NaoManLoader )

IF(WEBOTS_BACKEND)
#NOTE: this doesn't work anymore because we changed the way we
# load man - Octavian
  LIST(APPEND MAN_SRCS nao_soccer_player
    )
ENDIF()

SET( MAN_PYTHON_SRCS __init__.py )

ADD_LIBRARY(
  ${MAN_TARGET}
  SHARED
  ${MAN_SRCS}
  )

TARGET_LINK_LIBRARIES(
  ${MAN_TARGET}
  ${COMM_TARGET}
  ${SENSORS_TARGET}
  ${ROBOT_INTERFACE_TARGET}
  ${NBINCLUDE_TARGET}
  ${VISION_TARGET}
  ${MOTION_TARGET}
  ${NOGGIN_TARGET}
  ${MEMORY_TARGET}
  ${WEBOTS_LIBRARIES}
  )

############################## Setting up Man Preloader

IF(BUILDING_FOR_A_REMOTE_NAO)
  SET(MAN_PRELOADER_SRCS NaoManPreloader)
ELSE()
  SET(MAN_PRELOADER_SRCS FakeManPreloader)
ENDIF()

ADD_LIBRARY(
  ${MAN_PRELOADER_TARGET}
  SHARED
  ${MAN_PRELOADER_SRCS}
  )

TARGET_LINK_LIBRARIES(
  ${MAN_PRELOADER_TARGET}
  ${NBINCLUDE_TARGET}
  ${SENSORS_TARGET}
  ${ROBOT_INTERFACE_TARGET}
  ${ROBOT_CONNECT_TARGET}
  )

############################# INSTALL PATHS

INSTALL(
  TARGETS ${MAN_PRELOADER_TARGET}
  RUNTIME DESTINATION ${OUTPUT_ROOT_DIR_BIN}
  LIBRARY DESTINATION ${OUTPUT_ROOT_DIR_NAOQI_LIB}
  CONFIGURATIONS "" Debug Release
  )
INSTALL(
  TARGETS ${MAN_TARGET}
  RUNTIME DESTINATION ${OUTPUT_ROOT_DIR_BIN}
  LIBRARY DESTINATION ${OUTPUT_ROOT_DIR_LIB}
  CONFIGURATIONS "" Debug Release
  )
INSTALL(
  FILES ${MAN_PYTHON_SRCS}
  DESTINATION ${MAN_PYTHON_TARGET_DIR}
  CONFIGURATIONS "" Debug Release
  )
INSTALL(
  FILES ${AUTOLOAD_INI}
  DESTINATION ${OUTPUT_ROOT_DIR_PREF}
  CONFIGURATIONS "" Debug Release
  )

############################ (SUB)DIRECTORY COMPILATION
# Set the sudirectories to
# include in this package
add_subdirectory( memory )
add_subdirectory( comm )
add_subdirectory( corpus )
add_subdirectory( vision )
add_subdirectory( motion )
add_subdirectory( noggin )
add_subdirectory( include )