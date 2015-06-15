# Main CMake file for the Tool!
project( nbcross )
cmake_minimum_required( VERSION 2.8.3 )

# Make sure we can find the shared files
set( SHARE_DIR "${NBITES_DIR}/src/share/" )
set( MAN_DIR "${NBITES_DIR}/src/man" )
include_directories( ${MAN_DIR} )

# So that we can use find_package commands
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${SHARE_DIR}/cmake/" )
mark_as_advanced( CMAKE_INSTALL_PREFIX )
mark_as_advanced( CMAKE_TOOLCHAIN_FILE )

set( CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -pthread" )

set( CMAKE_C_FLAGS
  "${CMAKE_CXX_FLAGS}" )

# Enable C++11 features for bhuman
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# Option to turn rebuilding our protobuf libs on/off
option( BUILD_PROTOBUF "Rebuild the protobuf libraries." OFF )
if( BUILD_PROTOBUF )
  # Only build them if the user wants to
  include( ${SHARE_DIR}/cmake/BuildProtobuf.cmake )
endif()

include ( ${SHARE_DIR}/cmake/BuildGTest.cmake )

include( ${SHARE_DIR}/cmake/DebugConfig.cmake)


# Configure the debug options
configure_file(
  "${SHARE_DIR}/cmake/DebugConfig.in"
  "${PROJECT_BINARY_DIR}/DebugConfig.h"
)

configure_file(
  "${MAN_DIR}/cmake/RobotConfig.in"
  "${PROJECT_BINARY_DIR}/RobotConfig.h"
)


if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, default to RelWithDebInfo")
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()


include_directories( ${PROJECT_BINARY_DIR} )
include_directories( ${CMAKE_SOURCE_DIR} )


find_package( PROTOBUF REQUIRED )
include_directories( SYSTEM ${PROTOBUF_INCLUDE_DIR} )

# Our core includes come from here
include_directories( ${SHARE_DIR}/include )

# Compile portals into portals folder
add_subdirectory( ${SHARE_DIR}/portals ${CMAKE_CURRENT_BINARY_DIR}/portals )
include_directories( ${SHARE_DIR}/portals )

# Compile logshare into logshare folder
add_subdirectory( ${SHARE_DIR}/logshare ${CMAKE_CURRENT_BINARY_DIR}/logshare )
include_directories( ${SHARE_DIR}/logshare )

# Compile messages into messages folder
add_subdirectory( ${SHARE_DIR}/messages ${CMAKE_CURRENT_BINARY_DIR}/messages )
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/messages )
include_directories( ${SHARE_DIR}/messages )

# If protobuf libs don't exist yet, make sure they build before man
if( BUILD_PROTOBUF )
add_dependencies(messages protobuf_libs)
endif()

# Man modules that we need to use
include_directories( ${MAN_DIR}/support )
add_subdirectory( ${MAN_DIR}/support ${CMAKE_CURRENT_BINARY_DIR}/man/support)
add_subdirectory( ${MAN_DIR}/vision ${CMAKE_CURRENT_BINARY_DIR}/man/vision)
add_subdirectory( ${MAN_DIR}/balltrack ${CMAKE_CURRENT_BINARY_DIR}/man/balltrack)
add_subdirectory( ${MAN_DIR}/comm ${CMAKE_CURRENT_BINARY_DIR}/man/comm)
add_subdirectory( ${MAN_DIR}/sharedball ${CMAKE_CURRENT_BINARY_DIR}/man/sharedball)
add_subdirectory( ${MAN_DIR}/localization ${CMAKE_CURRENT_BINARY_DIR}/man/localization)
add_subdirectory( ${MAN_DIR}/log ${CMAKE_CURRENT_BINARY_DIR}/man/log)
add_subdirectory( ${MAN_DIR}/control ${CMAKE_CURRENT_BINARY_DIR}/man/control)

## SOURCES ##
set( NBCROSS_SRCS
  main.cpp
  misc_defs.cpp
  pkoch_defs.cpp
  vision_defs.cpp
)

set( NBCROSS_HEADERS
  nbfuncs.h
)

add_library(nbcross
  STATIC
  ${NBCROSS_SRCS}
  ${NBCROSS_HEADERS}
)

## EXECUTABLE ##
add_executable( nbcross_tool
  ${NBCROSS_SRCS}
  ${NBCROSS_HEADERS} )

target_link_libraries( nbcross_tool
  portals
  messages
  vision
  comm
  sharedball
  log
  logshare
  control
  )
