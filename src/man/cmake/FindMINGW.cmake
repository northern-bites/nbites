##
# Includes
##

INCLUDE ( ${CMAKE_MODULE_PATH}/base_definitions.cmake )


##
# Clear variables/caches
##

SET( MINGW_DEFINITIONS "" )
SET( MINGW_INCLUDE_DIR "MINGW_INCLUDE_DIR-NOTFOUND" CACHE FILEPATH "Cleared." FORCE )
SET( MINGW_LIBRARIES "MINGW_LIBRARIES-NOTFOUND" CACHE FILEPATH "Cleared." FORCE )

##
# Defined package variable
##
#TODO: maybe look in the registr for the install path
SET( MINGW_INCLUDE_DIR "C:\MinGW\include" )
SET( MINGW_LIB_DIR "C:\MinGW\lib" )

IF( EXISTS ${MINGW_INCLUDE_DIR} )
  SET( MINGW_FOUND TRUE )
  FOREACH( dir ${MINGW_LIBRARIES} )
    IF ( NOT EXISTS ${dir} )
      SET( MINGW_FOUND FALSE )
    ENDIF ( NOT EXISTS ${dir} )
  ENDFOREACH( dir ${MINGW_LIBRARIES} )
ENDIF( EXISTS ${MINGW_INCLUDE_DIR} )

IF( NOT MINGW_FOUND AND MINGW_FIND_REQUIRED )
  IF( NOT MINGW_INCLUDE_DIR )
        MESSAGE( STATUS "Required include not found" )
    MESSAGE( FATAL_ERROR "Could not find MINGW include!" )
  ENDIF( NOT MINGW_INCLUDE_DIR )
  IF( NOT MINGW_LIBRARIES )
    MESSAGE( STATUS "Required libraries not found" )
    MESSAGE( FATAL_ERROR "Could not find MINGW libraries!" )
  ENDIF( NOT MINGW_LIBRARIES )
ENDIF( NOT MINGW_FOUND AND MINGW_FIND_REQUIRED )


##
# Finally, display informations if not in quiet mode
##

IF( NOT MINGW_FIND_QUIETLY )
  MESSAGE( STATUS "MINGW found " )
  MESSAGE( STATUS "  robot name : ${ROBOT_PREFIX}" )
  MESSAGE( STATUS "  includes   : ${MINGW_INCLUDE_DIR}" )
  MESSAGE( STATUS "  libraries  : ${MINGW_LIBRARIES}" )
  MESSAGE( STATUS "  definitions: ${MINGW_DEFINITIONS}" )
ENDIF( NOT MINGW_FIND_QUIETLY )



MARK_AS_ADVANCED(
  MINGW_INCLUDE_DIR
  MINGW_LIBRARIES
)
