cmake_minimum_required( VERSION 2.8.3 )

set( IMAGE_SRCS
  TranscriberModule.cpp
  ImageConverterModule.cpp
  ImageAcquisition.cpp
  ImageAcquisitionFast.s
  ImageAcquisition.s
)

set_property(SOURCE
  ImageAcquisition.s
  ImageAcquisitionFast.s
  PROPERTY LANGUAGE C )

add_library(
  image
  STATIC
  ${IMAGE_SRCS}
)

target_link_libraries(
  image
  support
  messages
  portals
)