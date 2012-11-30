
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

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32 -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -g")

########### QTool Sources

set( qtool_srcs main.cpp
  EmptyQTool.cpp
  QTool.cpp
  )

set(qtool_headers
  EmptyQTool.h
  QTool.h
)

#set(qtool_forms qtool.ui)

#set(qtool_resources qtool.res)

########### ConnectTool Sources

set( connect_srcs connect_main.cpp
  EmptyQTool.cpp
  ConnectTool.cpp
  )

set( connect_headers
  EmptyQTool.h
  ConnectTool.h
)

include_directories(.
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_BINARY_DIR}/overseer #for the GroundTruth.pb.h
                    ${PROTOBUF_INCLUDE_DIR}
                    )

########## OfflineTool Sources

set( offline_tool_srcs offline_main.cpp
  EmptyQTool.cpp
  OfflineTool.cpp
  )

set( offline_tool_headers
  EmptyQTool.h
  OfflineTool.h
)

include_directories(.
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${PROTOBUF_INCLUDE_DIR}
                    )

############ VSSP

set( vssp_tool_sources vssp/main.cpp
  vssp/Scenes.cpp
  )

set( vssp_tool_headers
  vssp/VSSPTool.h
  )

include_directories(.
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${PROTOBUF_INCLUDE_DIRS}
                    )

############ Qt4 goodness

Qt4_wrap_cpp(qtool_headers_moc ${qtool_headers})
Qt4_wrap_ui(qtool_forms_headers ${qtool_forms})

#### resources example
#Qr4_add_resources(qtool_resources_rcc ${qtool_resources})

Qt4_wrap_cpp(connect_headers_moc ${connect_headers})
Qt4_wrap_cpp(offline_tool_headers_moc ${offline_tool_headers})
Qt4_wrap_cpp(vssp_tool_headers_moc ${vssp_tool_headers})

######### Subdirectory compilation
add_subdirectory( image )
add_subdirectory( colorcreator )
add_subdirectory( data )
add_subdirectory( viewer )
add_subdirectory( offline )
add_subdirectory( remote )
add_subdirectory( overseer )

######### The main QTool executable

add_executable(qtool
  ${qtool_srcs}
  ${qtool_headers_moc}
  ${qtool_forms_headers} )

target_link_libraries(
  qtool
  colorcreator
  image
  data
  viewer
  vision_viewer
  offline
  remote
  overseer_client
  ${QT_LIBRARIES}
  )

########## ConnectTool executable

add_executable( connect_tool
  ${connect_srcs}
  ${connect_headers_moc} )

target_link_libraries(
  connect_tool
  image
  data
  viewer
  remote
  overseer_client
  ${QT_LIBRARIES}
  )

########## OfflineTool executable

add_executable( offline_tool
  ${offline_tool_srcs}
  ${offline_tool_headers_moc} )

target_link_libraries(
  offline_tool
  image
  data
  offline
  viewer
  remote
  ${QT_LIBRARIES}
  )

######### VSSP executable
add_executable( vssp_tool
  ${vssp_tool_sources}
  ${vssp_tool_headers_moc}
  )

target_link_libraries(
  vssp_tool
  parse
  portals
  protos
  _nb_include
  ${QT_LIBRARIES}
)