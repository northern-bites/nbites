cmake_minimum_required(VERSION 2.8.3)

set( LOGVIEW_SRCS
  LogViewer.cpp
  ProtoViewer.cpp )

set(LOGVIEW_HEADERS
  LogViewer.h
  ProtoViewer.h )

Qt4_wrap_cpp(LOGVIEW_HEADERS_MOC ${LOGVIEW_HEADERS})

add_subdirectory( treemodel )

add_library(logview
  STATIC
  ${LOGVIEW_SRCS}
  ${LOGVIEW_HEADERS_MOC} )

target_link_libraries(
  logview
  ${PROTOBUF_LIBRARY}
  ${QT_LIBRARIES}
  )