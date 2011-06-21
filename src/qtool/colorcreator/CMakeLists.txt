
############ ColorCreator

project( colorcreator )
cmake_minimum_required(VERSION 2.6.0)

########### Dependencies

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

########### Sources

set( ColorCreator_srcs
  ColorCreator.cpp
  ColorZone.cpp
  ColorEdit.cpp
  ColorTable.cpp
  Stats.cpp )

set(ColorCreator_headers ColorCreator.h
    ColorEdit.h
)

set(ColorCreator_forms ColorCreator.ui)

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
  ${QT_LIBRARIES}
  )
