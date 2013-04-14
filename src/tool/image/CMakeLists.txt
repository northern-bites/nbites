cmake_minimum_required(VERSION 2.8.3)

set( IMAGE_SRCS
  ImageDisplayModule.cpp
  Color.cpp
    )

set( IMAGE_HEADERS
  ImageDisplayModule.h
  )

Qt4_wrap_cpp( IMAGE_HEADERS_MOC ${IMAGE_HEADERS} )

add_library(image_display
    STATIC
    ${IMAGE_SRCS}
    ${IMAGE_HEADERS_MOC}
    )


target_link_libraries(
  image_display
  messages
  portals
  ${QT_LIBRARIES}
  )
