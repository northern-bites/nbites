cmake_minimum_required( VERSION 2.8.3 )

#protobuf input - finds all .proto files in the current dir
file(GLOB PROTO_FILES
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  "*.proto")

#protobuf compiler
set(PROTOC_C_OUT_FLAG --cpp_out)

#protobuf output
set(GENERATED_FILE_DIR ${CMAKE_CURRENT_BINARY_DIR})
foreach(PROTO_FILE ${PROTO_FILES})
  #get the name of the file without extension
  get_filename_component(PROTO_NAME ${PROTO_FILE} NAME_WE)
  #add the generated files
  set(GENERATED_SRC_FILES ${GENERATED_SRC_FILES}
  ${GENERATED_FILE_DIR}/${PROTO_NAME}.pb.h
  ${GENERATED_FILE_DIR}/${PROTO_NAME}.pb.cc)

  set(WRAPPED_FILES ${WRAPPED_FILES} ${GENERATED_FILE_DIR}/${PROTO_NAME}.boost.pb.cpp)

endforeach(PROTO_FILE ${PROTO_FILES})

# add the custom command that will generate all the files
add_custom_command(
  OUTPUT ${GENERATED_SRC_FILES}
  COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} ${PROTO_FILES}
  ${PROTOC_C_OUT_FLAG} ${GENERATED_FILE_DIR}
  DEPENDS ${PROTOBUF_PROTOC_EXECUTABLE} ${PROTO_FILES}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )

add_custom_command(
  OUTPUT ${WRAPPED_FILES}
  COMMAND autowrap/wrapifier.py -o ${GENERATED_FILE_DIR} ${PROTO_FILES}
  DEPENDS ${PROTO_FILES}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  )

############################ PROJECT SOURCES FILES
# Add here source files needed to compile this project
set( PROTOS_SRCS ${GENERATED_SRC_FILES} )
set( MESSAGE_SRCS Images.cpp )

############################ Add sources

# Add static library target
add_library(
  messages
  STATIC
  ${PROTOS_SRCS}
  ${MESSAGE_SRCS}
  ${WRAPPED_FILES}
  )

target_link_libraries(
  messages
  ${PROTOBUF_LIBRARY}
  )
