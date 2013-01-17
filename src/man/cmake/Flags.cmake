# Flags for gcc

# Default (no release specific) build flags
SET( CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -m32 -Wall -Wconversion -Wno-unused -Wno-write-strings -fno-strict-aliasing -fPIC")
SET( CMAKE_C_FLAGS
  "${CMAKE_CXX_FLAGS}" )
# Release build flags
SET( CMAKE_CXX_FLAGS_RELEASE
  "-O3 -DNDEBUG")
SET( CMAKE_C_FLAGS_RELEASE
  "${CMAKE_CXX_FLAGS_RELEASE}" )
# Debug build flags
SET( CMAKE_CXX_FLAGS_DEBUG
  "-g3" )
SET( CMAKE_C_FLAGS_DEBUG
  "${CMAKE_CXX_FLAGS_DEBUG}" )
