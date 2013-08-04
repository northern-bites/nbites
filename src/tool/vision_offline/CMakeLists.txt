cmake_minimum_required(VERSION 2.8.3)

set( VisionOffline_srcs
  VisionDisplayModule.cpp
  )

set(VisionOffline_headers
    VisionDisplayModule.h
)

Qt4_wrap_cpp(VisionOffline_headers_moc ${VisionOffline_headers})

add_library(
    vision_offline
    STATIC
    ${VisionOffline_srcs}
    ${VisionOffline_headers_moc}
    )

target_link_libraries(
  vision_offline
  portals
  messages
  image # man image
  vision # man vision
  ${QT_LIBRARIES}
  )
