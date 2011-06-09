
############ ProtoViewer

project( ColorTool )
cmake_minimum_required(VERSION 2.6.0)

########### Dependencies

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

######### FLAGS

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")

########### Sources

set( ProtoViewer_srcs main.cpp
  colorcreator.cpp
  roboimage.cpp
  colorspace.cpp
  colorzone.cpp
  coloredit.cpp
  colortable.cpp
  stats.cpp )

set(ProtoViewer_headers colorcreator.h
    roboimage.h
    colorspace.h
    colorzone.h
    coloredit.h
    colortable.h
    stats.h
)

set(ProtoViewer_forms colorcreator.ui)

#SET(ProtoViewer_resources images.qrc)

include_directories(${CMAKE_CURRENT_BINARY_DIR}
  /usr/include/qt4
  /home/oneamtu/nbites/build/man/straight)

############ Qt4 goodness

Qt4_wrap_cpp(ProtoViewer_headers_moc ${ProtoViewer_headers})
Qt4_wrap_ui(ProtoViewer_forms_headers ${ProtoViewer_forms})
#QT4_ADD_RESOURCES(ProtoViewer_RESOURCES_RCC ${Protoviewer_RESOURCES})

add_executable(ColorTool ${ProtoViewer_srcs}
  ${ProtoViewer_headers_moc}
  ${ProtoViewer_forms_headers} )
#    ${helloworld_RESOURCES_RCC})

link_directories( /home/oneamtu/nbites/build/man/straight/memory/protos )
find_library(PROTOS protos /home/oneamtu/nbites/build/man/straight/memory/protos)

target_link_libraries(
  ColorTool
  protobuf
  ${PROTOS}
  ${QT_LIBRARIES}
  )
