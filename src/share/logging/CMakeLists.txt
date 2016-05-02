cmake_minimum_required( VERSION 2.8.3 )

set( LOGSHARE_SRCS Log.cpp SExpr.cpp )

add_library(
  logshare
  STATIC
  ${LOGSHARE_SRCS}
)
