
############ QTool/Image

project( image )
cmake_minimum_required(VERSION 2.6.0)

########### Dependencies

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

########### Sources

set( Image_srcs
    YUVImage
    BMPYUVImage
    ColorSpace
    ThresholdedImage
    TestImage
    )

set( Image_headers )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

############ Qt4 goodness

Qt4_wrap_cpp(Image_headers_moc ${Image_headers})

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