
############ ColorCreator

project( colorcreator )
cmake_minimum_required(VERSION 2.8.3)

########### Dependencies

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

########### Sources

set( ColorCreator_srcs
  ColorCalibrate.cpp
  ColorEdit.cpp
  ColorTable.cpp
  ColorSpaceWidget.cpp
  ColorWheel.cpp
  Stats.cpp
  ColorTableCreator.cpp)

set(ColorCreator_headers
	ColorCalibrate.h
    ColorEdit.h
    ColorSpaceWidget.h
    ColorSpace.h
    ColorWheel.h
    ColorTableCreator.h
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

############ Qt4 goodness

Qt4_wrap_cpp(ColorCreator_headers_moc ${ColorCreator_headers})
Qt4_wrap_ui(ColorCreator_forms_headers ${ColorCreator_forms})

add_library(
    colorcreator
    STATIC
    ${ColorCreator_srcs}
    ${ColorCreator_headers_moc}
    ${ColorCreator_forms_headers} )

target_link_libraries(
  colorcreator
  data
  ${QT_LIBRARIES}
  man

  )
