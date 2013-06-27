cmake_minimum_required(VERSION 2.8.3)

set( BALL_VIEW_SRCS
   BallViewer.cpp
   BallViewerPainter.cpp
   OfflineBallListener.cpp
    )

set( BALL_VIEW_HEADERS
   BallViewer.h
   BallViewerPainter.h
  )

Qt4_wrap_cpp( BALL_VIEW_HEADERS_MOC ${BALL_VIEW_HEADERS} )

add_library(
  ballview
    STATIC
    ${BALL_VIEW_SRCS}
    ${BALL_VIEW_HEADERS_MOC}
    )

target_link_libraries(
  ballview
  messages
  portals
  tool_common
  support
  balltrack
  ${QT_LIBRARIES}
  )
