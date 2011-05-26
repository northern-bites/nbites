
############ RoboImageViewer

project( RoboImageViewer )
cmake_minimum_required(VERSION 2.6.0)

########### Dependencies

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

######### FLAGS

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")

########### Sources

set( RoboImageViewer_srcs main.cpp
  colorcreator.cpp
  RoboImage.cpp
  colorspace.cpp
  colorzone.cpp
  renderarea.cpp
  window.cpp
  coloredit.cpp
  colortable.cpp
  linefit.cpp
  stats.cpp
  FileParser.cpp)

set(RoboImageViewer_headers colorcreator.h
    RoboImage.h
    colorspace.h
    colorzone.h
    renderarea.h
    window.h
    coloredit.h
    colortable.h
    linefit.h
    stats.h
)

set(RoboImageViewer_forms colorcreator.ui)

#SET(RoboImageViewer_resources images.qrc)

include_directories(${CMAKE_CURRENT_BINARY_DIR}
  /home/oneamtu/nbites/build/man/straight)

############ Qt4 goodness

Qt4_wrap_cpp(RoboImageViewer_headers_moc ${RoboImageViewer_headers})
Qt4_wrap_ui(RoboImageViewer_forms_headers ${RoboImageViewer_forms})
#QT4_ADD_RESOURCES(RoboImageViewer_RESOURCES_RCC ${Protoviewer_RESOURCES})

add_executable(RoboImageViewer ${RoboImageViewer_srcs}
  ${RoboImageViewer_headers_moc}
  ${RoboImageViewer_forms_headers} )
#    ${helloworld_RESOURCES_RCC})

link_directories( /home/oneamtu/nbites/build/man/straight/memory/protos )
find_library(PROTOS protos /home/oneamtu/nbites/build/man/straight/memory/protos)

target_link_libraries(
  RoboImageViewer
  protobuf
  ${PROTOS}
  ${QT_LIBRARIES}
  )