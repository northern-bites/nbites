
############ QTool/Image

project( image )
cmake_minimum_required(VERSION 2.6.0)

########### Dependencies

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

######### FLAGS

#TODO: remove this. have the toolchain instead
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")

########### Sources

set( Image_srcs RoboImageViewer.cpp
    YUVImage.cpp
    BMPYUVImage.cpp
    ColorSpace.cpp
    )

set( Image_headers
    RoboImageViewer.h
)

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

############ Qt4 goodness

Qt4_wrap_cpp(Image_headers_moc ${Image_headers})

########## MAN LINK
set( MAN_STRAIGHT_BUILD ../../build/man/straight )
find_library(MEMORY_LIB memory ${MAN_STRAIGHT_BUILD}/memory)

######### The Library

add_library(image
    STATIC
    ${Image_srcs}
    ${Image_headers_moc}
    )


target_link_libraries(
  image
  ${QT_LIBRARIES}
  )