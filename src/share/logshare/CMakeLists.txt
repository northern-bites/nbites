cmake_minimum_required( VERSION 2.8.3 )

set( LOGSHARE_SRCS json Log LogRPC nblogio SExpr utilities utilities-pp.hpp utilities-test.hpp )

# Enable C++11 features
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    message(STATUS "logshare using c++11 compiler")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
    message(STATUS "logshare using c++0x compiler")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set( CMAKE_CXX_COMPILER /usr/bin/g++ )

message(STATUS "Using compiler ${CMAKE_CXX_COMPILER} to compile logshare.")
message(STATUS "Using flags ${CMAKE_CXX_FLAGS} to compile logshare.")

add_library(
  logshare
  STATIC
  ${LOGSHARE_SRCS}
)
