SET( ALCOMMON_DEFINITIONS "" )

##
# Includes
##

INCLUDE( "${CMAKE_MODULE_PATH}/base_definitions.cmake" )
IF(WEBOTS_BACKEND)
  SET( ALCOMMON_INCLUDE_DIR ${AL_DIR}/extern/c/aldebaran/alcommon/include
    ${AL_DIR}/extern/c/aldebaran/alcommon/interface
    ${AL_DIR}/extern/c/aldebaran/alcommon/soap
    ${AL_DIR}/modules/proxies
    )
ELSE(WEBOTS_BACKEND)
  IF(OE_CROSS_BUILD)
  SET( ALCOMMON_INCLUDE_DIR ${OE_SYSROOT}/usr/include/alcommon/include
    ${OE_SYSROOT}/usr/include/alcommon/interface
    ${OE_SYSROOT}/usr/include/alcommon/soap
    ${OE_SYSROOT}/usr/include/alproxies
    ${OE_SYSROOT}/usr/include/alcommon
    ${OE_SYSROOT}/usr/include/alcore
    ${OE_SYSROOT}/usr/include/libthread
    ${OE_SYSROOT}/usr/include/alvalue
    ${OE_SYSROOT}/usr/include/altools
    ${OE_SYSROOT}/usr/include/alfactory
    ${OE_SYSROOT}/usr/include
    )
  ELSE(OE_CROSS_BUILD)
  SET( ALCOMMON_INCLUDE_DIR ${AL_DIR}/include/alcommon/include
    ${AL_DIR}/include/alcommon/interface
    ${AL_DIR}/include/alcommon/soap
    ${AL_DIR}/include/alproxies
    ${AL_DIR}/include/alcommon
    ${AL_DIR}/include/alcore
    ${AL_DIR}/include/libthread
    ${AL_DIR}/include/alvalue
    ${AL_DIR}/include/altools
    ${AL_DIR}/include/alfactory
    ${AL_DIR}/include
    )
  ENDIF(OE_CROSS_BUILD)
ENDIF(WEBOTS_BACKEND)

    IF( OE_CROSS_BUILD )
        SET( ALCOMMON_LIBRARIES
          ${OE_SYSROOT}/usr/lib/libalcommon.so )
    ELSE(OE_CROSS_BUILD )
        IF (APPLE)
            IF(WEBOTS_BACKEND)
                SET( ALCOMMON_LIBRARIES
                  ${AL_DIR}/extern/c/aldebaran/alcommon/lib/${TARGET_ARCH}/libalcommon.a
                  )
            ELSE(WEBOTS_BACKEND)
                SET( ALCOMMON_LIBRARIES
              ${AL_DIR}/lib/libalcommon.a
              )
            ENDIF(WEBOTS_BACKEND)
        ELSE(APPLE)
            IF(WEBOTS_BACKEND)
                SET( ALCOMMON_LIBRARIES
                  ${AL_DIR}/extern/c/aldebaran/alcommon/lib/${TARGET_ARCH}/libalcommon.a
                  )
            ELSE(WEBOTS_BACKEND)
                SET( ALCOMMON_LIBRARIES
              ${AL_DIR}/lib/libalcommon.so
              )
            ENDIF(WEBOTS_BACKEND)
        ENDIF(APPLE)
    ENDIF( OE_CROSS_BUILD )

IF( ALCOMMON_LIBRARIES AND EXISTS ${ALCOMMON_LIBRARIES} )
    SET( ALCOMMON_FOUND TRUE )
ENDIF( ALCOMMON_LIBRARIES AND EXISTS ${ALCOMMON_LIBRARIES} )

IF( NOT ALCOMMON_FOUND_TRUE AND ALCOMMON_FIND_REQUIRED )
    IF( NOT ALCOMMON_INCLUDE_DIR )
        MESSAGE( STATUS "Required include not found" )
        MESSAGE( FATAL_ERROR "Could not find ALCOMMON include!")
    ENDIF( NOT ALCOMMON_INCLUDE_DIR )
    IF( NOT ALCOMMON_LIBRARIES )
        MESSAGE( STATUS "Required libraries not found" )
        MESSAGE( FATAL_ERROR "Could not find ALCOMMON libraries!")
    ENDIF( NOT ALCOMMON_LIBRARIES )
ENDIF( NOT ALCOMMON_FOUND_TRUE AND ALCOMMON_FIND_REQUIRED )

##
# Finally, display informations if not in quiet mode
##

IF( NOT ALCOMMON_FIND_QUIETLY )
  MESSAGE( STATUS "ALCOMMON found " )
  MESSAGE( STATUS "  includes   : ${ALCOMMON_INCLUDE_DIR}" )
  MESSAGE( STATUS "  libraries  : ${ALCOMMON_LIBRARIES}" )
  MESSAGE( STATUS "  definitions: ${ALCOMMON_DEFINITIONS}" )
ENDIF( NOT ALCOMMON_FIND_QUIETLY )



MARK_AS_ADVANCED(
  ALCOMMON_DEFINITIONS
  ALCOMMON_INCLUDE_DIR
  ALCOMMON_LIBRARIES
)
