
# Name of the file is intentionally FindBOOTS, because it calls the module
# FindBoost in cmake modules, so if it were to be named FindBOOST, it would
# only call itself in an infinite loop - Octavian

##
# Includes
##

INCLUDE( ${CMAKE_MODULE_PATH}/base_definitions.cmake )

##
# Clear variables/caches
##
SET( BOOST_DEFINITIONS "" )
SET( BOOST_INCLUDE_DIR "BOOST_INCLUDE_DIR-NOT-FOUND" CACHE FILEPATH "Cleared." FORCE)
SET( BOOST_LIBRARIES "BOOST_LIBRARIES-NOT-FOUND" CACHE FILEPATH "Cleared." FORCE)
##
# Defined package variable
##

IF( OE_CROSS_BUILD )
  SET( BOOST_INCLUDE_DIR "${OE_SYSROOT}/usr/include")
  SET( BOOST_LIBRARIES ${OE_CROSS_DIR}/staging/geode-linux/usr/lib/libboost_python-mt.so
    ${OE_CROSS_DIR}/staging/geode-linux/usr/lib/libboost_signals-mt.so )
ELSE( OE_CROSS_BUILD )
  INCLUDE(FindBoost)
  SET(Boost_USE_MULTITHREADED ON)
  SET(Boost_USE_STATIC_LIBS OFF)
  FIND_PACKAGE( Boost COMPONENTS python signals)
  IF(Boost_FOUND)
    SET (BOOST_INCLUDE_DIR ${Boost_INCLUDE_DIRS})
    SET (BOOST_LIBRARIES ${Boost_LIBRARIES})
  ELSE(Boost_FOUND)
    MESSAGE( FATAL_ERROR "Could not find Boost for straight compilation!")
  ENDIF(Boost_FOUND)
ENDIF( OE_CROSS_BUILD )

# Check if it's found
IF( BOOST_INCLUDE_DIR AND BOOST_LIBRARIES )
  IF ( EXISTS BOOST_INCLUDE_DIR AND EXISTS BOOST_LIBRARIES )
    SET( BOOST_FOUND TRUE )
  ENDIF ( EXISTS BOOST_INCLUDE_DIR AND EXISTS BOOST_LIBRARIES )
ENDIF( BOOST_INCLUDE_DIR AND BOOST_LIBRARIES )

IF( NOT BOOST_FOUND AND BOOST_FIND_REQUIRED )
  IF( NOT BOOST_INCLUDE_DIR OR NOT EXISTS ${BOOST_INCLUDE_DIR} )
    MESSAGE( STATUS "Required include not found" )
    MESSAGE( FATAL_ERROR "Could not find BOOST include!" )
  ENDIF( NOT BOOST_INCLUDE_DIR OR NOT EXISTS ${BOOST_INCLUDE_DIR} )
  IF( NOT BOOST_LIBRARIES OR NOT EXISTS ${BOOST_LIBRARIES} )
    MESSAGE( STATUS "Required libraries not found" )
    MESSAGE( FATAL_ERROR "Could not find BOOST_ libraries!" )
  ENDIF( NOT BOOST_LIBRARIES OR NOT EXISTS ${BOOST_LIBRARIES} )
ENDIF( NOT BOOST_FOUND AND BOOST_FIND_REQUIRED )

# Finally, display informations if not in quiet mode
IF( NOT BOOST_FIND_QUIETLY )
  MESSAGE( STATUS "BOOST found " )
  MESSAGE( STATUS "  includes   : ${BOOST_INCLUDE_DIR}" )
  MESSAGE( STATUS "  libraries  : ${BOOST_LIBRARIES}" )
  MESSAGE( STATUS "  definitions: ${BOOST_DEFINITIONS}" )
ENDIF( NOT BOOST_FIND_QUIETLY )



MARK_AS_ADVANCED(
  BOOST_INCLUDE_DIR
  BOOST_LIBRARIES
  BOOST_DEFINITIONS
  Boost_LIB_DIAGNOSTIC_DEFINITIONS
)
