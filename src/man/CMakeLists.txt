project( man )
cmake_minimum_required( VERSION 2.8.3 )

include(cmake/Advanced.cmake)

set( MAN_SRCS main.cpp
)

add_library(
  man
  SHARED
  ${MAN_SRCS}
)

