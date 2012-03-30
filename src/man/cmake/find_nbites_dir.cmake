
# Finds and sets the NBITES_DIR variable; throws an error otherwise

message(STATUS "Checking for NBITES_DIR ... ")

if( DEFINED NBITES_DIR )
    message(STATUS "Found it, set to ${NBITES_DIR}")
elseif( DEFINED ENV{NBITES_DIR} )
  SET( NBITES_DIR "$ENV{NBITES_DIR}" )
  MESSAGE( STATUS "Found it, set to environment variable ${NBITES_DIR}" )
endif()

if( NOT EXISTS ${NBITES_DIR} )
  message( FATAL_ERROR "Path to the nbites directory does not exist!" )
endif()

# Misc paths
set( NBITES_EXT "${NBITES_DIR}/ext" )