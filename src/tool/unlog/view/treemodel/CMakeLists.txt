cmake_minimum_required(VERSION 2.8.3)

set( treemodel_srcs
  TreeModel.cpp
  Node.cpp
  ProtoNode.cpp )

set(treemodel_headers
  TreeModel.h )

Qt4_wrap_cpp(treemodel_headers_moc ${treemodel_headers})


add_library(treemodel
  STATIC
  ${treemodel_srcs}
  ${treemodel_headers_moc} )

target_link_libraries(
  treemodel
  ${PROTOBUF_LIBRARY}
  ${QT_LIBRARIES}
  )