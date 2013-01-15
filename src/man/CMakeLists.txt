project( man )
cmake_minimum_required( VERSION 2.8.3 )

find_package( Boost REQUIRED )

set( MAN_SRCS main.cpp
)

add_library(
  man
  SHARED
  ${MAN_SRCS}
)


