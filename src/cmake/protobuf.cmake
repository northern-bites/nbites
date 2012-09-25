########################### protobuf

if(DEFINED included_protobuf_cmake)
    return()
else()
    set(included_protobuf_cmake TRUE)
endif()

# Enable ExternalProject CMake module
include(ExternalProject)

# Set default ExternalProject root directory
SET_DIRECTORY_PROPERTIES(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/third_party)

# http://stackoverflow.com/questions/9689183/cmake-googletest
ExternalProject_Add(
    protobuf_ep
    URL http://protobuf.googlecode.com/files/protobuf-2.4.1.tar.gz
    # TIMEOUT 10
    # # Force separate output paths for debug and release builds to allow easy
    # # identification of correct lib in subsequent TARGET_LINK_LIBRARIES commands
    # CMAKE_ARGS -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=DebugLibs
    #            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=ReleaseLibs
    #            -Dgtest_force_shared_crt=ON
    # Disable install step
    INSTALL_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_IN_SOURCE ON
    # Wrap download, configure and build steps in a script to log output
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON)

ExternalProject_Get_Property(protobuf_ep source_dir)
# Custom configure
ExternalProject_Add_Step(
    protobuf_ep conf
    COMMAND ${source_dir}/configure --disable-shared --enable-static
    DEPENDEES download
    DEPENDERS build
    WORKING_DIRECTORY ${source_dir}
)

# Specify include dir
set(PROTOBUF_INCLUDE_DIR ${source_dir}/src CACHE PATH "" FORCE)

# Protoc
set(PROTOBUF_PROTOC_EXECUTABLE ${source_dir}/src/protoc CACHE FILEPATH "" FORCE)

# Library
ExternalProject_Get_Property(protobuf_ep binary_dir)
set(PROTOBUF_LIBRARY_PATH ${binary_dir}/src/.libs/${CMAKE_FIND_LIBRARY_PREFIXES}protobuf.a CACHE FILEPATH "" FORCE)
set(PROTOBUF_LIBRARY protobuf)
add_library(${PROTOBUF_LIBRARY} UNKNOWN IMPORTED)
set_property(TARGET ${PROTOBUF_LIBRARY} PROPERTY IMPORTED_LOCATION
                ${PROTOBUF_LIBRARY_PATH} )
add_dependencies(${PROTOBUF_LIBRARY} protobuf_ep)

mark_as_advanced(
    PROTOBUF_LIBRARY_PATH
    PROTOBUF_PROTOC_EXECUTABLE
    PROTOBUF_INCLUDE_DIR
)