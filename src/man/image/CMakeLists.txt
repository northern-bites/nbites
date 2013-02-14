cmake_minimum_required( VERSION 2.8.3 )

set( IMAGE_SRCS ColorParams.cpp
  ImageAcquisition.cpp
  ImageTranscriberModule.cpp
  V4L2ImageTranscriber.cpp
  ImageAcquisition.s
  ImageAcquisitionFast.s
  CopyImage.s
)

set_property(SOURCE
  ImageAcquisition.s
  ImageAcquisitionFast.s
  CopyImage.s
  PROPERTY LANGUAGE C )

add_library(
  image
  STATIC
  ${IMAGE_SRCS}
)

target_link_libraries(
  image
  common
  messages
)
