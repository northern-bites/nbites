# Finding the protobuf stuff

if( OFFLINE AND NOT EXISTS ${NBITES_DIR}/lib/protobuf_native/lib AND NOT BUILD_PROTOBUF)
    message( FATAL_ERROR "Protobuf libs do not exist for straight compilation. Use BUILD_PROTOBUF to get them." )
endif()
if( NOT OFFLINE AND NOT EXISTS ${NBITES_DIR}/lib/protobuf_cross/lib AND NOT BUILD_PROTOBUF)
    message( FATAL_ERROR "Protobuf libs do not exist for cross compilation. Use BUILD_PROTOBUF to get them." )
endif()

if( OFFLINE )
  set ( PROTOBUF_LOCATION ${NBITES_DIR}/lib/protobuf_native )
else( OFFLINE )
  set ( PROTOBUF_LOCATION ${NBITES_DIR}/lib/protobuf_cross )
endif( OFFLINE )

set( PROTOBUF_INCLUDE_DIR ${PROTOBUF_LOCATION}/include/google )
set( PROTOBUF_LIBRARY ${PROTOBUF_LOCATION}/lib/libprotobuf.a )
set( PROTOBUF_PROTOC_EXECUTABLE ${PROTOBUF_LOCATION}/bin/protoc )

if( PROTOBUF_INCLUDE_DIR AND PROTOBUF_LIBRARY )
  set( PROTOBUF_FOUND TRUE )
endif( PROTOBUF_INCLUDE_DIR AND PROTOBUF_LIBRARY )

if( NOT PROTOBUF_FOUND AND PROTOBUF_FIND_REQUIRED )
  if( NOT PROTOBUF_INCLUDE_DIR )
        message( STATUS "Required include not found" )
    message( FATAL_ERROR "Could not find PROTOBUF include!" )
  endif( NOT PROTOBUF_INCLUDE_DIR )
  if( NOT PROTOBUF_LIBRARY )
    message( STATUS "Required libraries not found" )
    message( FATAL_ERROR "Could not find PROTOBUF libraries!" )
  endif( NOT PROTOBUF_LIBRARY )
endif( NOT PROTOBUF_FOUND AND PROTOBUF_FIND_REQUIRED )

message( STATUS "PROTOBUF found " )
message( STATUS "  includes   : ${PROTOBUF_INCLUDE_DIR}" )
message( STATUS "  libraries  : ${PROTOBUF_LIBRARY}" )
