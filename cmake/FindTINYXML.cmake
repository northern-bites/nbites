SET( TINYXML_DEFINITIONS "" )

INCLUDE( "${CMAKE_MODULE_PATH}/base_definitions.cmake" )

##
# Includes
##
SET( TINYXML_INCLUDE_DIR ${AL_DIR}/extern/c/tinyxml )

IF( OE_CROSS_BUILD )
    SET( TINYXML_LIBRARIES
        ${AL_DIR}/extern/c/tinyxml/lib/linux/libtinyxml.a
    )
ELSE( OE_CROSS_BUILD )
    IF( WIN32 )
        SET( TINYXML_LIBRARIES
        ${AL_DIR}/extern/c/tinyxml/lib/${TARGET_ARCH}/tinyxml.lib
        )
    ELSE( WIN32 )
        IF( APPLE )
            MESSAGE( STATUS "Remote objects have not been setup for Mac's yet - jfishman@" )
        ELSE( APPLE )
            SET( TINYXML_LIBRARIES
                ${AL_DIR}/extern/c/tinyxml/lib/${TARGET_ARCH}/libtinyxml.a
            )
        ENDIF( APPLE )
    ENDIF( WIN32 )
ENDIF( OE_CROSS_BUILD )

IF( EXISTS ${TINYXML_LIBRARIES} )
    SET( TINYXML_FOUND TRUE )
ENDIF( EXISTS ${TINYXML_LIBRARIES} )

IF( NOT TINYXML_FOUND_TRUE AND TINYXML_FIND_REQUIRED )
    IF( NOT TINYXML_INCLUDE_DIR )
        MESSAGE( STATUS "Required include not found" )
        MESSAGE( FATAL_ERROR "Could not find TINYXML include!")
    ENDIF( NOT TINYXML_INCLUDE_DIR )
    IF( NOT TINYXML_LIBRARIES )
        MESSAGE( STATUS "Required libraries not found" )
        MESSAGE( FATAL_ERROR "Could not find TINYXML libraries!")
    ENDIF( NOT TINYXML_LIBRARIES )
ENDIF( NOT TINYXML_FOUND_TRUE AND TINYXML_FIND_REQUIRED )


# Finally, display informations if not in quiet mode
IF( NOT TINYXML_FIND_QUIETLY )
  MESSAGE( STATUS "TINYXML found " )
  MESSAGE( STATUS "  includes   : ${TINYXML_INCLUDE_DIR}" )
  MESSAGE( STATUS "  libraries  : ${TINYXML_LIBRARIES}" )
  MESSAGE( STATUS "  definitions: ${TINYXML_DEFINITIONS}" )
ENDIF( NOT TINYXML_FIND_QUIETLY )


MARK_AS_ADVANCED(
  TINYXML_DEFINITIONS
  TINYXML_INCLUDE_DIR
  TINYXML_LIBRARIES
)
