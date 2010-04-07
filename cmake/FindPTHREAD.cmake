
##
# Includes
##

INCLUDE ( ${CMAKE_MODULE_PATH}/base_definitions.cmake )

##
# Clear variables/caches
##

SET(PTHREAD_DEFINITIONS "-pthread" )
SET(PTHREAD_INCLUDE_DIR "PTHREAD_INCLUDE_DIR-NOTFOUND" CACHE FILEPATH "Cleared." FORCE)
SET(PTHREAD_LIBRARIES "PTHREAD_LIBRARIES-NOTFOUND" CACHE FILEPATH "Cleared." FORCE)

##
# Find package requirements
##

IF (${ROBOT_PREFIX} STREQUAL nao)
  IF ( OE_CROSS_BUILD )
    SET( PTHREAD_INCLUDE_DIR ${OE_CROSS_DIR}/staging/geode-linux/usr/include )
    SET( PTHREAD_LIBRARIES ${OE_CROSS_DIR}/staging/geode-linux/lib/libpthread.so.0 )
  ELSE ( OE_CROSS_BUILD )
    IF( WIN32 )
      SET( PTHREAD_INCLUDE_DIR ${AL_DIR}/extern/c/pthread/include/ )
      SET( PTHREAD_LIBRARIES 
          ${AL_DIR}/extern/c/pthread/lib/${TARGET_ARCH}/pthreadVCE2.lib )
    ELSE ( WIN32 )
      SET( PTHREAD_INCLUDE_DIR /usr/include/ )
      IF ( APPLE )
        SET( PTHREAD_LIBRARIES /usr/lib/libpthread.dylib )
      ELSE ( APPLE )
        SET( PTHREAD_LIBRARIES ${LIB_PREFIX}/libpthread.so )
        SET( PTHREAD_LIBRARIES /usr/lib/libpthread.so )
      ENDIF( APPLE )
    ENDIF( WIN32 )
  ENDIF ( OE_CROSS_BUILD )
ENDIF (${ROBOT_PREFIX} STREQUAL nao)




IF( PTHREAD_INCLUDE_DIR AND PTHREAD_LIBRARIES )
  SET( PTHREAD_FOUND TRUE )
ENDIF( PTHREAD_INCLUDE_DIR AND PTHREAD_LIBRARIES )

IF( NOT PTHREAD_FOUND AND PTHREAD_FIND_REQUIRED )
  IF( NOT PTHREAD_INCLUDE_DIR )
		MESSAGE( STATUS "Required include not found" )
    MESSAGE( FATAL_ERROR "Could not find PTHREAD include!" )
  ENDIF( NOT PTHREAD_INCLUDE_DIR )
  IF( NOT PTHREAD_LIBRARIES )
    MESSAGE( STATUS "Required libraries not found" )
    MESSAGE( FATAL_ERROR "Could not find PTHREAD libraries!" )
  ENDIF( NOT PTHREAD_LIBRARIES )
ENDIF( NOT PTHREAD_FOUND AND PTHREAD_FIND_REQUIRED )


# Finally, display informations if not in quiet mode
IF( NOT PTHREAD_FIND_QUIETLY )
  MESSAGE( STATUS "PTHREAD found " )
  MESSAGE( STATUS "  includes   : ${PTHREAD_INCLUDE_DIR}" )
  MESSAGE( STATUS "  libraries  : ${PTHREAD_LIBRARIES}" )
  MESSAGE( STATUS "  definitions: ${PTHREAD_DEFINITIONS}" )
ENDIF( NOT PTHREAD_FIND_QUIETLY )



MARK_AS_ADVANCED(
  PTHREAD_DEFINITIONS
  PTHREAD_INCLUDE_DIR
  PTHREAD_LIBRARIES
)
