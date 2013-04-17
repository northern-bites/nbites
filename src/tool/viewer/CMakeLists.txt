cmake_minimum_required(VERSION 2.8.3)

set( VIEWER_SRCS
   FieldViewer.cpp
    )

set( VIEWER_HEADERS
   FieldViewer.h
  )

Qt4_wrap_cpp( VIEWER_HEADERS_MOC ${VIEWER_HEADERS} )

add_library(viewer
    STATIC
    ${VIEWER_SRCS}
    ${VIEWER_HEADERS_MOC}
    )


target_link_libraries(
  viewer
  messages
  portals
  support
  tool_common
  ${QT_LIBRARIES}
  )
