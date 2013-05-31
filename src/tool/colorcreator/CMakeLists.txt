cmake_minimum_required(VERSION 2.8.3)

set( ColorCreator_srcs
  ColorTable.cpp
  ColorTableCreator.cpp
  Stats.cpp
  ColorCalibrate.cpp
  ColorWheel.cpp
  ColorSpaceWidget.cpp
  )

set(ColorCreator_headers
    ColorTableCreator.h
	ColorCalibrate.h
	ColorSpace.h
	ColorWheel.h
	ColorSpaceWidget.h
)

Qt4_wrap_cpp(ColorCreator_headers_moc ${ColorCreator_headers})

add_library(
    colorcreator
    STATIC
    ${ColorCreator_srcs}
    ${ColorCreator_headers_moc}
    )

target_link_libraries(
  colorcreator
  portals
  messages
  image
  ${QT_LIBRARIES}
  )
