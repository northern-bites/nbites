project( man )
cmake_minimum_required( VERSION 2.8.3 )

set( CMAKE_TOOLCHAIN_FILE cmake/atom.cmake )
set( NBITES_DIR  $ENV{NBITES_DIR} )

find_package( Boost REQUIRED )
include( ${NBITES_DIR}/src/man/cmake/FindALCOMMON.cmake )

set( MAN_SRCS main.cpp
)

add_library(
  man
  SHARED
  ${MAN_SRCS}
)


