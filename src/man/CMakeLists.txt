project( man )
cmake_minimum_required( VERSION 2.8.3 )

find_package( Boost REQUIRED )
include( ${NBITES_DIR}/src/man/cmake/FindALCOMMON.cmake )

set( MAN_SRCS main.cpp
)

add_library(
  man
  SHARED
  ${MAN_SRCS}
)


