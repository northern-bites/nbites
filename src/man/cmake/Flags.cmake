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
