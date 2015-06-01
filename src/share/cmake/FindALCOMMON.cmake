# Finds Aldebaran's libraries

if( IS_DIRECTORY "${NBITES_DIR}/lib/atomtoolchain/libnaoqi-sysroot/" )
  set ( AL_DIR "${NBITES_DIR}/lib/naoqi-sdk-2.1.0.19-linux32" )
else()
  set ( AL_DIR "${NBITES_DIR}/lib/naoqi-sdk-1.14.5-linux32" )
endif()

#set( AL_DIR $ENV{AL_DIR} )
message(STATUS "Set ${AL_DIR} as the SDK.")
message(STATUS "If this is not correct, edit your nbites.bash.")

if( NOT OFFLINE )
  set( ALCOMMON_INCLUDE_DIR ${OE_SYSROOT}/include/ )
else()
  set( ALCOMMON_INCLUDE_DIR ${AL_DIR}/include/ )
endif()

if( NOT OFFLINE )
  if( IS_DIRECTORY "${NBITES_DIR}/lib/atomtoolchain/libnaoqi-sysroot/" )
    set( ALCOMMON_LIBRARIES
      ${OE_SYSROOT}/lib/libalcommon.so
      ${OE_SYSROOT}/lib/libalmemoryfastaccess.so)
  else()
    set( ALCOMMON_LIBRARIES
      ${OE_SYSROOT}/usr/lib/libalcommon.so
      ${OE_SYSROOT}/usr/lib/libalmemoryfastaccess.so)
  endif()
else()
  set( CMAKE_PREFIX_PATH ${AL_DIR} )
  set( ALCOMMON_LIBRARIES
    ${AL_DIR}/lib/libalcommon.so
    ${AL_DIR}/lib/libalmemoryfastaccess.so)
endif()

if( NOT ALCOMMON_FOUND_TRUE AND ALCOMMON_FIND_REQUIRED )
    if( NOT ALCOMMON_INCLUDE_DIR )
        message( STATUS "Required include not found" )
        message( FATAL_ERROR "Could not find ALCOMMON include!")
    endif( NOT ALCOMMON_INCLUDE_DIR )
    if( NOT ALCOMMON_LIBRARIES )
        message( STATUS "Required libraries not found" )
        message( FATAL_ERROR "Could not find ALCOMMON libraries!")
    endif( NOT ALCOMMON_LIBRARIES )
endif( NOT ALCOMMON_FOUND_TRUE AND ALCOMMON_FIND_REQUIRED )

message( STATUS "ALCOMMON found " )
message( STATUS "  includes   : ${ALCOMMON_INCLUDE_DIR}" )
message( STATUS "  libraries  : ${ALCOMMON_LIBRARIES}" )

mark_as_advanced(
  ALCOMMON_DEFINITIONS
  ALCOMMON_INCLUDE_DIR
  ALCOMMON_LIBRARIES
)
