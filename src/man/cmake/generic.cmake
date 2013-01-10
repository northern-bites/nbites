
include("${CMAKE_CURRENT_LIST_DIR}/find_nbites_dir.cmake")

SET( OFFLINE ON )
SET( BUILDING_FOR_A_REMOTE_NAO OFF )

######## PyVersion
### Note: the robots currently use 2.6, so this is to make sure cmake
### looks for the right thing
SET( CMAKE_USE_PYTHON_VERSION 2.6 )

########################## FIND PATH
# we should look in the naoqi sdk folder for the libraries we need first
# so we use the same version as on the robot

# The order matters - this forces cmake to first look in
# nbites_dir/ext, then the regular system folders
SET( CMAKE_FIND_ROOT_PATH  ${NBITES_EXT} /usr/)

# search for programs in the system root
SET( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY )
# for libraries and headers in the nao sdk preferably, if not defaults to
# system root
SET( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
SET( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )
SET( CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY )
