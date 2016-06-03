cmake_minimum_required( VERSION 2.8.3 )

set(LOG_SRCS Commands.cpp Control Filesystem flags.defs.h Logging Network )

# Enable C++11 features
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

set( CMAKE_CXX_COMPILER /usr/bin/g++ )

add_library(
  logging
  STATIC
  ${LOG_SRCS} )

target_link_libraries(
  logging
  logshare
  portals
  messages
)