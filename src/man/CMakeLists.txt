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
CMAKE_MINIMUM_REQUIRED( VERSION 2.8.3 )

ENABLE_TESTING()

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
MARK_AS_ADVANCED( Boost_DIR )

include ( cmake/FindALCOMMON.cmake )

include(../cmake/protobuf.cmake)
include_directories(${PROTOBUF_INCLUDE_DIR})

############################ BUILDCONFIG
# Include the config file for this build
INCLUDE( ${MAN_CMAKE_DIR}/buildconfig.cmake )

############################ CONFIG.H GENERATION
CONFIGURE_FILE(
  ${MAN_CMAKE_DIR}/manconfig.in
  ${CFG_DIR}/manconfig.h
  ESCAPE_QUOTES
  )

SET( @REMOTE_ADDRESS@
  "mal.local"
  CACHE STRING
  "The remote address of the robot"
  )
SET( REMOTE_ADDRESS ${@REMOTE_ADDRESS@} )

############################ COLOR TABLE SELECTION
# Set the path to the color table you wish to be loaded into install/etc
SET( @TABLE_TOP@
  "../../data/tables/120-top.mtb"
  CACHE STRING
  "Path relative to man directory for the desired top camera color table"
  )
SET( TABLE_TOP ${@TABLE_TOP@})

SET( @TABLE_BOTTOM@
  "../../data/tables/120-bottom.mtb"
  CACHE STRING
  "Path relative to man directory for the desired bottom camera color table"
  )
SET( TABLE_BOTTOM ${@TABLE_BOTTOM@})

MARK_AS_ADVANCED(CCACHE)
MARK_AS_ADVANCED(I_AM_A_ROBOT)

#upload.sh needs to get generated after REMOTE and OFFLINE stuff is set
CONFIGURE_FILE(
  ${MAN_CMAKE_DIR}/upload.sh.in
  ${CMAKE_CURRENT_BINARY_DIR}/upload.sh
  ESCAPE_QUOTES
  )

IF ( NOT USE_ALSPEECH )
  SET( USE_ALSPEECH_POUND "#" )
ELSE()
  SET( USE_ALSPEECH_POUND "" )
ENDIF()

############################ DEFINITIONS
# Add here definition needed.
ADD_DEFINITIONS(
  ${CMAKE_THREAD_LIBS_INIT}
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
  ${BHWALK_INCLUDE_DIR}
  ${NOGGIN_INCLUDE_DIR}
  ${VISION_INCLUDE_DIR}
  ${MEMORY_INCLUDE_DIR}
  ${PROJECT_BINARY_DIR}
  ${CFG_DIR}
  )

############################ Platform settings

IF(BUILDING_FOR_A_REMOTE_NAO)
  SET( MAN_LOADER_SRC NaoManLoader )
ENDIF()

############################ Setting up Man
SET( MAN_SRCS Man
              TMan
              TTMan
              ${MAN_LOADER_SRC} )

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
  ${ROBOT_CONNECT_TARGET}
  ${NBINCLUDE_TARGET}
  ${VISION_TARGET}
  ${MOTION_TARGET}
  ${NOGGIN_TARGET}
  ${MEMORY_TARGET}
  portals
  )

############################# INSTALL PATHS

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
add_subdirectory( portals )
