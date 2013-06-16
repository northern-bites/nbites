cmake_minimum_required(VERSION 2.8.3)

set( CALIBRATION_SRCS
  CalibrationModule.cpp )

set( CALIBRATION_HEADERS
  CalibrationModule.h )

Qt4_wrap_cpp( CALIBRATION_HEADERS_MOC ${CALIBRATION_HEADERS} )

add_library(
  calibrator
  STATIC
  ${CALIBRATION_SRCS}
  ${CALIBRATION_HEADERS_MOC}
  )

target_link_libraries(
  calibrator
  portals
  messages
  image_display
  vision
  ${QT_LIBRARIES}
)