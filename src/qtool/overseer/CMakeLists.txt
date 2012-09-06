
############ overseer

project( overseer )
cmake_minimum_required(VERSION 2.8.0)

########### Dependencies

find_package(Qt4 REQUIRED)
find_package(Protobuf REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

########### GroundTruth ProtoBuf

#protobuf compiler
set(PROTOC_C_OUT_FLAG --cpp_out)

set(ground_truth_proto GroundTruth.proto)
set(ground_truth_output GroundTruth.pb.h GroundTruth.pb.cc)
#generate the ground truth class
add_custom_command(
  OUTPUT ${ground_truth_output}
  COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} ${ground_truth_proto}
  ${PROTOC_C_OUT_FLAG} ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS ${PROTOBUF_PROTOC_EXECUTABLE} ${ground_truth_proto}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )
  
########### Sources

set( overseer_server_srcs OverseerServer.cpp 
                          ${ground_truth_output})


set( overseer_client_srcs OverseerClient.cpp
                          ${ground_truth_output})

set( overseer_client_headers OverseerClient.h )

include_directories(.
                    ${CMAKE_CURRENT_BINARY_DIR}
                    ${PROTOBUF_INCLUDE_DIRS}
                    )

############ Qt4 goodness

Qt4_wrap_cpp(overseer_client_header_moc ${overseer_client_headers})

######### The executable

add_library(
    overseer_server
    SHARED
    ${overseer_server_srcs} )

target_link_libraries(
  overseer_server
  ${PROTOBUF_LIBRARY}
  _nb_include
  log
  data
  ${QT_LIBRARIES}
  )

add_library(
    overseer_client
    STATIC
    ${overseer_client_srcs} ${overseer_client_header_moc} )
    
    
target_link_libraries(
    overseer_client
    memory
    ${PROTOBUF_LIBRARY}
    parse )
