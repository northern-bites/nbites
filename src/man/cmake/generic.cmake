
########################## DEFINES & DEFINITIONS
SET( STRAIGHT_BUILD ON )

########################## PLATFORM
IF( WIN32 )
  SET( TARGET_ARCH "windows" )
  SET( TARGET_HOST "TARGET_HOST_WINDOWS")
ENDIF( WIN32 )

IF( UNIX )
  SET( TARGET_ARCH "linux")
  SET( TARGET_HOST "TARGET_HOST_LINUX")
  SET( PLATFORM_X86 1 )
ENDIF( UNIX )

IF( APPLE )
  SET( TARGET_ARCH "macosx" )
  SET( TARGET_HOST "TARGET_HOST_MACOSX")
ENDIF( APPLE )

########################## FIND PATH
# we should look in the naoqi sdk folder for the libraries we need first
# so we use the same version as on the robot
SET( CMAKE_FIND_ROOT_PATH  $ENV{NBITES_DIR}/ext/ )

# search for programs in the system root
SET( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH )
# for libraries and headers in the nao sdk preferably, if not defaults to
# system root
SET( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH )
SET( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH )
SET( CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH )
