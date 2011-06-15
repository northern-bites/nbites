
############ data

project( data )
cmake_minimum_required(VERSION 2.6.0)

########### Dependencies

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

########### Sources

set( data_srcs DataManager.cpp )

set(data_headers  )

include_directories(${CMAKE_CURRENT_BINARY_DIR}
  /usr/include/qt4)

############ Qt4 goodness

Qt4_wrap_cpp(data_headers_moc ${data_headers})
Qt4_wrap_ui(data_forms_headers ${data_forms})

add_library(data
    STATIC
    ${data_srcs}
    ${data_headers_moc}
    ${data_forms_headers} )

link_directories( /home/oneamtu/nbites/build/man/straight/memory/protos )
find_library(PROTOS protos /home/oneamtu/nbites/build/man/straight/memory/protos)

target_link_libraries(
  data
  protobuf
  ${PROTOS}
  ${QT_LIBRARIES}
  )