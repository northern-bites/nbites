
############ ColorCreator

project( colorcreator )
cmake_minimum_required(VERSION 2.6.0)

########### Dependencies

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

######### FLAGS

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")

########### Sources

set( ColorCreator_srcs
  ColorCreator.cpp
  colorzone.cpp
  ColorEdit.cpp
  colortable.cpp
  stats.cpp )

set(ColorCreator_headers ColorCreator.h
    colorzone.h
    ColorEdit.h
    colortable.h
    stats.h
)

set(ColorCreator_forms ColorCreator.ui)

include_directories(${CMAKE_CURRENT_BINARY_DIR}
  /usr/include/qt4
  /home/oneamtu/nbites/build/man/straight)

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
