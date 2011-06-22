
############ data

project( data )
cmake_minimum_required(VERSION 2.6.0)

########### Dependencies

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

########### Sources

set( data_srcs DataHandler.cpp
               DataManager.cpp
               DataLoader.cpp
               OfflineDataFinder.cpp
               )

set( data_headers DataLoader.h
                  DataFinder.h
                  OfflineDataFinder.h )

include_directories(${CMAKE_CURRENT_BINARY_DIR})

############ Qt4 goodness

Qt4_wrap_cpp(data_headers_moc ${data_headers})
Qt4_wrap_ui(data_forms_headers ${data_forms})

add_subdirectory(treemodel)

add_library(data
    STATIC
    ${data_srcs}
    ${data_headers_moc}
    ${data_forms_headers} )

target_link_libraries(
  data
  treemodel
  memory
  parse
  ${QT_LIBRARIES}
  )