
# Set options that are constrained by building for atom
set( OFFLINE OFF )

# Find the NBITES_DIR
include(${CMAKE_CURRENT_LIST_DIR}/FindNBITES_DIR.cmake)

############################# CROSS-COMPILATION VARIABLES
# Set the variable for the cross-compilation directory, cmake variables
set( TOOLCHAIN_DIR "${NBITES_DIR}/lib/atomtoolchain" )
set( OE_SYSROOT "${TOOLCHAIN_DIR}/sysroot/" )

set( CMAKE_CROSSCOMPILING   TRUE  )
set( CMAKE_SYSTEM_NAME      Linux )
set( CMAKE_SYSTEM_VERSION   1     )
set( CMAKE_SYSTEM_PROCESSOR atom  )

include("${TOOLCHAIN_DIR}/toolchain-atom.cmake")

set( CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -march=core2 -mtune=generic -mssse3 -mfpmath=sse -fomit-frame-pointer -pipe")
set( CMAKE_C_FLAGS
  "${CMAKE_CXX_FLAGS}" )

# where should we look for libraries we need
set(CMAKE_FIND_ROOT_PATH ${OE_SYSROOT})

# search for programs in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
# for libraries and headers in the target directories
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
