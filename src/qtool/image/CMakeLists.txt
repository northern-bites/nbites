
############ QTool/Image

project( image )
cmake_minimum_required(VERSION 2.6.0)

########### Dependencies

find_package(Qt4 REQUIRED)
find_library(SWSCALE_LIBRARY swscale PATHS)
find_library(AVUTIL_LIBRARY avutil PATHS)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

########### Sources

set( Image_srcs
    YUVImage
    BMPYUVImage
    Color
    ThresholdedImage
    VisualInfoImage
    FastYUVToBMPImage
    PaintField
    PaintLocalization
    )

set( Image_headers BMPImage.h
				   FastYUVToBMPImage.h
				   PaintField.h
				   PaintGroundTruth.h
				   PaintLocalization.h)

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
  overseer_client
  noggin
  ${QT_LIBRARIES}
  ${SWSCALE_LIBRARY}
  ${AVUTIL_LIBRARY}
  )