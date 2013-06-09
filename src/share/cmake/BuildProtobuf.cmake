# Download, build, and install all of the PROTOBUF crap
# Enable ExternalProject CMake module
include(ExternalProject)

if( OFFLINE )
  set( PROTO_INSTALL ${NBITES_DIR}/lib/protobuf_native)
else( OFFLINE )
  set( PROTO_INSTALL ${NBITES_DIR}/lib/protobuf_cross)
endif( OFFLINE )

# http://stackoverflow.com/questions/9689183/cmake-googletest
ExternalProject_Add(
    protobuf_libs
    GIT_REPOSITORY http://github.com/northern-bites/protobuf.git
    GIT_TAG 2.5.0-nbites
    SOURCE_DIR ${CMAKE_BINARY_DIR}/protobufsrc
    CONFIGURE_COMMAND ${CMAKE_BINARY_DIR}/protobufsrc/configure -prefix=${PROTO_INSTALL} --with-pic
    INSTALL_DIR ${PROTO_INSTALL}
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON)
