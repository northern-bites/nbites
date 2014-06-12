# Flags for gcc

# Default (no release specific) build flags
set( CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -m32 -Wall -Wconversion -Wno-unused -Wno-write-strings -fno-strict-aliasing -fPIC")
set( CMAKE_C_FLAGS
  "${CMAKE_CXX_FLAGS}" )
# Release build flags
set( CMAKE_CXX_FLAGS_RELEASE
  "-O3 -DNDEBUG")
set( CMAKE_C_FLAGS_RELEASE
  "${CMAKE_CXX_FLAGS_RELEASE}" )
# Debug build flags
set( CMAKE_CXX_FLAGS_DEBUG
  "-g3" )
set( CMAKE_C_FLAGS_DEBUG
  "${CMAKE_CXX_FLAGS_DEBUG}" )

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
