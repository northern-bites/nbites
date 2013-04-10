# Uses the NBITES_DIR environment variable or otherwise makes an
# educated guess.

if( DEFINED ENV{NBITES_DIR} )
  set( NBITES_DIR "$ENV{NBITES_DIR}" )
  message(STATUS "Found the NBITES_DIR environment variable.")
else( DEFINED ENV{NBITES_DIR} )
  set( NBITES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../" )
  message(STATUS "Guessing a relative directory for NBITES_DIR.")
endif( DEFINED ENV{NBITES_DIR} )

message(STATUS "Set NBITES_DIR to ${NBITES_DIR}.")
message(STATUS "If this is not correct, edit your nbites.bash.")

