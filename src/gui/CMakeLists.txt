####### subproject gui

project( gui )
cmake_minimum_required(VERSION 2.8.0)

####### dependencies

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

####### sources

set( gui_srcs ImageView.cpp
  MainView.cpp
)

set( gui_headers ImageView.h
  MainView.h
)

####### qt4

Qt4_wrap_cpp(gui_headers_moc ${gui_headers})

add_library(gui
  STATIC
  ${gui_srcs}
  ${gui_headers_moc} )

target_link_libraries(
  gui
  model
  ${QT_LIBRARIES}
)