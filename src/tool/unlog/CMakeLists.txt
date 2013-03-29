cmake_minimum_required( VERSION 2.8.3 )

set( UNLOG_SRCS UnlogModule.cpp )
set( UNLOG_HEADERS UnlogModule.h )

Qt4_wrap_cpp( UNLOG_HEADERS_MOC ${UNLOG_HEADERS} )

add_subdirectory( view )

add_library(
  unlog
  STATIC
  ${UNLOG_SRCS}
  ${UNLOG_HEADERS_MOC}
)

target_link_libraries(
  unlog
  portals
  messages
  logview
  ${PROTOBUF_LIBRARY}
  ${QT_LIBRARIES}
)