# Finds Aldebaran's libraries

set( AL_DIR $ENV{AL_DIR} )
message(STATUS "Set $ENV{AL_DIR}/ as the SDK.")
message(STATUS "If this is not correct, edit your nbites.bash.")

if( NOT OFFLINE )
  set( ALCOMMON_INCLUDE_DIR ${OE_SYSROOT}/usr/include/ )
else()
  set( ALCOMMON_INCLUDE_DIR ${AL_DIR}/include/ )
endif()

if( NOT OFFLINE )
  set( ALCOMMON_LIBRARIES
    ${OE_SYSROOT}/usr/lib/libalcommon.so
    ${OE_SYSROOT}/usr/lib/libalmemoryfastaccess.so)
else()
  set( ALCOMMON_LIBRARIES
    ${AL_DIR}/lib/libalcommon.so
    ${AL_DIR}/lib/libalmemoryfastaccess.so)
endif()

# if( EXISTS "${ALCOMMON_LIBRARIES}" AND ALCOMMON_LIBRARIES )
#     set( ALCOMMON_FOUND TRUE )
# endif( EXISTS "${ALCOMMON_LIBRARIES}" AND ALCOMMON_LIBRARIES )

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