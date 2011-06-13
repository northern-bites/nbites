
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
  colorcreator.cpp
  colorzone.cpp
  coloredit.cpp
  colortable.cpp
  stats.cpp )

set(ColorCreator_headers colorcreator.h
    colorzone.h
    coloredit.h
    colortable.h
    stats.h
)

set(ColorCreator_forms colorcreator.ui)

#SET(ColorCreator_resources images.qrc)

include_directories(${CMAKE_CURRENT_BINARY_DIR}
  /usr/include/qt4
  /home/oneamtu/nbites/build/man/straight)

############ Qt4 goodness

Qt4_wrap_cpp(ColorCreator_headers_moc ${ColorCreator_headers})
Qt4_wrap_ui(ColorCreator_forms_headers ${ColorCreator_forms})
#QT4_ADD_RESOURCES(ColorCreator_RESOURCES_RCC ${ColorCreator_RESOURCES})

add_library(colorcreator ${ColorCreator_srcs}
  ${ColorCreator_headers_moc}
  ${ColorCreator_forms_headers} )
#    ${helloworld_RESOURCES_RCC})

target_link_libraries(
  colorcreator
  ${QT_LIBRARIES}
  )
