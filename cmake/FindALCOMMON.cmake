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
ENDIF(WEBOTS_BACKEND)

IF( WIN32 )
    IF ( EXISTS "${AL_DIR}/extern/c/aldebaran/alcommon/lib/${TARGET_ARCH}/alcommond.lib" )
        SET( ALCOMMON_LIBRARIES_RELEASE
            ${AL_DIR}/extern/c/aldebaran/alcommon/lib/${TARGET_ARCH}/alcommon.lib
        )
        SET( ALCOMMON_LIBRARIES_DEBUG
            ${AL_DIR}/extern/c/aldebaran/alcommon/lib/${TARGET_ARCH}/alcommond.lib
        )

        IF( CMAKE_BUILD_TYPE EQUAL DEBUG)
            SET( ALCOMMON_LIBRARIES	${ALCOMMON_LIBRARIES_DEBUG} )
        ELSE( CMAKE_BUILD_TYPE EQUAL DEBUG)
            SET( ALCOMMON_LIBRARIES	${ALCOMMON_LIBRARIES_RELEASE} )
        ENDIF( CMAKE_BUILD_TYPE EQUAL DEBUG)
    ELSE( EXISTS "${AL_DIR}/extern/c/aldebaran/alcommon/lib/${TARGET_ARCH}/alcommond.lib" )
        SET( ALCOMMON_LIBRARIES
            ${AL_DIR}/extern/c/aldebaran/alcommon/lib/${TARGET_ARCH}/alcommon.lib
        )
        SET( ALCOMMON_LIBRARIES_RELEASE
	  ${AL_DIR}/extern/c/aldebaran/alcommon/lib/${TARGET_ARCH}/alcommon.lib
        )

        SET( ALCOMMON_LIBRARIES_DEBUG
            ${AL_DIR}/extern/c/aldebaran/alcommon/lib/${TARGET_ARCH}/alcommon.lib
        )
    ENDIF ( EXISTS "${AL_DIR}/extern/c/aldebaran/alcommon/lib/${TARGET_ARCH}/alcommond.lib" )

#    MESSAGE( "ALCOMMON_LIBRARIES is set to " ${ALCOMMON_LIBRARIES} )

ELSE( WIN32 )
    IF( OE_CROSS_BUILD )
        SET( ALCOMMON_LIBRARIES
          ${OE_CROSS_DIR}/staging/geode-linux/usr/lib/libalcommon.so
          )
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

ENDIF( WIN32 )

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
