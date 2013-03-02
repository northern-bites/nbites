cmake_minimum_required( VERSION 2.8.3 )

#protobuf input - finds all .proto files in the current dir
file(GLOB PROTO_INPUT
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "*.proto")

#protobuf compiler
set(PROTOC_C_OUT_FLAG --cpp_out)

#protobuf output
set(PROTO_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR})
foreach(PROTO_FILE ${PROTO_INPUT})
  #get the name of the file without extension
  get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)
  #add the generated files
  set(PROTO_GEN ${PROTO_GEN}
  ${PROTO_GEN_DIR}/${PROTO_NAME}.pb.h
  ${PROTO_GEN_DIR}/${PROTO_NAME}.pb.cc)
endforeach(PROTO_FILE ${PROTO_INPUT})

# add the custom command that will generate all the files
add_custom_command(
  OUTPUT ${PROTO_GEN}
  COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} ${PROTO_INPUT}
  ${PROTOC_C_OUT_FLAG} ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS ${PROTOBUF_PROTOC_EXECUTABLE} ${PROTO_INPUT}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )

############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project
set( PROTOS_SRCS ${PROTO_GEN} 
		 )

############################ Add sources

# Add static library target
add_library(
  messages
  STATIC
  ${PROTOS_SRCS}
  )

target_link_libraries(
  messages
  ${PROTOBUF_LIBRARY}
  )
