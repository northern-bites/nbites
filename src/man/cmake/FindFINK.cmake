# This file locates the fink distribution on a mac, which is where many
# important headers are located (/sw)  (for example, boost1.33)


##
# Clear variables/caches
##

SET( FINK_DEFINITIONS "" )
SET( FINK_INCLUDE_DIR "FINK_INCLUDE_DIR-NOTFOUND" CACHE FILEPATH "Cleared." FORCE )
SET( FINK_LIBRARIES "FINK_LIBRARIES-NOTFOUND" CACHE FILEPATH "Cleared." FORCE )

##
# Defined package variable
##

SET( FINK_INCLUDE_DIR "/sw/include" )
SET( FINK_LIB_DIR "/sw/lib" )

IF( EXISTS ${FINK_INCLUDE_DIR} )
  SET( FINK_FOUND TRUE )
  FOREACH( dir ${FINK_LIBRARIES} )
    IF ( NOT EXISTS ${dir} )
      SET( FINK_FOUND FALSE )
    ENDIF ( NOT EXISTS ${dir} )
  ENDFOREACH( dir ${FINK_LIBRARIES} )
ENDIF( EXISTS ${FINK_INCLUDE_DIR} )

IF( NOT FINK_FOUND AND FINK_FIND_REQUIRED )
  IF( NOT FINK_INCLUDE_DIR )
		MESSAGE( STATUS "Required include not found" )
    MESSAGE( FATAL_ERROR "Could not find FINK include!" )
  ENDIF( NOT FINK_INCLUDE_DIR )
  IF( NOT FINK_LIBRARIES )
  	MESSAGE( STATUS "Required libraries not found" )
    MESSAGE( FATAL_ERROR "Could not find FINK libraries!" )
  ENDIF( NOT FINK_LIBRARIES )
ENDIF( NOT FINK_FOUND AND FINK_FIND_REQUIRED )


##
# Finally, display informations if not in quiet mode
##

IF( NOT FINK_FIND_QUIETLY )
  MESSAGE( STATUS "FINK found " )
  MESSAGE( STATUS "  robot name : ${ROBOT_PREFIX}" )
  MESSAGE( STATUS "  includes   : ${FINK_INCLUDE_DIR}" )
  MESSAGE( STATUS "  libraries  : ${FINK_LIBRARIES}" )
  MESSAGE( STATUS "  definitions: ${FINK_DEFINITIONS}" )
ENDIF( NOT FINK_FIND_QUIETLY )



MARK_AS_ADVANCED(
  FINK_INCLUDE_DIR
  FINK_LIBRARIES
)
