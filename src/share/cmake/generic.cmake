
# Set options that are constrained by building straight
set( OFFLINE ON )

# Find the NBITES_DIR
include(${CMAKE_CURRENT_LIST_DIR}/FindNBITES_DIR.cmake)

# This is the version the robots use
set( CMAKE_USE_PYTHON_VERSION 2.6 )

#### Find Path
# We should look in the naoqi sdk folder for the libraries we need first
# so we use the same version as on the robot

# The order matters - this forces cmake to first look in
# nbites_dir/ext, then the regular system folders
set( CMAKE_FIND_ROOT_PATH  ${NBITES_DIR}/lib /usr/)
# search for programs in the system root
set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY )

# for libraries and headers look in the nao sdk preferably
# if not defaults to system root
set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY )