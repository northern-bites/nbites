
############ viewer

project( viewer )
cmake_minimum_required(VERSION 2.8.3)

########### Dependencies

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS}) #-DR_HOME=/usr/lib/R)

find_library(R_LIBRARY R PATHS)
find_library(QWT_LIBRARY qwt PATHS)

########### Sources

set( viewer_srcs #FieldViewer.cpp
                 PaintField.cpp
                 )

set( viewer_headers #FieldViewer.h
                    PaintField.h
                    )

############ Qt4 goodness

Qt4_wrap_cpp(viewer_headers_moc ${viewer_headers})
Qt4_wrap_ui(viewer_forms_headers ${viewer_forms})

add_library(viewer
    STATIC
    ${viewer_srcs}
    ${viewer_forms_headers} )

target_link_libraries(
  viewer
  ${QT_LIBRARIES}
  )
