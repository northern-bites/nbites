cmake_minimum_required(VERSION 2.8.3)

set( IMAGE_SRCS
  YUVtoRGBModule.cpp
  ImageDisplayQModule.cpp
    )

set( IMAGE_HEADERS
  ImageDisplayQModule.h
  )

Qt4_wrap_cpp( IMAGE_HEADERS_MOC ${IMAGE_HEADERS} )

add_library(image
    STATIC
    ${IMAGE_SRCS}
    ${IMAGE_HEADERS_MOC}
    )


target_link_libraries(
  image
  messages
  portals
  ${QT_LIBRARIES}
  )
