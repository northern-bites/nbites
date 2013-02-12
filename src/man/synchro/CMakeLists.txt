cmake_minimum_required( VERSION 2.8.3 )

# We wrap pthreads stuff in synchro.h
find_package( Threads REQUIRED )

set( SYNCHRO_SRCS synchro.cpp )

add_library(
  synchro
  STATIC
  ${SYNCHRO_SRCS}
)