
############ ProtoViewer

project( ProtoViewer )
cmake_minimum_required(VERSION 2.6.0)

########### Dependencies

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

########### Sources

set( ProtoViewer_srcs main.cpp
  mainwindow.cpp
  TreeModel.cpp
  Node.cpp
  ProtoNode.cpp )

set(ProtoViewer_headers mainwindow.h
  TreeModel.h )

set(ProtoViewer_forms mainwindow.ui)

#SET(ProtoViewer_resources images.qrc)

include_directories(${CMAKE_CURRENT_BINARY_DIR}
  /usr/include/qt4)

############ Qt4 goodness

Qt4_wrap_cpp(ProtoViewer_headers_moc ${ProtoViewer_headers})
Qt4_wrap_ui(ProtoViewer_forms_headers ${ProtoViewer_forms})
#QT4_ADD_RESOURCES(ProtoViewer_RESOURCES_RCC ${Protoviewer_RESOURCES})

add_executable(ProtoViewer ${ProtoViewer_srcs}
  ${ProtoViewer_headers_moc}
  ${ProtoViewer_forms_headers} )
#    ${helloworld_RESOURCES_RCC})

link_directories( /home/oneamtu/nbites/build/man/straight/memory/protos )
find_library(PROTOS protos /home/oneamtu/nbites/build/man/straight/memory/protos)

target_link_libraries(
  ProtoViewer
  protobuf
  ${PROTOS}
  ${QT_LIBRARIES}
  )