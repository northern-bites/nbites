
############ QTool

project( QTool )
cmake_minimum_required(VERSION 2.6.0)

########### Dependencies

find_package(Qt4 REQUIRED)
find_package(Protobuf REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

######### FLAGS

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")

########### Sources

set( QTool_srcs main.cpp
  QTool.cpp
  )

set(QTool_headers
    QTool.h
)

#set(QTool_forms colorcreator.ui)

#SET(QTool_resources images.qrc)

include_directories(${CMAKE_CURRENT_BINARY_DIR}
                    ${PROTOBUF_INCLUDE_DIRS}
                    .)

############ Qt4 goodness

Qt4_wrap_cpp(QTool_headers_moc ${QTool_headers})
Qt4_wrap_ui(QTool_forms_headers ${QTool_forms})
#QT4_ADD_RESOURCES(QTool_RESOURCES_RCC ${Protoviewer_RESOURCES})

#    ${helloworld_RESOURCES_RCC})

########## MAN LINK
set( MAN_STRAIGHT_BUILD ../../build/man/straight )

include_directories( ../ ../man ${MAN_STRAIGHT_BUILD}/cfg ${MAN_STRAIGHT_BUILD}
    ../man/vision ../man/include ../man/sensors ../man/corpus)
find_library(MEMORY_LIB memory ${MAN_STRAIGHT_BUILD}/memory)
find_library(PARSE_LIB parse ${MAN_STRAIGHT_BUILD}/memory/parse)
find_library(LOG_LIB log ${MAN_STRAIGHT_BUILD}/memory/log)
find_library(VISION_LIB vision ${MAN_STRAIGHT_BUILD}/vision)
find_library(SENSORS_LIB _sensors ${MAN_STRAIGHT_BUILD}/corpus)
find_library(NB_INCLUDE _nb_include ${MAN_STRAIGHT_BUILD}/corpus)
find_library(ROBOT_CONNECT _robot_connect ${MAN_STRAIGHT_BUILD}/corpus)

######### Subdirectory compilation
add_subdirectory( image )
add_subdirectory( colorcreator )
add_subdirectory( data )

######### The executable

add_executable(QTool ${QTool_srcs}
  ${QTool_headers_moc}
  ${QTool_forms_headers} )

target_link_libraries(
  QTool
  colorcreator
  image
  data
  ${MEMORY_LIB}
  ${PARSE_LIB}
  ${LOG_LIB}
  ${ROBOT_CONNECT}
  ${SENSORS_LIB}
  ${NB_INCLUDE}
  ${PROTOBUF_LIBRARIES}
  ${QT_LIBRARIES}
  )