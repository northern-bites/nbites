project( man )
cmake_minimum_required( VERSION 2.8.3 )

# A list of options that should be advanced
include( cmake/Advanced.cmake )

set( MAN_SRCS main.cpp
  NewMan.cpp
)

add_library(
  man
  SHARED
  ${MAN_SRCS}
)
