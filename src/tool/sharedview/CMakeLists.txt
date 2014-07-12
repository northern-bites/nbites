cmake_minimum_required(VERSION 2.8.3)

set( VIEWER_SRCS
   SharedViewer.cpp
   SharedViewerPainter.cpp
    )

set( VIEWER_HEADERS
   SharedViewer.h
   SharedViewerPainter.h
  )

Qt4_wrap_cpp( VIEWER_HEADERS_MOC ${VIEWER_HEADERS} )

add_library(
  sharedview
    STATIC
    ${VIEWER_SRCS}
    ${VIEWER_HEADERS_MOC}
    )

target_link_libraries(
  sharedview
  messages
  portals
  tool_common
  support
  localization
  ${QT_LIBRARIES}
  )
