cmake_minimum_required(VERSION 2.8.3)

set( COMMON_SRCS
   PaintField.cpp
    )

set( COMMON_HEADERS
   PaintField.h
  )


Qt4_wrap_cpp( COMMON_HEADERS_MOC ${COMMON_HEADERS} )


add_library(tool_common
    STATIC
    ${COMMON_SRCS}
    ${COMMON_HEADERS_MOC}
    )


target_link_libraries(
  tool_common
  messages
  portals
  ${QT_LIBRARIES}
  )
