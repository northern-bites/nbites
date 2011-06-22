
############ qtool

project( qtool )
cmake_minimum_required(VERSION 2.8.0)

message(STATUS "")
message(STATUS "...::: Configuring the qtool project :::... ")
message(STATUS "")

########### Dependencies

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

######### FLAGS

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")

########### Sources

set( qtool_srcs main.cpp
  QTool.cpp
  )

set(qtool_headers
    QTool.h
)

#set(qtool_forms qtool.ui)

#set(qtool_resources qtool.res)

include_directories(.
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${PROTOBUF_INCLUDE_DIRS}
                    )

############ Qt4 goodness

Qt4_wrap_cpp(qtool_headers_moc ${qtool_headers})
Qt4_wrap_ui(qtool_forms_headers ${qtool_forms})
#### resources example
#Qr4_add_resources(qtool_resources_rcc ${qtool_resources})


######### Subdirectory compilation
add_subdirectory( image )
add_subdirectory( colorcreator )
add_subdirectory( data )

######### The executable

add_executable(qtool
  ${qtool_srcs}
  ${qtool_headers_moc}
  ${qtool_forms_headers} )

target_link_libraries(
  qtool
  colorcreator
  image
  data
  ${QT_LIBRARIES}
  )

set_property(
  TARGET qtool
  APPEND PROPERTY COMPILE_DEFINITIONS NBITES_DIR=${NBITES_DIR} )