SET( TINYXML_DEFINITIONS "" )

INCLUDE( "${CMAKE_MODULE_PATH}/base_definitions.cmake" )

##
# Includes
##


#FOR NAOQI 0.18 back compatability
IF( NAOQI1.0 STREQUAL "ON")

## from ${AL_DIR}/cmakemodules/FindTINYXML.cmake
include("${AL_DIR}/cmakemodules/aldebaran.cmake")
include("${AL_DIR}/cmakemodules/libfind.cmake")

clean(TINYXML)

set(TINYXML_INCLUDE_DIR ${AL_DIR}/extern/c/src/tinyxml)
if(TARGET_HOST STREQUAL "TARGET_HOST_WINDOWS")
  SET( TINYXML_LIBRARIES ${AL_DIR}/extern/c/${TARGET_ARCH}/lib/tinyxml.lib)
else(TARGET_HOST STREQUAL "TARGET_HOST_WINDOWS")
  SET( TINYXML_LIBRARIES ${AL_DIR}/extern/c/${TARGET_ARCH}/lib/libtinyxml.a)
endif(TARGET_HOST STREQUAL "TARGET_HOST_WINDOWS")

export(TINYXML)



ELSE( NAOQI1.0 STREQUAL "ON")
SET(TINYXML_REL_PATH /extern/c/tinyxml/lib)
SET( TINYXML_INCLUDE_DIR ${AL_DIR}/extern/c/tinyxml )


#MESSAGE(ERROR "${TINYXML_REL_PATH}")
IF( OE_CROSS_BUILD )
    SET( TINYXML_LIBRARIES
      ${AL_DIR}${TINYXML_REL_PATH}/${TARGET_ARCH}/libtinyxml.a
    )
ELSE( OE_CROSS_BUILD )
    IF( WIN32 )
        SET( TINYXML_LIBRARIES
        ${AL_DIR}${TINYXML_REL_PATH}/${TARGET_ARCH}/tinyxml.lib
        )
    ELSE( WIN32 )
        IF( APPLE )
            MESSAGE( STATUS "Remote objects have not been setup for Mac's yet - jfishman@" )
        ELSE( APPLE )
            SET( TINYXML_LIBRARIES
              ${AL_DIR}${TINYXML_REL_PATH}/${TARGET_ARCH}//libtinyxml.a
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
ENDIF( NAOQI1.0 STREQUAL "ON")