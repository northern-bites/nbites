
############ viewer

project( viewer )
cmake_minimum_required(VERSION 2.6.0)

########### Dependencies

find_package(Qt4 REQUIRED)

include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS}) #-DR_HOME=/usr/lib/R)

#find_library(R_LIBRARY R PATHS)
find_library(QWT_LIBRARY qwt PATHS)

########### Sources

set( viewer_srcs MemoryViewer.cpp
                 MObjectViewer.cpp
                 BMPImageViewer.cpp
                 ChannelImageViewer.cpp
		 BallEKFViewer.cpp
		 FieldViewer.cpp
		 EKFDataViewer.cpp
		 GraphViewer.cpp
		 #R/RTool.cpp
		 )

set( viewer_headers MemoryViewer.h
                    MObjectViewer.h
                    BMPImageViewer.h
                    ChannelImageViewer.h
		    BallEKFViewer.h
		    FieldViewer.h
		    EKFDataViewer.h
		    GraphViewer.h
		    #R/RTool.h
		    )

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
  ${QT_QTSVG_LIBRARY}
  #${R_LIBRARY} 
  ${QWT_LIBRARY}

  )