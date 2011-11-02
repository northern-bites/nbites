
############ offline

project( offline )
cmake_minimum_required(VERSION 2.6.0)

########### Dependencies

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

########### Sources

set( offline_srcs OfflineViewer.cpp )

set( offline_headers OfflineViewer.h )

include_directories(${CMAKE_CURRENT_BINARY_DIR})

############ Qt4 goodness

Qt4_wrap_cpp(offline_headers_moc ${offline_headers})
Qt4_wrap_ui(offline_forms_headers ${offline_forms})

add_library(offline
    STATIC
    ${offline_srcs}
    ${offline_headers_moc}
    ${offline_forms_headers} )

target_link_libraries(
  offline
  viewer
  manpreloader
  ${QT_LIBRARIES}
  )
  
add_dependencies( offline man )