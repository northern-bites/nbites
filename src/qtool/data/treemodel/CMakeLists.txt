
############ treemodel

project( treemodel )
cmake_minimum_required(VERSION 2.6.0)

########### Dependencies

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

########### Sources

set( treemodel_srcs
  TreeModel.cpp
  Node.cpp
  ProtoNode.cpp )

set(treemodel_headers
  TreeModel.h )


include_directories(${CMAKE_CURRENT_BINARY_DIR}
  /usr/include/qt4)

############ Qt4 goodness

Qt4_wrap_cpp(treemodel_headers_moc ${treemodel_headers})
Qt4_wrap_ui(treemodel_forms_headers ${treemodel_forms})


add_library(treemodel
  STATIC
  ${treemodel_srcs}
  ${treemodel_headers_moc}
  ${treemodel_forms_headers} )

link_directories( /home/oneamtu/nbites/build/man/straight/memory/protos )
find_library(PROTOS protos /home/oneamtu/nbites/build/man/straight/memory/protos)

target_link_libraries(
  treemodel
  protobuf
  ${PROTOS}
  ${QT_LIBRARIES}
  )