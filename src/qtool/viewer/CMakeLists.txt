
############ viewer

project( viewer )
cmake_minimum_required(VERSION 2.6.0)

########### Dependencies

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

########### Sources

set( viewer_srcs LogViewer.cpp
                 MObjectViewer.cpp
                 RoboImageViewer.cpp
		 BallEKFViewer.cpp
		 FieldViewer.cpp
		 EKFDataViewer.cpp
		 PaintField.cpp)

set( viewer_headers LogViewer.h
                    MObjectViewer.h
                    RoboImageViewer.h
		    BallEKFViewer.h
		    FieldViewer.h
		    EKFDataViewer.h
		    PaintField.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

############ Qt4 goodness

Qt4_wrap_cpp(viewer_headers_moc ${viewer_headers})
Qt4_wrap_ui(viewer_forms_headers ${viewer_forms})

add_library(viewer
    STATIC
    ${viewer_srcs}
    ${viewer_headers_moc}
    ${viewer_forms_headers} )

target_link_libraries(
  viewer
  treemodel
  image
  memory
  ${QT_LIBRARIES}
  )